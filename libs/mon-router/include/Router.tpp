#include <sys/types.h>

#include <exception>
#include <iostream>
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

    for (size_t i = 0; i < _handlers.size(); ++i) {
      if (_handlers[i].port == port && _handlers[i].path == uri.path()) {
        invoke_handler(_handlers[i], request, client_fd, listener);
        return;
      }
    }

    Route route = find_match(uri.path(), port);
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

    const mon_cgi::Handle* cgiHandle = _cgiHandler.isCgi(uri, port);
    if (cgiHandle) {
      Handler cgiH = {full_path, NULL, port};
      _cgiHandler.handleCgi(cgiH, cgiHandle->cgiBin, request, client_fd,
                            listener);
      return;
    }

    serve_static_file(full_path, client_fd, listener);
  } catch (mon_http::HttpException& e) {
    mon_http::Http10Response res;
    res.setError(e.statusCode(), e.what());
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

  HandlerResponse result = handler.func(request, form_data);

  mon_http::Http10Response res;
  res.setError(result.code, result.message);
  res.setBody(result.body);

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
