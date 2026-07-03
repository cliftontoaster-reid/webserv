#ifndef MON_ROUTER_INCLUDE_HANDLER_HPP
#define MON_ROUTER_INCLUDE_HANDLER_HPP

#include <sys/types.h>

#include <map>
#include <string>

#include "HeaderMap.hpp"
#include "Value.hpp"

namespace mon_http {

class AHttpRequest;
class Form;

}  // namespace mon_http

namespace mon_router {

struct HandlerResponse {
  HandlerResponse() {}
  HandlerResponse(int code, const std::string& message, const std::string& body)
      : code(code), message(message), body(body) {}

  int code;
  std::string message;
  mon_http::HeaderMap headers;
  std::string body;
};

struct Handler {
  std::string path;
  HandlerResponse (*func)(
      mon_http::AHttpRequest& request, mon_http::Form& form_data,
      const std::map<std::string, toml98::Value>& arguments);
  u_int16_t port;
  std::map<std::string, toml98::Value> arguments;
};

}  // namespace mon_router

#endif
