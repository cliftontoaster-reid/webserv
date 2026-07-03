#include "CgiHandler.hpp"

#include <stdatomic.h>
#include <sys/types.h>

#include <string>

#include "AHttpResponse.hpp"
#include "HttpException.hpp"
#include "Uri.hpp"

namespace mon_cgi {

CgiHandler::CgiHandler() {}
CgiHandler::CgiHandler(const CgiHandler& other) : _handles(other._handles) {}
CgiHandler& CgiHandler::operator=(const CgiHandler& other) {
  if (this != &other) {
    this->_handles = other._handles;
  }
  return *this;
}

const Handle* CgiHandler::isCgi(mon_router::Uri uri, u_int16_t port) const {
  for (size_t i = 0; i < _handles.size(); i++) {
    if (!uri.hasPath()) {
      throw mon_http::HttpException(STATUS_Bad_Request, "No Path");
    }
    if (_handles[i].port == port && _handles[i].glob.matches(uri.path())) {
      return &_handles[i];
    }
  }
  return NULL;
}

void CgiHandler::addGlober(const Glob& glob, const std::string& cgiBin,
                           u_int16_t port) {
  Handle res = {glob, cgiBin, port};
  _handles.push_back(res);
}

void CgiHandler::addGlober(const std::string& glob, const std::string& cgiBin,
                           u_int16_t port) {
  Glob glob_parsed;
  glob_parsed.compile(glob);

  addGlober(glob_parsed, cgiBin, port);
}

}  // namespace mon_cgi
