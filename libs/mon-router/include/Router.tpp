#include <dirent.h>
#include <sys/types.h>

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

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

static inline std::string escapeHtml(const std::string& s) {
  std::ostringstream out;
  for (std::string::const_iterator it = s.begin(); it != s.end(); ++it) {
    switch (*it) {
      case '&':
        out << "&amp;";
        break;
      case '<':
        out << "&lt;";
        break;
      case '>':
        out << "&gt;";
        break;
      case '"':
        out << "&quot;";
        break;
      case '\'':
        out << "&#39;";
        break;
      default:
        out << *it;
        break;
    }
  }
  return out.str();
}

static inline std::string buildFileGrid(
    const std::vector<std::string>& filenames) {
  std::ostringstream html;
  html << "\n";

  for (std::vector<std::string>::const_iterator it = filenames.begin();
       it != filenames.end(); ++it) {
    const std::string& name = *it;
    std::string safeName = escapeHtml(name);

    std::string iconClass = "\xF0\x9F\x93\x84";  // 📄 default
    std::string ext = "";
    size_t dot = name.rfind('.');
    if (dot != std::string::npos) {
      ext = name.substr(dot + 1);
      for (size_t i = 0; i < ext.size(); ++i) {
        ext[i] = std::tolower(ext[i]);
      }
      if (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "gif" ||
          ext == "webp" || ext == "svg" || ext == "bmp" || ext == "ico") {
        iconClass = "\xF0\x9F\x96\xBC\xEF\xB8\x8F";  // 🖼️
      } else if (ext == "mp4" || ext == "mov" || ext == "avi" || ext == "mkv" ||
                 ext == "webm" || ext == "flv" || ext == "m4v" ||
                 ext == "3gp" || ext == "ogv") {
        iconClass = "\xF0\x9F\x8E\xAC\xEF\xB8\x8F";  // 🎬
      } else if (ext == "mp3" || ext == "wav" || ext == "flac" ||
                 ext == "aac") {
        iconClass = "\xF0\x9F\x8E\xB5";  // 🎵
      } else if (ext == "zip" || ext == "tar" || ext == "gz" || ext == "bz2" ||
                 ext == "rar" || ext == "7z") {
        iconClass = "\xF0\x9F\x93\xA6";  // 📦
      } else if (ext == "txt" || ext == "md" || ext == "log") {
        iconClass = "\xF0\x9F\x93\x9D";  // 📝
      } else if (ext == "pdf") {
        iconClass = "\xF0\x9F\x93\x95";  // 📕
      } else if (ext == "html" || ext == "htm" || ext == "php" ||
                 ext == "css" || ext == "js") {
        iconClass = "\xF0\x9F\x8C\x90";  // 🌐
      }
    }

    html << "      <div class=\"bg-white rounded-xl shadow-md overflow-hidden "
            "hover:shadow-xl transition-shadow duration-200\">\n";
    html << "        <div class=\"h-32 bg-gray-100 flex items-center "
            "justify-center text-6xl\">\n";
    html << "          " << iconClass << "\n";
    html << "        </div>\n";
    html << "        <div class=\"p-4\">\n";
    html << "          <a href=\"" << safeName
         << "\" class=\"text-blue-600 hover:underline font-medium block "
            "truncate\" title=\""
         << safeName << "\">\n";
    html << "            " << safeName << "\n";
    html << "          </a>\n";
    html << "        </div>\n";
    html << "      </div>\n";
  }

  return html.str();
}

