#include <exception>
#include <string>

#include "AHttpResponse.hpp"
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

namespace mon_router {

template <int MaxEvents>
void Router::handle(mon_http::AHttpRequest& request, int client_fd,
                    mon_net::Listener<MaxEvents>& listener) {
  try {
    Uri uri(request.path());

    for (size_t i = 0; i < _handlers.size(); ++i) {
      if (_handlers[i].path == uri.path()) {
        invoke_handler(_handlers[i], request, client_fd, listener);
        return;
      }
    }
    const Route& route = find_match(uri.path());
    serve_static_file(route, uri, client_fd, listener);
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
void Router::serve_static_file(const Route& route, const Uri& uri,
                               int client_fd,
                               mon_net::Listener<MaxEvents>& listener) {
  Path path(route.path);
  path.append(uri.path().substr(route.preffix.length()));
  std::string full_path;
  if (!path.resolve(full_path)) {
    throw mon_http::HttpException(STATUS_Not_Found, "Not Found");
  }

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
    mon_http::AHttpRequest& request, int client_fd,
    mon_net::Listener<MAX_EVENTS>& listener);

template void Router::serve_static_file<MAX_EVENTS>(
    const Route& route, const Uri& uri, int client_fd,
    mon_net::Listener<MAX_EVENTS>& listener);

template void Router::invoke_handler<MAX_EVENTS>(
    const Handler& handler, mon_http::AHttpRequest& request, int client_fd,
    mon_net::Listener<MAX_EVENTS>& listener);

}  // namespace mon_router
