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
  return sniffHttpVersion(data.empty() ? NULL : &data[0], data.size());
}
HttpVersion HttpVersion::sniffHttpVersion(const std::string& data) {
  return sniffHttpVersion(data.data(), data.length());
}
HttpVersion HttpVersion::sniffHttpVersion(const char* data, size_t size) {
  // Define string constants and their lengths explicitly for C++98
  static const char HTTP_09[] = "GET /";
  static const size_t HTTP_09_LEN = sizeof(HTTP_09) - 1;

  static const char HTTP_20[] = "PRI * HTTP/2.0\r\n";
  static const size_t HTTP_20_LEN = sizeof(HTTP_20) - 1;

  static const char HTTP_11[] = "HTTP/1.1";
  static const size_t HTTP_11_LEN = sizeof(HTTP_11) - 1;

  static const char HTTP_10[] = "HTTP/1.0";
  static const size_t HTTP_10_LEN = sizeof(HTTP_10) - 1;

  static const char TARGET_RN[] = "\r\n";
  static const size_t TARGET_RN_LEN = sizeof(TARGET_RN) - 1;

  // 1. Check for HTTP/2.0
  if (size >= HTTP_20_LEN && std::memcmp(data, HTTP_20, HTTP_20_LEN) == 0) {
    return HttpVersion(HttpVersion2_0);
  }

  // 2. Find the first occurrence of "\r\n"
  const char* data_end = data + size;
  const char* iter =
      std::search(data, data_end, TARGET_RN, TARGET_RN + TARGET_RN_LEN);

  if (iter == data_end) {
    return HttpVersion(HttpVersionUnknown);
  }

  // Calculate the offset of "\r\n" from the start
  size_t rn_offset = static_cast<size_t>(iter - data);

  // 3. Check for HTTP/1.1 or HTTP/1.0 right before the "\r\n"
  if (rn_offset >= HTTP_11_LEN) {
    const char* version_start = iter - HTTP_11_LEN;

    if (std::memcmp(version_start, HTTP_11, HTTP_11_LEN) == 0) {
      return HttpVersion(HttpVersion1_1);
    }

    if (std::memcmp(version_start, HTTP_10, HTTP_10_LEN) == 0) {
      return HttpVersion(HttpVersion1_0);
    }
  }

  // 4. Check for HTTP/0.9
  if (size >= HTTP_09_LEN && std::memcmp(data, HTTP_09, HTTP_09_LEN) == 0 &&
      rn_offset > HTTP_09_LEN) {
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
    default:
      return "unknown HTTP version";
  }
}

}  // namespace mon_http
