#include "Router.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

namespace mon_router {

Router::Router() {}
Router::Router(const Router& other) { (void)other; }
Router& Router::operator=(const Router& other) {
  if (this != &other) {
  }
  return *this;
}
Router::~Router() {}

void Router::addRoute(const std::string& prefix, const std::string& path,
                      u_int16_t port) {
  Route route;
  route.preffix = prefix;
  route.path = path;
  route.port = port;
  _paths.push_back(route);
}

void Router::addHandler(const std::string& path,
                        HandlerResponse (*func)(mon_http::AHttpRequest&,
                                                mon_http::Form&)) {
  Handler handler;
  handler.path = path;
  handler.func = func;
  _handlers.push_back(handler);
}

}  // namespace mon_router