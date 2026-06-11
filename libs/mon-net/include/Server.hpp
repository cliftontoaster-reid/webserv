#ifndef MON_NET_INCLUDE_SERVER_HTTP
#define MON_NET_INCLUDE_SERVER_HTTP

#include <sys/types.h>

#include "Listener.hpp"
namespace mon_net {

class Server {
 public:
  Server();
  Server(const Server& other);
  Server& operator=(const Server& other);

  void registerPort(u_int16_t port);
  void run();

 private:
  Listener<1024> _listener;
  // Router _router;

  bool readVersion(Context& ctx, const Event& event);
  void readCtx(Context& ctx);
};

}  // namespace mon_net

#endif