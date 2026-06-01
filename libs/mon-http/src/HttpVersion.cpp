#include "HttpVersion.hpp"

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <string>

namespace mon_http {

HttpVersion::HttpVersion() : value(HttpVersionUnknown) {}

HttpVersion::HttpVersion(Type val) : value(val) {}

HttpVersion::HttpVersion(const HttpVersion& other) : value(other.value) {}

HttpVersion& HttpVersion::operator=(const HttpVersion& other) {
  if (this != &other) {
    value = other.value;
  }
  return *this;
}

HttpVersion HttpVersion::sniffHttpVersion(const std::vector<char>& data) {
  static const std::string HTTP_09 = "GET ";
  static const std::string HTTP_20 = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";
  static const std::string HTTP_11 = "HTTP/1.1";
  static const std::string HTTP_10 = "HTTP/1.0";

  if (data.size() >= HTTP_20.size() &&
      std::memcmp(&data[0], HTTP_20.data(), HTTP_20.size()) == 0) {
    return HttpVersion(HttpVersion2_0);
  }

  const std::string TARGET_RN = "\r\n";

  std::vector<char>::const_iterator iter =
      std::search(data.begin(), data.end(), TARGET_RN.begin(), TARGET_RN.end());

  if (iter == data.end()) {
    return HttpVersion(HttpVersionUnknown);
  }

  if (std::distance(data.begin(), iter) >=
      static_cast<std::ptrdiff_t>(HTTP_11.length())) {
    std::vector<char>::const_iterator version_start =
        iter - static_cast<std::ptrdiff_t>(HTTP_11.length());

    if (std::equal(HTTP_11.begin(), HTTP_11.end(), version_start)) {
      return HttpVersion(HttpVersion1_1);
    }

    if (std::equal(HTTP_10.begin(), HTTP_10.end(), version_start)) {
      return HttpVersion(HttpVersion1_0);
    }
  }

  if (data.size() >= HTTP_09.length() &&
      std::equal(HTTP_09.begin(), HTTP_09.end(), data.begin()) &&
      std::distance(data.begin(), iter) >
          static_cast<std::ptrdiff_t>(HTTP_09.length())) {
    return HttpVersion(HttpVersion0_9);
  }

  return HttpVersion(HttpVersionUnknown);
}

const char* HttpVersion::toString() {
  switch (value) {
    case HttpVersionUnknown:
      return "unknown HTTP version";
    case HttpVersion0_9:
      return "HTTP/0.9";
    case HttpVersion1_0:
      return "HTTP/1.0";
    case HttpVersion1_1:
      return "HTTP/1.1";
    case HttpVersion2_0:
      return "HTTP/2.0";
  }
}

}  // namespace mon_http
