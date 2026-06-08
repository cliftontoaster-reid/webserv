#ifndef MON_HTTP_INCLUDE_HTTP10REQUEST_HTTP
#define MON_HTTP_INCLUDE_HTTP10REQUEST_HTTP

#include <stdexcept>
#include <string>

#include "HeaderMap.hpp"
#include "HttpMethod.hpp"
#include "HttpVersion.hpp"

namespace mon_http {

class Http10Request {
 public:
  Http10Request();
  Http10Request(const Http10Request& other);
  Http10Request& operator=(const Http10Request& other);
  ~Http10Request();

  Http10Request parse(const std::vector<char>& data);

  HttpVersion version() const;
  HttpMethod method() const;
  const std::string& path() const;
  bool hasBody() const;
  const std::string& body() const;
  const std::string& mediaType();
  bool hasHeader(const std::string& key);
  const std::string& header(const std::string& key);
  HeaderMap& headers();

 private:
  HttpMethod _method;
  std::string _path;
  std::string _body;
  HeaderMap _headers;
};

}  // namespace mon_http

#endif