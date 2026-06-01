
#ifndef MON_HTTP_INCLUDE_HTTP10REQUEST_HTTP
#define MON_HTTP_INCLUDE_HTTP10REQUEST_HTTP

#include <string>

#include "HttpMethod.hpp"
#include "HttpVersion.hpp"

namespace mon_http {

class Http10Request {
 public:
  Http10Request();
  Http10Request(const Http10Request& other);
  Http10Request& operator=(const Http10Request& other);
  virtual ~Http10Request();

  static HttpVersion version() {
    return HttpVersion(HttpVersion::HttpVersion1_0);
  }
  HttpMethod method();
  const std::string& path();
  bool hasBody();
  const std::string& body();
  const std::string& mediaType();
  bool hasHeader(const std::string& key);
  const std::string& header(const std::string& key);

 private:
  HttpMethod _method;
  std::string _path;
  std::string _body;
};

}  // namespace mon_http

#endif