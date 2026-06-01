
#ifndef MON_HTTP_INCLUDE_HTTPMETHOD_HTTP
#define MON_HTTP_INCLUDE_HTTPMETHOD_HTTP

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
  HttpMethod(const HttpMethod& other);
  HttpMethod& operator=(const HttpMethod& other);

  Type getType() const;

  static HttpMethod fromString(const std::string& data);
  std::string toString() const;

 private:
  Type val_;
};

}  // namespace mon_http

#endif