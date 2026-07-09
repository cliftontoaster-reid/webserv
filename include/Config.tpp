#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "Config.hpp"
#include "Listener.hpp"
#include "Router.hpp"

namespace webserv {

template <int MaxEvents>
size_t Server::implement(mon_router::Router& router,
                         std::vector<u_int16_t>& knownPorts,
                         mon_net::Listener<MaxEvents>& listener) const {
  size_t nbrRules = 0;

  if (std::find(knownPorts.begin(), knownPorts.end(), port) !=
      knownPorts.end()) {
    throw std::invalid_argument("Already used port");
  }
  listener.registerPort(port);
  knownPorts.push_back(port);

  for (std::vector<Host>::const_iterator it = host.begin(); it != host.end(); ++it) {
    nbrRules += it->implement(router, port);
  }

  return nbrRules;
}

template <int MaxEvents>
size_t Config::implement(mon_router::Router& router,
                         mon_net::Listener<MaxEvents>& listener) const {
  size_t nbrRules = 0;
  std::vector<u_int16_t> ports;

  std::vector<Server>::const_iterator iter;
  for (iter = server.begin(); iter != server.end(); iter++) {
    const Server& serv = *iter;

    nbrRules += serv.implement(router, ports, listener);
  }

  router.ready();

  return nbrRules;
}

template size_t Server::implement<MAX_EVENTS>(
    mon_router::Router& router, std::vector<u_int16_t>& ports,
    mon_net::Listener<MAX_EVENTS>& listener) const;

template size_t Config::implement<MAX_EVENTS>(
    mon_router::Router& router, mon_net::Listener<MAX_EVENTS>& listener) const;

}  // namespace webserv
