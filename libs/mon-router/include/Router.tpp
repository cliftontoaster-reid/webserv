#include <sys/types.h>

#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "AHttpResponse.hpp"
#include "Detect.hpp"
#include "Form.hpp"
#include "HeaderMap.hpp"
#include "Http10Response.hpp"
#include "HttpException.hpp"
#include "HttpMethod.hpp"
#include "Listener.hpp"
#include "MimeTypes.hpp"
#include "Path.hpp"
#include "Router.hpp"
#include "Uri.hpp"
#include "Value.hpp"

static inline bool isFolder(const std::string& path) {
  struct stat buf;
  return stat(path.c_str(), &buf) == 0 && S_ISDIR(buf.st_mode);
}

static inline bool isFile(const std::string& path) {
  struct stat buf;
  return stat(path.c_str(), &buf) == 0 && S_ISREG(buf.st_mode);
}

namespace mon_router {

template <int MaxEvents>
void Router::handle(mon_http::AHttpRequest& request, u_int16_t port,
                    int client_fd, mon_net::Listener<MaxEvents>& listener) {
  try {
    Uri uri(request.path());

    std::string hostname;
    if (request.hasHost()) {
      const std::string& host = request.host();
      size_t bracket = host.rfind(']');
      size_t colon = host.rfind(':');
      if (colon != std::string::npos &&
          (bracket == std::string::npos || colon > bracket)) {
        hostname = host.substr(0, colon);
      } else {
        hostname = host;
      }
    }

    for (size_t i = 0; i < _handlers.size(); ++i) {
      if (!_handlers[i].hostname.empty() && _handlers[i].hostname != hostname) {
        continue;
      }
      if (_handlers[i].port == port && _handlers[i].path == uri.path()) {
        invoke_handler(_handlers[i], request, client_fd, listener);
        return;
      }
    }

    Route route = find_match(uri.path(), hostname, port);
    Path path(route.path);
    path.append(uri.path().substr(route.preffix.length()));
    std::string full_path;
    if (!path.resolve(full_path)) {
      throw mon_http::HttpException(STATUS_Not_Found, "Not Found");
    }

    if (isFolder(full_path)) {
      if (full_path.at(full_path.length() - 1) != PATH_SEPARATOR) {
        full_path += PATH_SEPARATOR;
      }
      full_path += route.index;
      if (uri.path().at(uri.path().length() - 1) != PATH_SEPARATOR) {
        uri.path() += PATH_SEPARATOR;
      }
      uri.path() += route.index;
    }
    if (!isFile(full_path)) {
      std::cout << "GET 404 " << full_path << std::endl;
      throw mon_http::HttpException(STATUS_Not_Found, "Not Found");
    }

    const mon_cgi::Handle* cgiHandle = _cgiHandler.isCgi(uri, port, hostname);
    if (cgiHandle) {
      Handler cgiH = {full_path, NULL, hostname, port,
                      std::map<std::string, toml98::Value>()};
      _cgiHandler.handleCgi(cgiH, cgiHandle->cgiBin, request, client_fd,
                            listener);
      return;
    }

    serve_static_file(full_path, client_fd, listener);
  } catch (mon_http::HttpException& e) {
    mon_http::Http10Response res;
    res.setError(e.statusCode(), e.what());

    std::map<std::pair<std::string, int>, ErrorMap>::const_iterator iter;
    for (iter = _errors.begin(); iter != _errors.end(); ++iter) {
      const std::string& ruleHost = iter->first.first;
      int rulePort = iter->first.second;

      if (rulePort != port) {
        continue;
      }
      if (request.hasHost()) {
        if (!ruleHost.empty() && request.host() != ruleHost) {
          continue;
        }
      }

      std::map<uint, std::string>::const_iterator fileEntry =
          iter->second.files.find(e.statusCode());
      if (fileEntry == iter->second.files.end()) {
        break;
      }

      std::ifstream file;
      file.open(fileEntry->second.c_str());
      if (!file.is_open()) {
        break;
      }
      std::ostringstream sstr;
      sstr << file.rdbuf();
      file.close();
      res.setBody(sstr.str());
      break;
    }

    listener.markClose(client_fd);
    listener.write(res, client_fd);
  } catch (std::exception& err) {
    mon_http::Http10Response res;
    res.statusMessage = err.what();
    res.error500(err.what());
    listener.markClose(client_fd);
    listener.write(res, client_fd);
  }
}

template <int MaxEvents>
void Router::serve_static_file(const std::string& full_path, int client_fd,
                               mon_net::Listener<MaxEvents>& listener) {
  std::cout << "GET 200 " << full_path << std::endl;
  mon_http::Http10Response res;
  res.ok200();
  res.headers().insert("Content-Type", get_mime_type(full_path));

  std::string disp_path = full_path + "._txt";
  if (isFile(disp_path)) {
    FILE* disp_file = std::fopen(disp_path.c_str(), "rb");
    if (disp_file) {
      std::fseek(disp_file, 0, SEEK_END);
      long disp_size = std::ftell(disp_file);
      std::fseek(disp_file, 0, SEEK_SET);
      if (disp_size > 0) {
        std::string filename(disp_size, '\0');
        if (std::fread(&filename[0], 1, disp_size, disp_file) > 0) {
          filename.erase(filename.find_last_not_of(" \t\r\n") + 1);
          if (!filename.empty()) {
            res.headers().insert("Content-Disposition",
                                 "attachment; filename=\"" + filename + "\"");
          }
        }
      }
      std::fclose(disp_file);
    }
  }

  FILE* file = std::fopen(full_path.c_str(), "rb");
  if (file == NULL) {
    throw mon_http::HttpException(STATUS_Forbidden, "Forbidden");
  }

  std::fseek(file, 0, SEEK_END);  // NOLINT(cert-err33-c)
  long fsize = std::ftell(file);

  std::fseek(file, 0, SEEK_SET);  // NOLINT(cert-err33-c)
  std::string body(fsize, '\0');
  if (fsize > 0 && std::fread(&body[0], 1, fsize, file) == 0) {
    std::fclose(file);  // NOLINT(cert-err33-c)
    throw std::runtime_error("Could not read file content");
  }

  std::fclose(file);  // NOLINT(cert-err33-c)
  res.setBody(body);

  listener.write(res, client_fd);
  listener.markClose(client_fd);
}

template <int MaxEvents>
void Router::invoke_handler(const Handler& handler,
                            mon_http::AHttpRequest& request, int client_fd,
                            mon_net::Listener<MaxEvents>& listener) {
  mon_http::Form form_data;
  if (request.method() == mon_http::HttpMethod::HttpMethodPost) {
    const mon_http::HeaderMap& headers = request.headers();
    if (!headers.contains("Content-Type")) {
      throw mon_http::HttpException(STATUS_Bad_Request, "Bad Request");
    }
    form_data = mon_http::Form(request.header("Content-Type"));
    form_data.parse(request.body());
  }

  HandlerResponse result = handler.func(request, form_data, handler.arguments);

  mon_http::Http10Response res;
  res.setError(result.code, result.message);
  res.setBody(result.body);
  res.headers().extend(result.headers);

  listener.write(res, client_fd);
  listener.markClose(client_fd);
}

template void Router::handle<MAX_EVENTS>(
    mon_http::AHttpRequest& request, u_int16_t port, int client_fd,
    mon_net::Listener<MAX_EVENTS>& listener);

template void Router::serve_static_file<MAX_EVENTS>(
    const std::string& full_path, int client_fd,
    mon_net::Listener<MAX_EVENTS>& listener);

template void Router::invoke_handler<MAX_EVENTS>(
    const Handler& handler, mon_http::AHttpRequest& request, int client_fd,
    mon_net::Listener<MAX_EVENTS>& listener);

}  // namespace mon_router
