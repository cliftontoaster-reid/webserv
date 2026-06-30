#ifndef WEBSERV_INCLUDE__CONFIG_HPP
#define WEBSERV_INCLUDE__CONFIG_HPP

#include <sys/types.h>

#include <cstddef>
#include <string>
#include <vector>

#include "Router.hpp"
#include "Value.hpp"

namespace webserv {

struct Route {
  std::string preffix;
  std::string path;

  explicit Route(const toml98::Value&);
  size_t implement(mon_router::Router& router, u_int16_t port) const;
};

struct Api {
  std::string uri;
  bool external;
  std::string func;

  explicit Api(const toml98::Value&);
  size_t implement(mon_router::Router& router, u_int16_t port) const;
};

struct Cgi {
  std::string glob;
  std::string bin;

  explicit Cgi(const toml98::Value&);
  size_t implement(mon_router::Router& router, u_int16_t port) const;
};

struct Server {
  std::vector<u_int16_t> ports;
  std::vector<Route> route;
  std::vector<Api> api;
  std::vector<Cgi> cgi;

  explicit Server(const toml98::Value&);
  template <int MaxEvents>
  size_t implement(mon_router::Router& router, std::vector<u_int16_t>& ports,
                   mon_net::Listener<MaxEvents>& listener) const;
};

struct Config {
  std::vector<Server> server;

  explicit Config(const toml98::Value&);
  template <int MaxEvents>
  size_t implement(mon_router::Router& router,
                   mon_net::Listener<MaxEvents>& listener) const;
};

}  // namespace webserv

#include "Config.tpp"

#endif