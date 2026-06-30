#ifndef WEBSERV_INCLUDE__CONFIG_HPP
#define WEBSERV_INCLUDE__CONFIG_HPP

#include <sys/types.h>

#include <string>
#include <vector>

#include "Value.hpp"

namespace webserv {

struct Route {
  std::string preffix;
  std::string path;

  explicit Route(const toml98::Value&);
};

struct Api {
  std::string uri;
  bool external;
  std::string func;

  explicit Api(const toml98::Value&);
};

struct Cgi {
  std::string glob;
  std::string bin;

  explicit Cgi(const toml98::Value&);
};

struct Server {
  std::vector<u_int16_t> ports;
  std::vector<Route> route;
  std::vector<Api> api;
  std::vector<Cgi> cgi;

  explicit Server(const toml98::Value&);
};

struct Config {
  std::vector<Server> server;

  explicit Config(const toml98::Value&);
};

}  // namespace webserv

#endif