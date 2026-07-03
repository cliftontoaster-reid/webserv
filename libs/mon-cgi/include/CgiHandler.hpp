#ifndef MON_CGI_INCLUDE_CGIHANDLER_HPP
#define MON_CGI_INCLUDE_CGIHANDLER_HPP

#include <string>
#include <vector>

#include "Glob.hpp"
#include "Handler.hpp"
#include "Listener.hpp"
#include "Uri.hpp"

namespace mon_cgi {

struct Handle {
  Glob glob;
  std::string cgiBin;
  u_int16_t port;
};

class CgiHandler {
 public:
  CgiHandler();
  CgiHandler(const CgiHandler& other);
  CgiHandler& operator=(const CgiHandler& other);

  const Handle* isCgi(mon_router::Uri uri, u_int16_t port) const;

  template <int MaxEvents>
  void handleCgi(const mon_router::Handler& handler, const std::string& cgiBin,
                 mon_http::AHttpRequest& request, int client_fd,
                 mon_net::Listener<MaxEvents>& listener);

  void addGlober(const Glob& glob, const std::string& cgiBin, u_int16_t port);
  void addGlober(const std::string& glob, const std::string& cgiBin,
                 u_int16_t port);

 private:
  std::vector<Handle> _handles;
};

}  // namespace mon_cgi

#include "CgiHandler.tpp"

#endif
