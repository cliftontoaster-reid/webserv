#include <csignal>
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
  (void)sig;
  stopNow = 1;
  std::signal(SIGINT, SIG_DFL);  // NOLINT
}

int main(int argc, const char* argv[]) {
  std::signal(SIGPIPE, SIG_IGN);        // NOLINT
  std::signal(SIGTERM, handle_sigint);  // NOLINT
  std::signal(SIGINT, handle_sigint);   // NOLINT

  mon_net::Server serv;

  std::cout << "   ▖  ▖  ▌ ▐▘▜     ▐▘  ▄▖▄▖▄▖" << '\n';
  std::cout << "   ▌▞▖▌█▌▛▌▜▘▐ ▛▌▛▌▜▘  ▌▌▚ ▚ " << '\n';
  std::cout << "   ▛ ▝▌▙▖▙▌▐ ▐▖▙▌▙▌▐   ▙▌▄▌▄▌" << '\n';

  std::cout << '\n';
  std::cout << "Licensed under the MIT License" << '\n';
  std::cout << "Copyright (c) 2026 Clifton Toaster Reid" << '\n';
  std::cout << "Version: " << WEBSERV_VERSION << '\n';
  std::cout << '\n';

  if (argc != 2) {
    std::cout << "Invalid usage of webfloof." << '\n';
    std::cout << "Usage : " << argv[0] << " [config.toml]" << '\n';

    std::cout.flush();
    return 1;
  }

  toml98::Value preconf = toml98::readTomlFile(argv[1]);
  webserv::Config config = webserv::Config(preconf);

  config.implement(serv.router(), serv.listener());

  std::cout << "=================================" << '\n';
  std::cout << " Listning at http://0.0.0.0:1998" << '\n';
  std::cout << " Go here : http://localhost:1998" << '\n';
  std::cout << "=================================" << '\n';
  std::cout.flush();
  serv.run();
}
