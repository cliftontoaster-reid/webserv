#include "CgiHandler.hpp"
#include <stdatomic.h>
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

bool CgiHandler::isCgi(mon_router::Uri uri) {
	for (size_t i = 0; i < _handles.size(); i++) {
		if (!uri.hasPath()) {
			throw mon_http::HttpException(STATUS_Bad_Request, "No Path");
		}
		if (_handles[i].glob.matches(uri.path())) {
			return true;
		}
	}
	return false;
}

void CgiHandler::addGlober(Glob glob, const std::string& cgiBin) {
	// TODO: Check if cgiBin exists
	Handle res = {glob, cgiBin};
}

void CgiHandler::addGlober(const std::string& glob, const std::string& cgiBin) {
	Glob glob_parsed;
	glob_parsed.compile(glob);

	addGlober(glob_parsed, cgiBin);
}

}  // namespace mon_cgi
