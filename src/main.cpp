#include <csignal>
#include <iostream>

#include "Server.hpp"

int main() {
  std::signal(SIGPIPE, SIG_IGN);  // NOLINT

  mon_net::Server serv;

  std::cout << "   ▖  ▖  ▌ ▐▘▜     ▐▘  ▄▖▄▖▄▖" << '\n';
  std::cout << "   ▌▞▖▌█▌▛▌▜▘▐ ▛▌▛▌▜▘  ▌▌▚ ▚ " << '\n';
  std::cout << "   ▛ ▝▌▙▖▙▌▐ ▐▖▙▌▙▌▐   ▙▌▄▌▄▌" << '\n';

  std::cout << '\n';
  std::cout << "Licensed under the MIT License" << '\n';
  std::cout << "Copyright (c) 2026 Clifton Toaster Reid" << '\n';
  std::cout << "Version: " << WEBSERV_VERSION << '\n';
  std::cout << '\n';

  serv.registerPort(1998);
  serv.router().addRoute("/", "assets/html", 1998);
  serv.router().addRoute("/imgs", "/usr/share/pixmaps/", 1998);
  serv.router().ready();

  std::cout << "=================================" << '\n';
  std::cout << " Listning at http://0.0.0.0:1998" << '\n';
  std::cout << " Go here : http://localhost:1998" << '\n';
  std::cout << "=================================" << '\n';
  serv.run();
}
