#ifndef MON_ROUTER_INCLUDE_HANDLER_HPP
#define MON_ROUTER_INCLUDE_HANDLER_HPP

#include <sys/types.h>

#include <string>

namespace mon_http {

class AHttpRequest;
class Form;

}  // namespace mon_http

namespace mon_router {

struct HandlerResponse {
  int code;
  std::string message;
  std::string body;
};

struct Handler {
  std::string path;
  HandlerResponse (*func)(mon_http::AHttpRequest& request,
                          mon_http::Form& form_data);
  u_int16_t port;
};

}  // namespace mon_router

#endif
