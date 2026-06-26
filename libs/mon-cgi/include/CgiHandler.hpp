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
};

class CgiHandler {
 public:
  CgiHandler();
  CgiHandler(const CgiHandler& other);
  CgiHandler& operator=(const CgiHandler& other);

  bool isCgi(mon_router::Uri uri);

  template <int MaxEvents>
  void handleCgi(const mon_router::Handler& handler,
                 mon_http::AHttpRequest& request, int client_fd,
                 mon_net::Listener<MaxEvents>& listener);

  void addGlober(Glob glob, const std::string& cgiBin);
  void addGlober(const std::string& glob, const std::string& cgiBin);

 private:
  std::vector<Handle> _handles;
};

}  // namespace mon_cgi

#include "CgiHandler.tpp"

#endif
