#include "CgiHandler.hpp"

namespace mon_cgi {

template <int MaxEvents>
void CgiHandler::handleCgi(const mon_router::Handler& handler,
                           mon_http::AHttpRequest& request, int client_fd,
                           mon_net::Listener<MaxEvents>& listener) {
  (void)handler;
  (void)request;
  (void)client_fd;
  (void)listener;
}

}  // namespace mon_cgi