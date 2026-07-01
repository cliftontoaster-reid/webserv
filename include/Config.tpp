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

  std::vector<u_int16_t>::const_iterator iterPorts;
  for (iterPorts = ports.begin(); iterPorts != ports.end(); iterPorts++) {
    u_int16_t port = *iterPorts;
    if (std::find(knownPorts.begin(), knownPorts.end(), port) !=
        knownPorts.end()) {
      throw std::invalid_argument("Already used port");
    }
    listener.registerPort(port);
    knownPorts.push_back(port);

    std::vector<Route>::const_iterator iterRoute;
    std::vector<Api>::const_iterator iterApi;
    std::vector<Cgi>::const_iterator iterCgi;
    for (iterRoute = route.begin(); iterRoute != route.end(); iterRoute++) {
      const Route& route = *iterRoute;

      nbrRules += route.implement(router, port);
    }
    for (iterApi = api.begin(); iterApi != api.end(); iterApi++) {
      const Api& api = *iterApi;

      nbrRules += api.implement(router, port);
    }
    for (iterCgi = cgi.begin(); iterCgi != cgi.end(); iterCgi++) {
      const Cgi& cgi = *iterCgi;

      nbrRules += cgi.implement(router, port);
    }
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

  return nbrRules;
}

template size_t Server::implement<MAX_EVENTS>(
    mon_router::Router& router, std::vector<u_int16_t>& ports,
    mon_net::Listener<MAX_EVENTS>& listener) const;

template size_t Config::implement<MAX_EVENTS>(
    mon_router::Router& router, mon_net::Listener<MAX_EVENTS>& listener) const;

}  // namespace webserv
