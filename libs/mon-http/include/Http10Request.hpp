#ifndef MON_HTTP_INCLUDE_HTTP10REQUEST_HPP
#define MON_HTTP_INCLUDE_HTTP10REQUEST_HPP

#include <string>

#include "AHttpRequest.hpp"
#include "HeaderMap.hpp"
#include "HttpMethod.hpp"
#include "HttpVersion.hpp"

namespace mon_http {

class Http10Request : public AHttpRequest {
 public:
  Http10Request();
  Http10Request(const Http10Request& other);
  Http10Request& operator=(const Http10Request& other);
  ~Http10Request();

  void parse(const std::vector<char>& data);

  HttpVersion version() const;
  HttpMethod method() const;
  const std::string& path() const;
  bool hasBody() const;
  const std::string& body() const;
  const std::string& mediaType();
  bool hasHeader(const std::string& key);
  const std::string& header(const std::string& key);
  HeaderMap& headers();

  static bool isFullRequest(std::vector<char> data);

 private:
  HttpMethod _method;
  std::string _path;
  std::string _body;
  HeaderMap _headers;
};

}  // namespace mon_http

#endif