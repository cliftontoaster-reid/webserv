#include <csignal>
#include <iostream>

#include "AHttpResponse.hpp"
#include "Server.hpp"

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

int main() {
  std::signal(SIGPIPE, SIG_IGN);       // NOLINT
  std::signal(SIGINT, handle_sigint);  // NOLINT

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
  serv.router().addRoute("/", "/var/www/html", 1998);
  serv.router().addRoute("/imgs", "/usr/share/pixmaps/", 1998);
  serv.router().addHandler("/api/hello", hello);
  serv.router().addRoute("/die", "/home/creid/Downloads/", 1998);
  serv.router().addCgi("/*.php", "/usr/bin/php-cgi");
  serv.router().ready();

  std::cout << "=================================" << '\n';
  std::cout << " Listning at http://0.0.0.0:1998" << '\n';
  std::cout << " Go here : http://localhost:1998" << '\n';
  std::cout << "=================================" << '\n';
  std::cout.flush();
  serv.run();
}
