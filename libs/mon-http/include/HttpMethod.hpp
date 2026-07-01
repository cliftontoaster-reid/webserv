
#ifndef MON_HTTP_INCLUDE_HTTPMETHOD_HPP
#define MON_HTTP_INCLUDE_HTTPMETHOD_HPP

#include <string>

namespace mon_http {

class HttpMethod {
 public:
  enum Type {
    HttpMethodUnknown,

    HttpMethodGet,
    HttpMethodHead,
    HttpMethodPost,
    HttpMethodPut,
    HttpMethodDelete,
    HttpMethodConnect,
    HttpMethodOptions,
    HttpMethodTrace,
    HttpMethodPatch,
  };

  HttpMethod();
  explicit HttpMethod(Type val);
  explicit HttpMethod(const std::string& val);
  HttpMethod(const HttpMethod& other);
  HttpMethod& operator=(const HttpMethod& other);

  Type getType() const;

  static HttpMethod fromString(const std::string& data);
  std::string toString() const;

  friend bool operator==(const HttpMethod& lhs, const HttpMethod& rhs);
  friend bool operator!=(const HttpMethod& lhs, const HttpMethod& rhs);
  friend bool operator==(const HttpMethod& lhs, Type rhs);
  friend bool operator!=(const HttpMethod& lhs, Type rhs);
  friend bool operator==(Type lhs, const HttpMethod& rhs);
  friend bool operator!=(Type lhs, const HttpMethod& rhs);

 private:
  Type val_;
};

}  // namespace mon_http

#endif