namespace mon_router {

template <int MaxEvents>
void Router::handle(mon_http::AHttpRequest& request, u_int16_t port,
                    int client_fd, mon_net::Listener<MaxEvents>& listener,
                    int depth) {
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

    if (depth < 5) {
      for (size_t i = 0; i < _redirects.size(); ++i) {
        const Redirect& redir = _redirects[i];
        if (redir.port != port) {
          continue;
        }
        if (!redir.hostname.empty() && redir.hostname != hostname) {
          continue;
        }
        const std::string& req_path = uri.path();
        if (redir.preffix.length() > req_path.length()) {
          continue;
        }
        if (req_path.compare(0, redir.preffix.length(), redir.preffix) != 0) {
          continue;
        }
        if (req_path.length() != redir.preffix.length() &&
            redir.preffix[redir.preffix.length() - 1] != '/' &&
            req_path[redir.preffix.length()] != '/') {
          continue;
        }
        std::string new_path = redir.location;
        if (redir.preffix[redir.preffix.length() - 1] == '/' ||
            req_path[redir.preffix.length()] == '/') {
          new_path += req_path.substr(redir.preffix.length());
        }
        request.path() = new_path;
        handle(request, port, client_fd, listener, depth + 1);
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
    switch (request.method().getType()) {
      case mon_http::HttpMethod::HttpMethodGet:
        if (!route.allowGet) {
          throw mon_http::HttpException(
              STATUS_Forbidden,
              request.method().toString() + " is not allowed here");
        }
        break;
      case mon_http::HttpMethod::HttpMethodPost:
        if (!route.allowPost) {
          throw mon_http::HttpException(
              STATUS_Forbidden,
              request.method().toString() + " is not allowed here");
        }
        break;
      case mon_http::HttpMethod::HttpMethodDelete:
        if (!route.allowDelete) {
          throw mon_http::HttpException(
              STATUS_Forbidden,
              request.method().toString() + " is not allowed here");
        }
        break;

      case mon_http::HttpMethod::HttpMethodUnknown:
      case mon_http::HttpMethod::HttpMethodHead:
      case mon_http::HttpMethod::HttpMethodPut:
      case mon_http::HttpMethod::HttpMethodConnect:
      case mon_http::HttpMethod::HttpMethodOptions:
      case mon_http::HttpMethod::HttpMethodTrace:
      case mon_http::HttpMethod::HttpMethodPatch:
        throw mon_http::HttpException(
            STATUS_Forbidden, "Cannot use " + request.method().toString());
        break;
    }

    if (isFolder(full_path)) {
      if (full_path.at(full_path.length() - 1) != PATH_SEPARATOR) {
        full_path += PATH_SEPARATOR;
      }
      std::string dir_backup = full_path;
      full_path += route.index;
      if (uri.path().at(uri.path().length() - 1) != PATH_SEPARATOR) {
        uri.path() += PATH_SEPARATOR;
      }
      uri.path() += route.index;
      if (!isFile(full_path)) {
        if (route.dirListing) {
          serve_directory_listing(dir_backup, client_fd, listener);
          return;
        }
        throw mon_http::HttpException(STATUS_Forbidden, "Forbidden");
      }
    }
    if (!isFile(full_path)) {
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
void Router::handle_delete(mon_http::AHttpRequest& request, u_int16_t port,
                           int client_fd,
                           mon_net::Listener<MaxEvents>& listener, int depth) {
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

    if (depth < 5) {
      for (size_t i = 0; i < _redirects.size(); ++i) {
        const Redirect& redir = _redirects[i];
        if (redir.port != port) {
          continue;
        }
        if (!redir.hostname.empty() && redir.hostname != hostname) {
          continue;
        }
        const std::string& req_path = uri.path();
        if (redir.preffix.length() > req_path.length()) {
          continue;
        }
        if (req_path.compare(0, redir.preffix.length(), redir.preffix) != 0) {
          continue;
        }
        if (req_path.length() != redir.preffix.length() &&
            redir.preffix[redir.preffix.length() - 1] != '/' &&
            req_path[redir.preffix.length()] != '/') {
          continue;
        }
        std::string new_path = redir.location;
        if (redir.preffix[redir.preffix.length() - 1] == '/' ||
            req_path[redir.preffix.length()] == '/') {
          new_path += req_path.substr(redir.preffix.length());
        }
        request.path() = new_path;
        handle_delete(request, port, client_fd, listener, depth + 1);
        return;
      }
    }

    Route route = find_match(uri.path(), hostname, port);
    if (!route.allowDelete) {
      throw mon_http::HttpException(
          STATUS_Forbidden,
          request.method().toString() + " is not allowed here");
    }

    Path path(route.path);
    path.append(uri.path().substr(route.preffix.length()));
    std::string full_path;
    if (!path.resolve(full_path)) {
      throw mon_http::HttpException(STATUS_Not_Found, "Not Found");
    }

    if (isFolder(full_path)) {
      throw mon_http::HttpException(STATUS_Forbidden, "Forbidden");
    }
    if (!isFile(full_path)) {
      throw mon_http::HttpException(STATUS_Not_Found, "Not Found");
    }

    if (std::remove(full_path.c_str()) != 0) {
      throw mon_http::HttpException(STATUS_Forbidden, "Forbidden");
    }

    mon_http::Http10Response res;
    res.ok200();
    res.setBody("");
    listener.markClose(client_fd);
    listener.write(res, client_fd);
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

  listener.markClose(client_fd);
  listener.write(res, client_fd);
}

template <int MaxEvents>
void Router::serve_directory_listing(const std::string& dir_path, int client_fd,
                                     mon_net::Listener<MaxEvents>& listener) {
  DIR* dir = opendir(dir_path.c_str());
  if (!dir) {
    throw mon_http::HttpException(STATUS_Forbidden, "Forbidden");
  }

  std::vector<std::string> filenames;
  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
    std::string name(entry->d_name);
    if (name == "." || name == "..") {
      continue;
    }
    filenames.push_back(name);
  }
  closedir(dir);

  std::sort(filenames.begin(), filenames.end());

  std::string grid = buildFileGrid(filenames);
  std::string template_html = getFolderTemplate();

  size_t pos = template_html.find("{{FILE_GRID}}");
  if (pos != std::string::npos) {
    template_html.replace(pos, 13, grid);
  }

  mon_http::Http10Response res;
  res.ok200();
  res.headers().insert("Content-Type", "text/html");
  res.setBody(template_html);

  listener.markClose(client_fd);
  listener.write(res, client_fd);
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

  listener.markClose(client_fd);
  listener.write(res, client_fd);
}

template void Router::handle<MAX_EVENTS>(
    mon_http::AHttpRequest& request, u_int16_t port, int client_fd,
    mon_net::Listener<MAX_EVENTS>& listener, int depth);

template void Router::handle_delete<MAX_EVENTS>(
    mon_http::AHttpRequest& request, u_int16_t port, int client_fd,
    mon_net::Listener<MAX_EVENTS>& listener, int depth);

template void Router::serve_static_file<MAX_EVENTS>(
    const std::string& full_path, int client_fd,
    mon_net::Listener<MAX_EVENTS>& listener);

template void Router::serve_directory_listing<MAX_EVENTS>(
    const std::string& dir_path, int client_fd,
    mon_net::Listener<MAX_EVENTS>& listener);

template void Router::invoke_handler<MAX_EVENTS>(
    const Handler& handler, mon_http::AHttpRequest& request, int client_fd,
    mon_net::Listener<MAX_EVENTS>& listener);

}  // namespace mon_router
