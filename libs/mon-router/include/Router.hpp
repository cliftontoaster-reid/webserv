
#ifndef MON_ROUTER_INCLUDE_ROUTER_HTTP
#define MON_ROUTER_INCLUDE_ROUTER_HTTP

#include <sys/types.h>

#include <string>
#include <vector>

#include "AHttpRequest.hpp"
#include "Listener.hpp"

namespace mon_router {

struct Route {
  std::string path;
  u_int16_t port;
  std::string preffix;

  bool operator<(const Route& other) const {
    return preffix < other.preffix;  // TODO(cliftontoaster-reid) actally do it
  }
};

struct Handler {
  std::string glob;
  std::string cgiBin;
};

class Router {
 public:
  Router();
  Router(const Router& other);
  Router& operator=(const Router& other);
  ~Router();

  void ready() { std::sort(_paths.begin(), _paths.end()); }

  template <int MaxEvents>
  void handle(mon_http::AHttpRequest& request, int client_fd,
              mon_net::Listener<MaxEvents>& listener);

 private:
  std::vector<Route> _paths;
  std::vector<Handler> _handler;
};

}  // namespace mon_router

#endif