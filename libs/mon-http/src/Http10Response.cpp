#include "Http10Response.hpp"

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "HttpVersion.hpp"

namespace mon_http {

struct Http10HeaderPrinter {
  std::ostringstream& oStr;  // NOLINT
  explicit Http10HeaderPrinter(std::ostringstream& oSt) : oStr(oSt) {}

  void operator()(const std::string& key, const std::string& value) const {
    oStr << key << ':' << value << "\r\n";
  }
};

Http10Response::Http10Response() {}
Http10Response::Http10Response(const Http10Response& other)
    : statusMessage(other.statusMessage),
      body(other.body),
      headers(other.headers),
      _code(other._code) {}
Http10Response& Http10Response::operator=(const Http10Response& other) {
  if (this != &other) {
    statusMessage = other.statusMessage;
    body = other.body;
    headers = other.headers;
    _code = other._code;
  }
  return *this;
}
Http10Response::~Http10Response() {}

std::vector<char> Http10Response::encode() {
  std::ostringstream ost;

  if (statusMessage.empty()) {
    throw std::runtime_error("Empty status message.");
  }
  ost << version().toString() << " ";
  ost << statusCode() << " ";
  ost << statusMessage << "\r\n";

  if (hasBody()) {
    std::string& len = headers["Content-Length"];
    std::stringstream sstr;

    sstr << body.length();
    len = sstr.str();
  }

  headers.iter(Http10HeaderPrinter(ost));

  if (hasBody()) {
    ost << "\r\n" << "\r\n";
    ost << body;
  }

  std::string result = ost.str();
  return std::vector<char>(result.begin(), result.end());
}

// NOLINTNEXTLINE
HttpVersion Http10Response::version() const {
  return HttpVersion(HttpVersion::HttpVersion1_0);
}

int Http10Response::statusCode() const { return _code; }

void Http10Response::setStatusCode(int code) {
  if (code < 0 || code > 999) {
    throw std::runtime_error("Status code has to be 3 letters long");
  }
  _code = code;
}

bool Http10Response::hasBody() const { return body.empty(); }

bool Http10Response::hasHeader(const std::string& key) {
  try {
    headers.at(key);
    return true;
  } catch (std::runtime_error& err) {
    (void)err;
    return false;
  }
}

const std::string& Http10Response::header(const std::string& key) {
  return headers.at(key);
}

}  // namespace mon_http
