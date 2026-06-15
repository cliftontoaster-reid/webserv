#include "Router.hpp"

#include <exception>
#include <string>

#include "Http10Response.hpp"
#include "Listener.hpp"
#include "Uri.hpp"

namespace mon_router {

Router::Router() {}
Router::Router(const Router& other) { (void)other; }
Router& Router::operator=(const Router& other) {
  if (this != &other) {
  }
  return *this;
}
Router::~Router() {}

template <int MaxEvents>
void Router::handle(mon_http::AHttpRequest& request, int client_fd,
                    mon_net::Listener<MaxEvents>& listener) {
  try {
    const std::string& pathRaw = request.path();
    Uri path(pathRaw);

  } catch (std::exception& err) {
    mon_http::Http10Response res;
    res.statusMessage = err.what();
    res.error500();

    listener.write(res, client_fd);
  }
}

}  // namespace mon_router