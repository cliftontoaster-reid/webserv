
#ifndef MON_HTTP_INCLUDE_AHTTPREQUEST_HPP
#define MON_HTTP_INCLUDE_AHTTPREQUEST_HPP

#include <sys/types.h>

#include <exception>
#include <ostream>
#include <string>
#include <vector>

#include "HeaderMap.hpp"
#include "HttpMethod.hpp"
#include "HttpVersion.hpp"

namespace mon_http {

class EndedTooEarly : public std::exception {
 public:
  EndedTooEarly() {}

 private:
  virtual const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
    return "Malformed HTTP request: "
           "Packet ended too early, could not read body.";
  }
};

class AHttpRequest {
 public:
  virtual ~AHttpRequest() {}

  virtual void parse(const std::vector<char>& data) = 0;

  virtual HttpVersion version() const;
  virtual HttpMethod method() const = 0;
  virtual const std::string& path() const = 0;
  virtual bool hasBody() const = 0;
  virtual const std::string& body() const = 0;
  virtual const std::string& mediaType() = 0;
  virtual bool hasHeader(const std::string& key) = 0;
  virtual const std::string& header(const std::string& key) = 0;
  virtual HeaderMap& headers() = 0;

  static bool isFullRequest(std::vector<char> data);

 private:
};

std::ostream& operator<<(std::ostream& oStr, AHttpRequest& use);

}  // namespace mon_http

#endif
