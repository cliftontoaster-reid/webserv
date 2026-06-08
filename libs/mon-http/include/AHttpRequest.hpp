
#ifndef MON_HTTP_INCLUDE_AHTTPREQUEST_HTTP
#define MON_HTTP_INCLUDE_AHTTPREQUEST_HTTP

#include <sys/types.h>

#include <ostream>
#include <string>
#include <vector>

#include "HeaderMap.hpp"
#include "HttpMethod.hpp"
#include "HttpVersion.hpp"

namespace mon_http {

class AHttpRequest {
 public:
  virtual ~AHttpRequest() {}

  virtual AHttpRequest parse(const std::vector<char>& data) = 0;

  virtual HttpVersion version() const;
  virtual HttpMethod method() const = 0;
  virtual const std::string& path() const = 0;
  virtual bool hasBody() const = 0;
  virtual const std::string& body() const = 0;
  virtual const std::string& mediaType() const = 0;
  virtual bool hasHeader(const std::string& key) = 0;
  virtual const std::string& header(const std::string& key) = 0;
  virtual HeaderMap& headers() = 0;

 private:
};

std::ostream& operator<<(std::ostream& oStr, AHttpRequest& use);

}  // namespace mon_http

#endif
