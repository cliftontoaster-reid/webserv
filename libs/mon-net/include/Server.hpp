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
  Listener<MAX_EVENTS> _listener;
  mon_router::Router _router;

  static bool readVersion(Context& ctx);
  static void readCtx(Context& ctx);

  void handleRequest(Event& event);

  void handleV0_9(Event& event, Context& ctx, mon_http::AHttpRequest& req);
  void handleV1_0(Event& event, Context& ctx, mon_http::AHttpRequest& req);
  void handleV1_1(Event& event, Context& ctx, mon_http::AHttpRequest& req);
  void handleV2_0(Event& event, Context& ctx, mon_http::AHttpRequest& req);

  void handleGetV1_0(Context& ctx, mon_http::AHttpRequest& req);
  void handleGetV1_1(Context& ctx, mon_http::AHttpRequest& req);
};

}  // namespace mon_net

#endif