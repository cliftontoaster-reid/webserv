#include <sys/types.h>

#include <csignal>
#include <exception>
#include <iostream>

#include "AHttpResponse.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "Value.hpp"
#include "toml98.hpp"

mon_router::HandlerResponse hello(mon_http::AHttpRequest& request,
                                  mon_http::Form& form_data) {
  (void)request;
  (void)form_data;
  mon_router::HandlerResponse res = {STATUS_OK, "OK", "Hello from /api/hello!"};
  return res;
}

void handle_sigint(int sig) {
  stopNow = 1;
  std::signal(sig, SIG_DFL);  // NOLINT
}

void printPorts(const webserv::Config& cfg) {
  std::vector<webserv::Server>::const_iterator iterServ;
  for (iterServ = cfg.server.begin(); iterServ != cfg.server.end();
       iterServ++) {
    const webserv::Server& serv = *iterServ;
    std::vector<u_int16_t>::const_iterator iterPorts;
    for (iterPorts = serv.ports.begin(); iterPorts != serv.ports.end();
         iterPorts++) {
      std::cerr << " - http://0.0.0.0:" << *iterPorts << '\n';
    }
  }
}

int main(int argc, const char* argv[]) {
  std::signal(SIGPIPE, SIG_IGN);        // NOLINT
  std::signal(SIGTERM, handle_sigint);  // NOLINT
  std::signal(SIGINT, handle_sigint);   // NOLINT

  mon_net::Server serv;

  std::cerr << "   ▖  ▖  ▌ ▐▘▜     ▐▘  ▄▖▄▖▄▖" << '\n';
  std::cerr << "   ▌▞▖▌█▌▛▌▜▘▐ ▛▌▛▌▜▘  ▌▌▚ ▚ " << '\n';
  std::cerr << "   ▛ ▝▌▙▖▙▌▐ ▐▖▙▌▙▌▐   ▙▌▄▌▄▌" << '\n';

  std::cerr << '\n';
  std::cerr << "Licensed under the MIT License" << '\n';
  std::cerr << "Copyright (c) 2026 Clifton Toaster Reid" << '\n';
  std::cerr << "Version: " << WEBSERV_VERSION << '\n';
  std::cerr << '\n';

  if (argc != 2) {
    std::cerr << "Invalid usage of webfloof." << '\n';
    std::cerr << "Usage : " << argv[0] << " [config.toml]" << '\n';

    std::cerr.flush();
    return 1;
  }

  toml98::Value preconf = toml98::readTomlFile(argv[1]);
  webserv::Config config = webserv::Config(preconf);

  try {
    config.implement(serv.router(), serv.listener());
  } catch (const std::exception& err) {
    std::cerr << "ERROR: Could not start the server." << '\n';
    std::cerr << "==================================" << '\n' << '\n';
    std::cerr << err.what() << '\n';
    return 1;
  }

  std::cerr << "=================================" << '\n';
  std::cerr << " Listning at on :" << '\n';
  printPorts(config);
  std::cerr << "=================================" << '\n';
  std::cerr.flush();
  serv.run();
}
