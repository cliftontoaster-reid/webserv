
#ifndef MON_ROUTER_INCLUDE_ROUTER_HPP
#define MON_ROUTER_INCLUDE_ROUTER_HPP

#include <sys/types.h>

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "AHttpRequest.hpp"
#include "AHttpResponse.hpp"
#include "CgiHandler.hpp"
#include "Detect.hpp"
#include "Form.hpp"
#include "Handler.hpp"
#include "HttpException.hpp"
#include "Listener.hpp"
#include "Uri.hpp"

#define MAX_EVENTS 1024

namespace mon_router {

std::string random(size_t len) {
  static unsigned char buf[2048];
  static size_t pos = sizeof(buf);

  const std::string charset =
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789";

  std::string result;
  result.reserve(len);

  while (result.length() < len) {
    if (pos >= sizeof(buf)) {
      std::ifstream file("/dev/urandom", std::ios::binary);
      if (!file.is_open()) {
        throw std::runtime_error("Failed to open /dev/urandom");
      }
      file.read(reinterpret_cast<char*>(buf), sizeof(buf));
      pos = 0;
    }

    unsigned char randomByte = buf[pos++];
    result += charset[randomByte % charset.length()];
  }

  return result;
}

inline std::string getNewFileName(const std::string& file_name) {
  size_t last_slash = file_name.find_last_of(PATH_SEPARATOR);

  if (last_slash == std::string::npos) {
    return random(16) + "_" + file_name;
  }

  return file_name.substr(0, last_slash + sizeof(PATH_SEPARATOR)) + random(16) +
         "_" + file_name.substr(last_slash + sizeof(PATH_SEPARATOR));
}

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

class Router {
 public:
  Router();
  Router(const Router& other);
  Router& operator=(const Router& other);
  ~Router();

  void addRoute(const std::string& prefix, const std::string& path,
                u_int16_t port);
  void addHandler(const std::string& path,
                  HandlerResponse (*func)(mon_http::AHttpRequest&,
                                          mon_http::Form&));

  void ready() { std::sort(_paths.begin(), _paths.end()); }

  template <int MaxEvents>
  void handle(mon_http::AHttpRequest& request, int client_fd,
              mon_net::Listener<MaxEvents>& listener);

 private:
  std::vector<Route> _paths;
  std::vector<Handler> _handlers;
  mon_cgi::CgiHandler _cgiHandler;

  const Route& find_match(const std::string& request_path) const {
    for (size_t i = 0; i < _paths.size(); ++i) {
      if (_paths[i].is_match(request_path)) {
        return _paths[i];
      }
    }
    throw mon_http::HttpException(STATUS_Not_Found, "Not Found");
  }

  template <int MaxEvents>
  void serve_static_file(const Route& route, const Uri& uri, int client_fd,
                         mon_net::Listener<MaxEvents>& listener);

  template <int MaxEvents>
  void invoke_handler(const Handler& handler, mon_http::AHttpRequest& request,
                      int client_fd, mon_net::Listener<MaxEvents>& listener);
};

}  // namespace mon_router

#include "Router.tpp"

#endif
