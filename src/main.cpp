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

int main() {
  std::signal(SIGPIPE, SIG_IGN);  // NOLINT

  mon_net::Server serv;

  std::cout << "   ▖  ▖  ▌ ▐▘▜     ▐▘  ▄▖▄▖▄▖" << std::endl;
  std::cout << "   ▌▞▖▌█▌▛▌▜▘▐ ▛▌▛▌▜▘  ▌▌▚ ▚ " << std::endl;
  std::cout << "   ▛ ▝▌▙▖▙▌▐ ▐▖▙▌▙▌▐   ▙▌▄▌▄▌" << std::endl;

  std::cout << std::endl;
  std::cout << "Licensed under the MIT License" << std::endl;
  std::cout << "Copyright (c) 2026 Clifton Toaster Reid" << std::endl;
  std::cout << "Version: " << WEBSERV_VERSION << std::endl;
  std::cout << std::endl;

  serv.registerPort(1998);
  serv.router().addRoute("/", "/var/www/html", 1998);
  serv.router().addRoute("/imgs", "/usr/share/pixmaps/", 1998);
  serv.router().addHandler("/api/hello", hello);
  serv.router().addCgi("/*.php", "/usr/bin/php-cgi");
  serv.router().ready();

  std::cout << "=================================" << std::endl;
  std::cout << " Listning at http://0.0.0.0:1998" << std::endl;
  std::cout << " Go here : http://localhost:1998" << std::endl;
  std::cout << "=================================" << std::endl;
  std::cout.flush();
  serv.run();
}
