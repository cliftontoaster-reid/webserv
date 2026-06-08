#ifndef MON_HTTP_INCLUDE_HTTP10RESPONSE_HTTP
#define MON_HTTP_INCLUDE_HTTP10RESPONSE_HTTP

#include <sys/types.h>

#include <stdexcept>
#include <string>

#include "HeaderMap.hpp"
#include "HttpVersion.hpp"

namespace mon_http {

class Http10Response {
 public:
  Http10Response();
  Http10Response(const Http10Response& other);
  Http10Response& operator=(const Http10Response& other);
  virtual ~Http10Response();

  static Http10Response createRedirect(int code, const std::string& location);

  std::vector<char> encode();

  // NOLINTNEXTLINE
  HttpVersion version() const;
  int statusCode() const;
  void setStatusCode(int code);
  bool hasBody() const;
  bool hasHeader(const std::string& key);
  const std::string& header(const std::string& key);

  std::string statusMessage;
  std::string body;
  HeaderMap headers;

 private:
  int _code;
};

}  // namespace mon_http

#endif
