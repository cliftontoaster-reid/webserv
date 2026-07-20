#include "Router.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
#include <utility>

#include "emb.hpp"

namespace mon_router {

Router::Router() {}
Router::Router(const Router& other)
    : _paths(other._paths),
      _handlers(other._handlers),
      _cgiHandler(other._cgiHandler),
      _errors(other._errors),
      _redirects(other._redirects) {}
Router& Router::operator=(const Router& other) {
  if (this != &other) {
    _paths = other._paths;
    _handlers = other._handlers;
    _cgiHandler = other._cgiHandler;
    _errors = other._errors;
    _redirects = other._redirects;
  }
  return *this;
}
Router::~Router() {}

void Router::addRoute(const Route& route) { _paths.push_back(route); }

void Router::addHandler(
    const std::string& path,
    HandlerResponse (*func)(mon_http::AHttpRequest&, mon_http::Form&,
                            const std::map<std::string, toml98::Value>&),
    u_int16_t port, std::map<std::string, toml98::Value> arguments) {
  addHandler(path, func, port, arguments, std::string(""));
}

void Router::addHandler(
    const std::string& path,
    HandlerResponse (*func)(mon_http::AHttpRequest&, mon_http::Form&,
                            const std::map<std::string, toml98::Value>&),
    u_int16_t port, std::map<std::string, toml98::Value> arguments,
    const std::string& hostname) {
  Handler handler;
  handler.path = path;
  handler.func = func;
  handler.port = port;
  handler.arguments = arguments;
  handler.hostname = hostname;
  _handlers.push_back(handler);
}

void Router::addCgi(const std::string& glob, const std::string& cgiBin,
                    u_int16_t port) {
  addCgi(glob, cgiBin, port, std::string(""));
}

void Router::addCgi(const std::string& glob, const std::string& cgiBin,
                    u_int16_t port, const std::string& hostname) {
  _cgiHandler.addGlober(glob, cgiBin, port, hostname);
}

void Router::addErrorPage(const std::string& hostname, u_int16_t port,
                          uint code, const std::string& file) {
  _errors[std::make_pair(hostname, static_cast<int>(port))].files[code] = file;
}

void Router::addRedirect(const Redirect& redirect) {
  _redirects.push_back(redirect);
}

Route Router::find_match(const std::string& request_path,
                         const std::string& hostname, u_int16_t port) const {
  for (size_t i = 0; i < _paths.size(); ++i) {
    if (!_paths[i].hostname.empty() && _paths[i].hostname != hostname) {
      continue;
    }
    if (_paths[i].port == port && _paths[i].is_match(request_path)) {
      return _paths[i];
    }
  }
  throw mon_http::HttpException(STATUS_Not_Found, "Not Found");
}

std::string Router::getFolderTemplate() {
  return std::string(STD_PAGE_FOLDER_raw);
}

}  // namespace mon_router
