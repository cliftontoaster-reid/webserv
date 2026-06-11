#include "Server.hpp"

int main() {
  mon_net::Server serv;

  serv.registerPort(1998);
  serv.run();
}