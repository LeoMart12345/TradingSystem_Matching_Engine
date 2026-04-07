#include "./Network Servers/TraderClient.hpp"

int main() {
  std::string host = "127.0.0.1";
  TraderClient trader(host, 5555);
  trader.runTerminal();
}
