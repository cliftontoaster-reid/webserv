#include <iostream>

#include "Server.hpp"

int main() {
  mon_net::Server serv;

  std::cout << "▖  ▖  ▌ ▄▖▜     ▐▘  ▄▖  ▗   ▄▖" << '\n';
  std::cout << "▌▞▖▌█▌▛▌▙▖▐ ▛▌▛▌▜▘  ▛▌  ▜   ▛▌" << '\n';
  std::cout << "▛ ▝▌▙▖▙▌▌ ▐▖▙▌▙▌▐   █▌▗ ▟▖▗ █▌" << '\n';

  serv.registerPort(1998);

  std::cout << "=================================" << '\n';
  std::cout << " Listning at http://0.0.0.0:1998" << '\n';
  std::cout << " Go here : http://localhost:1998" << '\n';
  std::cout << "=================================" << '\n';
  serv.run();
}
