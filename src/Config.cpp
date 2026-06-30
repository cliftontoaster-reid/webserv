#include "Config.hpp"

#include <map>
#include <string>
#include <vector>

namespace webserv {

Route::Route(const toml98::Value& v) {
  const std::map<std::string, toml98::Value>& t = *v.getTable();
  preffix = *t.find("preffix")->second.getString();
  path = *t.find("path")->second.getString();
}

Api::Api(const toml98::Value& v) {
  const std::map<std::string, toml98::Value>& t = *v.getTable();
  uri = *t.find("uri")->second.getString();
  external = t.find("external")->second.getBoolean();
  func = *t.find("func")->second.getString();
}

Cgi::Cgi(const toml98::Value& v) {
  const std::map<std::string, toml98::Value>& t = *v.getTable();
  glob = *t.find("glob")->second.getString();
  bin = *t.find("bin")->second.getString();
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