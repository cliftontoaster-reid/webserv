#include "Router.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

namespace mon_router {

Router::Router() {}
Router::Router(const Router& other)
    : _paths(other._paths), _handlers(other._handlers) {}
Router& Router::operator=(const Router& other) {
  if (this != &other) {
    _paths = other._paths;
    _handlers = other._handlers;
  }
  return *this;
}
Router::~Router() {}

void Router::addRoute(const std::string& prefix, const std::string& path,
                      u_int16_t port) {
  addRoute(prefix, path, port, std::string("index.html"));
}

void Router::addRoute(const std::string& prefix, const std::string& path,
                      u_int16_t port, const std::string& index) {
  Route route;
  route.preffix = prefix;
  route.path = path;
  route.port = port;
  route.index = index;
  _paths.push_back(route);
}

void Router::addHandler(const std::string& path,
                        HandlerResponse (*func)(mon_http::AHttpRequest&,
                                                mon_http::Form&),
                        u_int16_t port) {
  Handler handler;
  handler.path = path;
  handler.func = func;
  handler.port = port;
  _handlers.push_back(handler);
}

void Router::addCgi(const std::string& glob, const std::string& cgiBin,
                    u_int16_t port) {
  _cgiHandler.addGlober(glob, cgiBin, port);
}

Route Router::find_match(const std::string& request_path,
                         u_int16_t port) const {
  for (size_t i = 0; i < _paths.size(); ++i) {
    if (_paths[i].port == port && _paths[i].is_match(request_path)) {
      return _paths[i];
    }
  }
  throw mon_http::HttpException(STATUS_Not_Found, "Not Found");
}

}  // namespace mon_router