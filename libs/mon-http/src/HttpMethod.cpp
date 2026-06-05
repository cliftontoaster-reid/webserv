#include "HttpMethod.hpp"

namespace mon_http {

HttpMethod::HttpMethod() : val_(HttpMethodUnknown) {}

HttpMethod::HttpMethod(Type val) : val_(val) {}

HttpMethod::HttpMethod(const HttpMethod& other) : val_(other.val_) {}

HttpMethod& HttpMethod::operator=(const HttpMethod& other) {
  if (this != &other) {
    val_ = other.val_;
  }
  return *this;
}

HttpMethod::Type HttpMethod::getType() const { return val_; }

HttpMethod HttpMethod::fromString(const std::string& data) {
  if (data == "GET") {
    return HttpMethod(HttpMethodGet);
  }
  if (data == "HEAD") {
    return HttpMethod(HttpMethodHead);
  }
  if (data == "POST") {
    return HttpMethod(HttpMethodPost);
  }
  if (data == "PUT") {
    return HttpMethod(HttpMethodPut);
  }
  if (data == "DELETE") {
    return HttpMethod(HttpMethodDelete);
  }
  if (data == "CONNECT") {
    return HttpMethod(HttpMethodConnect);
  }
  if (data == "OPTIONS") {
    return HttpMethod(HttpMethodOptions);
  }
  if (data == "TRACE") {
    return HttpMethod(HttpMethodTrace);
  }
  if (data == "PATCH") {
    return HttpMethod(HttpMethodPatch);
  }
  return HttpMethod(HttpMethodUnknown);
}

std::string HttpMethod::toString() const {
  switch (val_) {
    case HttpMethodGet:
      return "GET";
    case HttpMethodHead:
      return "HEAD";
    case HttpMethodPost:
      return "POST";
    case HttpMethodPut:
      return "PUT";
    case HttpMethodDelete:
      return "DELETE";
    case HttpMethodConnect:
      return "CONNECT";
    case HttpMethodOptions:
      return "OPTIONS";
    case HttpMethodTrace:
      return "TRACE";
    case HttpMethodPatch:
      return "PATCH";
    case HttpMethodUnknown:
    default:
      return "UNKNOWN";
  }
}

}  // namespace mon_http
