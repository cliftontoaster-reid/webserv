
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
    // 1. Sort by length descending (longer prefixes first)
    if (preffix.length() != other.preffix.length()) {
      return preffix.length() > other.preffix.length();
    }
    // 2. Tie-breaker: alphabetical order ascending
    return preffix < other.preffix;
  }

  bool is_match(const std::string& request_path) const {
    if (preffix.length() > request_path.length()) {
      return false;
    }
    if (request_path.compare(0, preffix.length(), preffix) != 0) {
      return false;
    }
    if (request_path.length() == preffix.length()) {
      return true;
    }

    if (preffix[preffix.length() - 1] == '/') {
      return true;
    }
    if (request_path[preffix.length()] == '/') {
      return true;
    }

    return false;
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

  void addRoute(const std::string& prefix, const std::string& path,
                u_int16_t port);
  void ready() { std::sort(_paths.begin(), _paths.end()); }

  template <int MaxEvents>
  void handle(mon_http::AHttpRequest& request, int client_fd,
              mon_net::Listener<MaxEvents>& listener);

 private:
  std::vector<Route> _paths;
  std::vector<Handler> _handler;

  const Route* find_match(const std::string& request_path) const {
    for (size_t i = 0; i < _paths.size(); ++i) {
      if (_paths[i].is_match(request_path)) {
        return &_paths[i];
      }
    }
    return NULL;
  }
};

}  // namespace mon_router

#endif