#ifndef MON_NET_INCLUDE_SERVER_HTTP
#define MON_NET_INCLUDE_SERVER_HTTP

#include <sys/types.h>

#include "Listener.hpp"
#include "Router.hpp"
namespace mon_net {

class Server {
 public:
  Server();
  Server(const Server& other);
  Server& operator=(const Server& other);
  ~Server();

  void registerPort(u_int16_t port);
  void run();

  mon_router::Router& router() { return _router; }

 private:
  Listener<1024> _listener;
  mon_router::Router _router;

  bool readVersion(Context& ctx);
  void readCtx(Context& ctx);
};

}  // namespace mon_net

#endif