#ifndef WEBSERV_INCLUDE_FUNCS_HPP
#define WEBSERV_INCLUDE_FUNCS_HPP

#include "AHttpRequest.hpp"
#include "AHttpResponse.hpp"
#include "Form.hpp"
#include "Handler.hpp"

namespace webserv {

// HandlerResponse (*func)(mon_http::AHttpRequest &, mon_http::Form &)

inline mon_router::HandlerResponse echo(mon_http::AHttpRequest& request,
                                        mon_http::Form& form) {
  mon_router::HandlerResponse res;
  (void)form;

  if (request.hasBody()) {
    res.body = request.body();
  }
  res.code = STATUS_OK;
  res.message = "OK";

  return res;
}

}  // namespace webserv

#endif