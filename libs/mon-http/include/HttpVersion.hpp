
#ifndef MON_HTTP_INCLUDE_HTTPVERSION_HTTP
#define MON_HTTP_INCLUDE_HTTPVERSION_HTTP

#include <stdint.h>

#include <string>
#include <vector>

namespace mon_http {

class HttpVersion {
 public:
  enum Type {
    HttpVersionUnknown,

    HttpVersion0_9,
    HttpVersion1_0,
    HttpVersion1_1,
    HttpVersion2_0,
  };

  HttpVersion();
  explicit HttpVersion(Type val);
  HttpVersion(const HttpVersion& other);
  HttpVersion& operator=(const HttpVersion& other);

  static HttpVersion sniffHttpVersion(const std::vector<char>& data);
  static HttpVersion sniffHttpVersion(const std::string& data);
  static HttpVersion sniffHttpVersion(const char* data, size_t size);
  const char* toString();

  Type value;
};

}  // namespace mon_http

#endif
