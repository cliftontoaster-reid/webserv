#include "Config.hpp"

#include <cstddef>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "Handler.hpp"
#include "Listener.hpp"
#include "Router.hpp"
#include "funcs.hpp"

namespace {

bool isFile(const std::string& path) {
  struct stat buf;
  return stat(path.c_str(), &buf) == 0 && S_ISREG(buf.st_mode);
}

bool isExec(const std::string& path) { return access(path.c_str(), X_OK) == 0; }

bool isFolder(const std::string& path) {
  struct stat buf;
  return stat(path.c_str(), &buf) == 0 && S_ISDIR(buf.st_mode);
}

}  // namespace

namespace webserv {

Route::Route(const toml98::Value& v) {
  const std::map<std::string, toml98::Value>& t = *v.getTable();
  preffix = *t.find("preffix")->second.getString();
  path = *t.find("path")->second.getString();
}

size_t Route::implement(mon_router::Router& router, u_int16_t port) const {
  if (!isFolder(path)) {
    throw std::invalid_argument("Invalid path '" + path +
                                "' is not a folder or does not exist.");
  }

  router.addRoute(preffix, path, port);
  return 1;
}

Api::Api(const toml98::Value& v) {
  const std::map<std::string, toml98::Value>& t = *v.getTable();
  uri = *t.find("uri")->second.getString();
  external = t.find("external")->second.getBoolean();
  func = *t.find("func")->second.getString();
}

typedef mon_router::HandlerResponse (*HandlerFunc)(mon_http::AHttpRequest&,
                                                   mon_http::Form&);

size_t Api::implement(mon_router::Router& router, u_int16_t port) const {
  HandlerFunc ptr = NULL;

  if (func == "echo" && !external) {
    ptr = echo;
  }

  if (ptr == NULL) {
    throw std::invalid_argument("Invalid function '" + func +
                                "' please check spelling or enable external.");
  }

  router.addHandler(uri, ptr, port);
  return 1;
}

Cgi::Cgi(const toml98::Value& v) {
  const std::map<std::string, toml98::Value>& t = *v.getTable();
  glob = *t.find("glob")->second.getString();
  bin = *t.find("bin")->second.getString();
}

size_t Cgi::implement(mon_router::Router& router, u_int16_t port) const {
  if (!isFile(bin)) {
    throw std::invalid_argument("Cgi '" + bin +
                                "' is not a file or does not exist.");
  }
  if (!isExec(bin)) {
    throw std::invalid_argument("Cgi '" + bin +
                                "' cannot be ran due to permission errors.");
  }

  router.addCgi(glob, bin, port);
  return 1;
}

Server::Server(const toml98::Value& v) {
  const std::map<std::string, toml98::Value>& t = *v.getTable();
  const std::vector<toml98::Value>& arr = *t.find("ports")->second.getArray();

  ports.reserve(arr.size());
  for (std::size_t i = 0; i < arr.size(); ++i) {
    ports.push_back(static_cast<u_int16_t>(arr[i].getInteger()));
  }

  {
    const std::vector<toml98::Value>& arr = *t.find("route")->second.getArray();

    route.reserve(arr.size());
    for (std::size_t i = 0; i < arr.size(); ++i) {
      route.push_back(Route(arr[i]));
    }
  }

  {
    const std::vector<toml98::Value>& arr = *t.find("api")->second.getArray();

    api.reserve(arr.size());
    for (std::size_t i = 0; i < arr.size(); ++i) {
      api.push_back(Api(arr[i]));
    }
  }

  {
    const std::vector<toml98::Value>& arr = *t.find("cgi")->second.getArray();

    cgi.reserve(arr.size());
    for (std::size_t i = 0; i < arr.size(); ++i) {
      cgi.push_back(Cgi(arr[i]));
    }
  }
}

Config::Config(const toml98::Value& root) {
  const std::vector<toml98::Value>& arr =
      *root.getTable()->find("server")->second.getArray();

  server.reserve(arr.size());
  for (std::size_t i = 0; i < arr.size(); ++i) {
    server.push_back(Server(arr[i]));
  }
}

}  // namespace webserv