
#ifndef MON_HTTP_INCLUDE_AHTTPREQUEST_HTTP
#define MON_HTTP_INCLUDE_AHTTPREQUEST_HTTP

#include <string>

#include "HttpMethod.hpp"
#include "HttpVersion.hpp"

namespace mon_http {

class AHttpRequest {
 public:
  virtual ~AHttpRequest() {}

  static HttpVersion version();
  virtual HttpMethod method() = 0;
  virtual const std::string& path() = 0;
  virtual bool hasBody() = 0;
  virtual const std::string& body() = 0;
  virtual const std::string& mediaType() = 0;
  virtual bool hasHeader(const std::string& key) = 0;
  virtual const std::string& header(const std::string& key) = 0;

 private:
};

}  // namespace mon_http

#endif
