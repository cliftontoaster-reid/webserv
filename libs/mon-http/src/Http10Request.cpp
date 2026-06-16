#include "Http10Request.hpp"

#include <algorithm>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "AHttpRequest.hpp"
#include "HttpMethod.hpp"
#include "HttpVersion.hpp"

#ifndef WEBSERV_VERSION
#define WEBSERV_VERSION __TIMESTAMP__
#endif

namespace mon_http {

struct IsLineTerminator {
  bool operator()(char c) const { return c == '\r' || c == '\n'; }
};

static inline std::vector<char>::const_iterator findLine(
    const std::vector<char>::const_iterator& begin,
    const std::vector<char>::const_iterator& end) {
  return std::find_if(begin, end, IsLineTerminator());
}

static inline size_t _newPos(std::vector<char>::const_iterator lineEnd,
                             const std::vector<char>& data) {
  size_t termLen = 1;

  if (lineEnd + 1 != data.end()) {
    char first = *lineEnd;
    char second = *(lineEnd + 1);
    if ((first == '\r' && second == '\n') ||
        (first == '\n' && second == '\r')) {
      termLen = 2;
    }
  }

  return static_cast<size_t>(lineEnd - data.begin()) + termLen;
}

static inline std::pair<HttpMethod, std::string> _parseHeaderLine(
    const std::vector<char>& data, size_t& pos) {
  std::vector<char>::const_iterator lineStart = data.begin() + pos;
  std::vector<char>::const_iterator lineEnd = findLine(lineStart, data.end());

  if (lineEnd == data.end()) {
    throw std::runtime_error("Malformed HTTP request: missing line terminator");
  }

  std::vector<char>::const_iterator itSpace1 =
      std::find(lineStart, lineEnd, ' ');
  if (itSpace1 == lineEnd) {
    throw std::runtime_error(
        "Malformed HTTP request: missing space after method");
  }

  std::vector<char>::const_iterator itSpace2 =
      std::find(itSpace1 + 1, lineEnd, ' ');
  if (itSpace2 == lineEnd) {
    throw std::runtime_error(
        "Malformed HTTP request: missing space after path");
  }

  std::string method(lineStart, itSpace1);
  std::string path(itSpace1 + 1, itSpace2);

  pos = _newPos(lineEnd, data);

  return std::make_pair(HttpMethod::fromString(method), path);
}

static inline std::pair<std::string, std::string> _parseHeaderLine(
    const std::string& line) {
  // ---- key -------------------------------------------------------------
  std::size_t keyPos = line.find_first_not_of(" \n\r");
  if (keyPos == std::string::npos) {
    throw std::runtime_error("Malformed HTTP request: missing header key");
  }

  std::size_t middlePos = line.find_first_of(" :", keyPos);
  if (middlePos == std::string::npos) {
    throw std::runtime_error(
        "Malformed HTTP request: header key not terminated");
  }

  std::string key = line.substr(keyPos, middlePos - keyPos);

  // ---- value -----------------------------------------------------------
  std::size_t valuePos = line.find_first_not_of(' ', middlePos + 1);
  if (valuePos == std::string::npos) {
    throw std::runtime_error("Malformed HTTP request: missing header value");
  }

  std::size_t endPos = line.find_first_of("\r\n", valuePos);
  if (endPos == std::string::npos) {
    endPos = line.size();
  }

  std::string value = line.substr(valuePos, endPos - valuePos);

  std::size_t lastNonSpace = value.find_last_not_of(" \t\f\v\n\r");
  if (lastNonSpace != std::string::npos) {
    value.erase(lastNonSpace + 1);
  }

  return std::make_pair(key, value);
}

Http10Request::Http10Request() {
  _headers.insert("Server", "WebFloof/" WEBSERV_VERSION);
}
Http10Request::Http10Request(const Http10Request& other)
    : _method(other._method),
      _path(other._path),
      _body(other._body),
      _headers(other._headers) {}
Http10Request& Http10Request::operator=(const Http10Request& other) {
  if (this != &other) {
    _method = other._method;
    _path = other._path;
    _body = other._body;
    _headers = other._headers;
  }
  return *this;
}
Http10Request::~Http10Request() {}

void Http10Request::parse(const std::vector<char>& data) {
  HttpVersion::Type ver = HttpVersion::sniffHttpVersion(data).value;
  if (ver != HttpVersion::HttpVersion1_0 && ver != HttpVersion::HttpVersion1_1) {
    throw std::runtime_error("Received wrong version of HTTP");
  }
  size_t pos = 0;

  std::pair<HttpMethod, std::string> headerLine = _parseHeaderLine(data, pos);
  this->_method = headerLine.first;
  this->_path = headerLine.second;

  while (pos < data.size()) {
    std::vector<char>::const_iterator lineStart = data.begin() + pos;
    std::vector<char>::const_iterator lineEnd = findLine(lineStart, data.end());

    if (lineStart == lineEnd) {
      pos = _newPos(lineEnd, data);
      break;
    }
    std::string line(lineStart, lineEnd);

    std::pair<std::string, std::string> header = _parseHeaderLine(line);
    this->_headers.insert(header.first, header.second);

    pos = _newPos(lineEnd, data);
  }

  if (pos < data.size()) {
    this->_body.assign(data.begin() + pos, data.end());
  }
  if (this->hasHeader("Content-Length")) {
    std::string lengthStr = this->header("Content-Length");
    std::stringstream ssStr(lengthStr);
    size_t claimedLength = 0;

    ssStr >> claimedLength;

    if (ssStr.fail()) {
      throw std::runtime_error(
          "Malformed HTTP request: malformed Content-Length header");
    }

    if (this->_body.size() != claimedLength) {
      throw EndedTooEarly();
    }
  } else {
    if (!_body.empty()) {
      throw std::runtime_error("Cannot have a body without Content-Length");
    }
  }
}

// NOLINTNEXTLINE
HttpVersion Http10Request::version() const {
  return HttpVersion(HttpVersion::HttpVersion1_0);
}

HttpMethod Http10Request::method() const { return _method; }

const std::string& Http10Request::path() const { return _path; }

bool Http10Request::hasBody() const { return !_body.empty(); }

const std::string& Http10Request::body() const { return _body; }

const std::string& Http10Request::mediaType() {
  if (!hasHeader("Content-Type")) {
    throw std::runtime_error(
        "Request does not have 'Content-Type' in its headers");
  }

  return header("Content-Type");
}

bool Http10Request::hasHeader(const std::string& key) {
  try {
    _headers.at(key);
    return true;
  } catch (std::out_of_range& err) {
    (void)err;
    return false;
  }
}

const std::string& Http10Request::header(const std::string& key) {
  return _headers.at(key);
}

HeaderMap& Http10Request::headers() { return _headers; }

bool Http10Request::isFullRequest(std::vector<char> data) {
  char needle_arr[] = {
      '\r',
      '\n',
      '\r',
      '\n',
  };
  std::vector<char>::iterator found =
      std::search(data.begin(), data.end(), needle_arr,
                  needle_arr + (sizeof(needle_arr) / sizeof(needle_arr[0])));

  return found != data.end();
}

}  // namespace mon_http