#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

#include "AHttpResponse.hpp"
#include "CgiHandler.hpp"
#include "HeaderMap.hpp"
#include "Http10Response.hpp"
#include "HttpMethod.hpp"

namespace {

inline void freeCStrArray(char** array, size_t size) {
  if (array == NULL) {
    return;
  }
  for (size_t i = 0; i < size; ++i) {
    delete[] array[i];
  }
  delete[] array;
}

inline std::string trimString(const std::string& str) {
  size_t first = str.find_first_not_of(" \t\r\n");
  if (first == std::string::npos) {
    return "";
  }
  size_t last = str.find_last_not_of(" \t\r\n");
  return str.substr(first, (last - first + 1));
}

inline char** buildEnvironment(const std::string& scriptPath,
                               const mon_http::HttpMethod& method,
                               const std::string& requestBody,
                               mon_http::HeaderMap& requestHeaders,
                               size_t& outSize) {
  std::vector<std::string> envStrings;

  envStrings.push_back("GATEWAY_INTERFACE=CGI/1.1");
  envStrings.push_back("REQUEST_METHOD=" + method.toString());
  envStrings.push_back("SCRIPT_FILENAME=" + scriptPath);
  envStrings.push_back("REDIRECT_STATUS=200");

  if (method == mon_http::HttpMethod::HttpMethodPost ||
      method == mon_http::HttpMethod::HttpMethodPut) {
    std::stringstream stringStream;
    stringStream << requestBody.size();
    envStrings.push_back("CONTENT_LENGTH=" + stringStream.str());

    if (requestHeaders.contains("Content-Type")) {
      envStrings.push_back("CONTENT_TYPE=" + requestHeaders["Content-Type"]);
    }
  }

  outSize = envStrings.size();
  char** envp = new char*[outSize + 1];

  for (size_t i = 0; i < outSize; ++i) {
    envp[i] = new char[envStrings[i].size() + 1];
    std::strcpy(envp[i], envStrings[i].c_str());
  }
  envp[outSize] = NULL;
  return envp;
}

inline void handleCgiIO(int writeFd, int readFd, const std::string& requestBody,
                        std::string& outCgiRawOutput) {
  if (!requestBody.empty()) {
    const char* ptr = requestBody.data();
    size_t remaining = requestBody.size();
    while (remaining > 0) {
      ssize_t written = write(writeFd, ptr, remaining);
      if (written <= 0) {
        break;
      }
      ptr += written;
      remaining -= written;
    }
  }
  close(writeFd);  // Send EOF to child script

  char buffer[4096];
  ssize_t bytesRead = 0;
  while ((bytesRead = read(readFd, buffer, sizeof(buffer))) > 0) {
    outCgiRawOutput.append(buffer, bytesRead);
  }
  close(readFd);
}

namespace {
inline std::string statusPhrase(int code) {
  switch (code) {
    case 200: return "OK";
    case 201: return "Created";
    case 204: return "No Content";
    case 301: return "Moved Permanently";
    case 302: return "Found";
    case 304: return "Not Modified";
    case 400: return "Bad Request";
    case 401: return "Unauthorized";
    case 403: return "Forbidden";
    case 404: return "Not Found";
    case 413: return "Request Entity Too Large";
    case 414: return "Request-URI Too Long";
    case 500: return "Internal Server Error";
    case 501: return "Not Implemented";
    case 502: return "Bad Gateway";
    case 503: return "Service Unavailable";
    case 505: return "HTTP Version Not Supported";
    default:  return "";
  }
}
}

inline void parseCgiResponse(const std::string& rawOutput,
                             mon_http::AHttpResponse& outResponse) {
  size_t headerEnd = rawOutput.find("\r\n\r\n");
  if (headerEnd == std::string::npos) {
    headerEnd = rawOutput.find("\n\n");
    if (headerEnd == std::string::npos) {
      outResponse.error500(
          "Internal Server Error: Malformed CGI response headers.");
      return;
    }
  }

  std::string headersPart = rawOutput.substr(0, headerEnd);
  size_t delimiterLen =
      (rawOutput.find("\r\n\r\n") != std::string::npos) ? 4 : 2;
  std::string bodyPart = rawOutput.substr(headerEnd + delimiterLen);

  std::stringstream headerStream(headersPart);
  std::string line;
  int parsedStatusCode = STATUS_OK;

  while (std::getline(headerStream, line)) {
    if (!line.empty() && line[line.size() - 1] == '\r') {
      line.resize(line.size() - 1);
    }
    if (line.empty()) {
      continue;
    }

    size_t colon = line.find(':');
    if (colon != std::string::npos) {
      std::string key = trimString(line.substr(0, colon));
      std::string val = trimString(line.substr(colon + 1));

      if (key == "Status") {
        std::stringstream ss(val);
        ss >> parsedStatusCode;
      } else {
        outResponse.headers().insert(key, val);
      }
    }
  }

  outResponse.setStatusCode(parsedStatusCode);
  outResponse.setStatusPhrase(statusPhrase(parsedStatusCode));
  outResponse.setBody(bodyPart);
}

inline bool executeCgi(const std::string& scriptPath,
                       const std::string& cgiBin,
                       const mon_http::HttpMethod& method,
                       const std::string& requestBody,
                       mon_http::HeaderMap& requestHeaders,
                       mon_http::AHttpResponse& outResponse) {
  int cgiInputPipe[2];
  int cgiOutputPipe[2];

  if (pipe(cgiInputPipe) < 0 || pipe(cgiOutputPipe) < 0) {
    outResponse.error500("Internal Server Error: Pipe creation failed.");
    return false;
  }

  size_t envSize = 0;
  char** envp = buildEnvironment(scriptPath, method, requestBody,
                                 requestHeaders, envSize);

  const std::string& execPath = cgiBin.empty() ? scriptPath : cgiBin;
  int argc = cgiBin.empty() ? 1 : 2;
  char** argv = new char*[argc + 1];
  argv[0] = new char[execPath.size() + 1];
  std::strcpy(argv[0], execPath.c_str());
  if (!cgiBin.empty()) {
    argv[1] = new char[scriptPath.size() + 1];
    std::strcpy(argv[1], scriptPath.c_str());
  }
  argv[argc] = NULL;

  pid_t pid = fork();
  if (pid < 0) {
    freeCStrArray(envp, envSize);
    freeCStrArray(argv, 1);
    outResponse.error500("Internal Server Error: Fork failed.");
    return false;
  }

  if (pid == 0) {
    // --- CHILD PROCESS ---
    dup2(cgiInputPipe[0], STDIN_FILENO);
    dup2(cgiOutputPipe[1], STDOUT_FILENO);

    close(cgiInputPipe[1]);
    close(cgiOutputPipe[0]);
    close(cgiInputPipe[0]);
    close(cgiOutputPipe[1]);

    execve(argv[0], argv, envp);
    std::exit(EXIT_FAILURE);
  }

  // --- PARENT PROCESS ---
  freeCStrArray(argv, argc);
  freeCStrArray(envp, envSize);

  close(cgiInputPipe[0]);
  close(cgiOutputPipe[1]);

  std::string cgiRawOutput;
  handleCgiIO(cgiInputPipe[1], cgiOutputPipe[0], requestBody, cgiRawOutput);

  int status = 0;
  waitpid(pid, &status, 0);

  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    outResponse.error500(
        "Internal Server Error: CGI script terminated abnormally.");
    return false;
  }

  parseCgiResponse(cgiRawOutput, outResponse);
  return true;
}

}  // namespace

namespace mon_cgi {

template <int MaxEvents>
void CgiHandler::handleCgi(const mon_router::Handler& handler,
                           const std::string& cgiBin,
                           mon_http::AHttpRequest& request, int client_fd,
                           mon_net::Listener<MaxEvents>& listener) {
  mon_http::Http10Response res;

  executeCgi(handler.path, cgiBin, request.method(), request.body(),
             request.headers(), res);

  listener.write(res, client_fd);
  listener.markClose(client_fd);
}

template void CgiHandler::handleCgi<1024>(const mon_router::Handler& handler,
                                          const std::string& cgiBin,
                                          mon_http::AHttpRequest& request,
                                          int client_fd,
                                          mon_net::Listener<1024>& listener);

}  // namespace mon_cgi