#ifndef WEBSERV_INCLUDE__CONFIG_HPP
#define WEBSERV_INCLUDE__CONFIG_HPP

#include <sys/types.h>

#include <cstddef>
#include <map>
#include <string>
#include <vector>

#include "Router.hpp"
#include "Value.hpp"

namespace webserv {

struct Route {
  std::string preffix;
  std::string path;
  std::string index;
  bool allowGet;
  bool allowPost;
  bool allowDelete;
  bool dirListing;

  explicit Route(const toml98::Value&);
  size_t implement(mon_router::Router& router, u_int16_t port,
                   const std::string& hostname) const;
};

struct Api {
  std::string uri;
  bool external;
  std::string func;

  std::map<std::string, toml98::Value> arguments;

  explicit Api(const toml98::Value&);
  size_t implement(mon_router::Router& router, u_int16_t port,
                   const std::string& hostname) const;
};

struct Cgi {
  std::string glob;
  std::string bin;

  explicit Cgi(const toml98::Value&);
  size_t implement(mon_router::Router& router, u_int16_t port,
                   const std::string& hostname) const;
};

struct Host {
  std::string hostname;
  std::map<std::string, std::string> error;
  std::vector<Route> route;
  std::vector<Api> api;
  std::vector<Cgi> cgi;
  std::vector<std::pair<std::string, std::string> > redirects;

  explicit Host(const toml98::Value&);
  size_t implement(mon_router::Router& router, u_int16_t port) const;
};

struct Server {
  u_int16_t port;
  long maxBody;
  std::vector<Host> host;

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