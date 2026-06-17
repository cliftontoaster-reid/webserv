#include "Http10Response.hpp"

#include <cstdio>
#include <stdexcept>
#include <string>
#include <vector>

#include "AHttpResponse.hpp"
#include "HttpVersion.hpp"
#include "emb.hpp"

#ifndef WEBSERV_VERSION
#define WEBSERV_VERSION __TIMESTAMP__
#endif

namespace mon_http {

struct Http10HeaderPrinter {
  std::vector<char>& buf;
  explicit Http10HeaderPrinter(std::vector<char>& b) : buf(b) {}

  void operator()(const std::string& key, const std::string& value) const {
    buf.insert(buf.end(), key.begin(), key.end());
    buf.push_back(':');
    buf.insert(buf.end(), value.begin(), value.end());
    buf.push_back('\r');
    buf.push_back('\n');
  }
};

Http10Response::Http10Response() : _code(0) {
  _headers.insert("Server", "WebFloof/" WEBSERV_VERSION);
}
Http10Response::Http10Response(const Http10Response& other)
    : statusMessage(other.statusMessage),
      _body(other._body),
      _headers(other._headers),
      _code(other._code) {}
Http10Response& Http10Response::operator=(const Http10Response& other) {
  if (this != &other) {
    statusMessage = other.statusMessage;
    _body = other._body;
    _headers = other._headers;
    _code = other._code;
  }
  return *this;
}
Http10Response::~Http10Response() {}

std::vector<char> Http10Response::encode() {
  std::vector<char> buf;

  if (statusMessage.empty()) {
    throw std::runtime_error("Empty status message.");
  }

  {
    const std::string s = version().toString();
    buf.insert(buf.end(), s.begin(), s.end());
  }
  buf.push_back(' ');
  {
    char tmp[12];
    int n = std::sprintf(tmp, "%d", _code);
    buf.insert(buf.end(), tmp, tmp + n);
  }
  buf.push_back(' ');
  buf.insert(buf.end(), statusMessage.begin(), statusMessage.end());
  buf.push_back('\r');
  buf.push_back('\n');

  if (hasBody()) {
    char tmp[64];
    int n = std::sprintf(tmp, "content-length:%zu\r\n", _body.length());
    buf.insert(buf.end(), tmp, tmp + n);
  }

  _headers.iter(Http10HeaderPrinter(buf));

  buf.push_back('\r');
  buf.push_back('\n');

  if (hasBody()) {
    buf.insert(buf.end(), _body.begin(), _body.end());
  }

  return buf;
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

bool Http10Response::hasBody() const { return !_body.empty(); }

const std::string& Http10Response::body() const { return _body; }

void Http10Response::setBody(const std::string& body) { _body = body; }

void Http10Response::appendBody(const std::string& str) { _body.append(str); }

std::string& Http10Response::body() { return _body; }

HeaderMap& Http10Response::headers() { return _headers; }

bool Http10Response::hasHeader(const std::string& key) {
  return _headers.contains(key);
}

const std::string& Http10Response::header(const std::string& key) {
  return _headers.at(key);
}

void Http10Response::error500(const std::string& message) {
  _code = STATUS_Internal_Server_Error;
  statusMessage = "Internal Server Error";
  _body = std::string(STD_PAGE_500_raw);

  size_t pos = _body.find("{{ERR_MSG}}");
  if (pos != std::string::npos) _body.replace(pos, 11, message);

  _headers.clear();
  _headers.insert("Cache-Control", "no-store, no-cache, must-revalidate");
  _headers.insert("Content-Type", "text/html");
  _headers.insert("Connection", "close");
  _headers.insert("Retry-After", "60");
}

void Http10Response::error404() {
  _code = STATUS_Not_Found;
  statusMessage = "Not Found";
  _body = std::string(STD_PAGE_404_raw);

  _headers.clear();
  _headers.insert("Cache-Control", "no-store, no-cache, must-revalidate");
  _headers.insert("Content-Type", "text/html");
  _headers.insert("Connection", "close");
  _headers.insert("Retry-After", "60");
}

void Http10Response::ok200() {
  _code = STATUS_OK;
  statusMessage = "OK";

  _headers.clear();
  _headers.insert("Cache-Control", "no-store, no-cache, must-revalidate");
  _headers.insert("Content-Type", "text/html");
  _headers.insert("Connection", "close");
  _headers.insert("Retry-After", "60");
}

}  // namespace mon_http
