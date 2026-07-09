#include "Config.hpp"

#include <cstddef>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "Handler.hpp"
#include "Listener.hpp"
#include "Router.hpp"
#include "Value.hpp"
#include "funcs.hpp"

namespace {

bool isExec(const std::string& path) { return access(path.c_str(), X_OK) == 0; }

}  // namespace

namespace webserv {

static inline bool isFolder(const std::string& path) {
  struct stat buf;
  return stat(path.c_str(), &buf) == 0 && S_ISDIR(buf.st_mode);
}

Route::Route(const toml98::Value& v) {
  const std::map<std::string, toml98::Value>& t = *v.getTable();
  preffix = *t.find("preffix")->second.getString();
  path = *t.find("path")->second.getString();

  std::map<std::string, toml98::Value>::const_iterator iterIndex =
      t.find("index");
  if (iterIndex != t.end()) {
    index = *iterIndex->second.getString();
  } else {
    index = "index.html";
  }
}

size_t Route::implement(mon_router::Router& router, u_int16_t port,
                        const std::string& hostname) const {
  if (!isFolder(path)) {
    throw std::invalid_argument("Invalid path '" + path +
                                "' is not a folder or does not exist.");
  }

  router.addRoute(preffix, path, port, index, hostname);
  return 1;
}

Api::Api(const toml98::Value& v) {
  const std::map<std::string, toml98::Value>& t = *v.getTable();
  uri = *t.find("uri")->second.getString();
  external = t.find("external")->second.getBoolean();
  func = *t.find("func")->second.getString();

  if (!external && func == "upload") {
    std::map<std::string, toml98::Value>::const_iterator it = t.find("upload");
    if (it == t.end() || it->second.type() != toml98::ValueTable) {
      throw std::invalid_argument("server[].api[].upload needs to be a table");
    }
    const std::map<std::string, toml98::Value>& upload_t =
        *it->second.getTable();

    it = upload_t.find("path");
    if (it == upload_t.end() || it->second.type() != toml98::ValueString) {
      throw std::invalid_argument(
          "server[].api[].upload.path needs to be a string");
    }
    arguments.insert(
        std::make_pair(std::string("path"), toml98::Value(it->second)));

    it = upload_t.find("random_len");
    if (it == upload_t.end() || it->second.type() != toml98::ValueInteger) {
      throw std::invalid_argument(
          "server[].api[].upload.random_len needs to be an integer");
    }
    arguments.insert(
        std::make_pair(std::string("random_len"), toml98::Value(it->second)));
  }
}

typedef mon_router::HandlerResponse (*HandlerFunc)(
    mon_http::AHttpRequest&, mon_http::Form&,
    const std::map<std::string, toml98::Value>&);

size_t Api::implement(mon_router::Router& router, u_int16_t port,
                      const std::string& hostname) const {
  HandlerFunc ptr = NULL;

  if (func == "echo" && !external) {
    ptr = echo;
  } else if (func == "upload" && !external) {
    ptr = upload;
  }

  if (ptr == NULL) {
    throw std::invalid_argument("Invalid function '" + func +
                                "' please check spelling or enable external.");
  }

  router.addHandler(uri, ptr, port, arguments, hostname);
  return 1;
}

Cgi::Cgi(const toml98::Value& v) {
  const std::map<std::string, toml98::Value>& t = *v.getTable();
  glob = *t.find("glob")->second.getString();
  bin = *t.find("bin")->second.getString();
}

size_t Cgi::implement(mon_router::Router& router, u_int16_t port,
                      const std::string& hostname) const {
  if (!isFile(bin)) {
    throw std::invalid_argument("Cgi '" + bin +
                                "' is not a file or does not exist.");
  }
  if (!isExec(bin)) {
    throw std::invalid_argument("Cgi '" + bin +
                                "' cannot be ran due to permission errors.");
  }

  router.addCgi(glob, bin, port, hostname);
  return 1;
}

size_t Host::implement(mon_router::Router& router, u_int16_t port) const {
  size_t nbrRules = 0;

  for (std::map<std::string, std::string>::const_iterator it = error.begin();
       it != error.end(); ++it) {
    uint code = static_cast<uint>(std::strtol(it->first.c_str(), NULL, 10));
    if (!isFile(it->second)) {
      throw std::invalid_argument("Error page '" + it->second +
                                  "' is not a file or does not exist.");
    }
    if (access(it->second.c_str(), R_OK) != 0) {
      throw std::invalid_argument("Error page '" + it->second +
                                  "' is not readable.");
    }
    router.addErrorPage(hostname, port, code, it->second);
    nbrRules++;
  }

  for (std::vector<Route>::const_iterator it = route.begin(); it != route.end();
       ++it) {
    nbrRules += it->implement(router, port, hostname);
  }
  for (std::vector<Api>::const_iterator it = api.begin(); it != api.end();
       ++it) {
    nbrRules += it->implement(router, port, hostname);
  }
  for (std::vector<Cgi>::const_iterator it = cgi.begin(); it != cgi.end();
       ++it) {
    nbrRules += it->implement(router, port, hostname);
  }

  return nbrRules;
}

Host::Host(const toml98::Value& v) {
  const std::map<std::string, toml98::Value>& t = *v.getTable();
  hostname = *t.find("hostname")->second.getString();

  std::map<std::string, toml98::Value>::const_iterator errIt = t.find("error");
  if (errIt != t.end()) {
    const std::map<std::string, toml98::Value>& errMap =
        *errIt->second.getTable();
    for (std::map<std::string, toml98::Value>::const_iterator it =
             errMap.begin();
         it != errMap.end(); ++it) {
      error.insert(std::make_pair(it->first, *it->second.getString()));
    }
  }

  const std::vector<toml98::Value>& routeArr =
      *t.find("route")->second.getArray();
  for (std::size_t i = 0; i < routeArr.size(); ++i) {
    route.push_back(Route(routeArr[i]));
  }

  const std::vector<toml98::Value>& apiArr =
      *t.find("api")->second.getArray();
  for (std::size_t i = 0; i < apiArr.size(); ++i) {
    api.push_back(Api(apiArr[i]));
  }

  const std::vector<toml98::Value>& cgiArr =
      *t.find("cgi")->second.getArray();
  for (std::size_t i = 0; i < cgiArr.size(); ++i) {
    cgi.push_back(Cgi(cgiArr[i]));
  }
}

Server::Server(const toml98::Value& v) {
  const std::map<std::string, toml98::Value>& t = *v.getTable();
  port = static_cast<u_int16_t>(t.find("port")->second.getInteger());

  const std::vector<toml98::Value>& arr = *t.find("host")->second.getArray();
  host.reserve(arr.size());
  for (std::size_t i = 0; i < arr.size(); ++i) {
    host.push_back(Host(arr[i]));
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