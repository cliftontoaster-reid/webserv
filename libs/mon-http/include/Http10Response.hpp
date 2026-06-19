#ifndef MON_HTTP_INCLUDE_HTTP10RESPONSE_HPP
#define MON_HTTP_INCLUDE_HTTP10RESPONSE_HPP

#include <sys/types.h>

#include <string>

#include "AHttpResponse.hpp"
#include "HeaderMap.hpp"
#include "HttpVersion.hpp"

namespace mon_http {

class Http10Response : public AHttpResponse {
 public:
  Http10Response();
  Http10Response(const Http10Response& other);
  Http10Response& operator=(const Http10Response& other);
  virtual ~Http10Response();

  static Http10Response createRedirect(int code, const std::string& location);

  std::vector<char> encode();

  HttpVersion version() const;
  int statusCode() const;
  void setStatusCode(int code);
  bool hasBody() const;
  const std::string& body() const;
  void setBody(const std::string& body);
  void appendBody(const std::string& str);
  std::string& body();
  bool hasHeader(const std::string& key);
  const std::string& header(const std::string& key);
  HeaderMap& headers();

  std::string statusMessage;  // NOLINT

  void error500(const std::string& message = "");
  void error404();
  void ok200();

 private:
  std::string _body;
  HeaderMap _headers;
  int _code;
};

}  // namespace mon_http

#endif
