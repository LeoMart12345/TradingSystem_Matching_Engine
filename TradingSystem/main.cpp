#include "TradingSystem.hpp"
#include <memory>

int main() {
  // Starts the TCP order gateway, the UDP market data publisher and the
  // simulated traders, then matches orders until the process is killed.
  auto system = std::make_unique<TradingSystem>(5555, 12345);
  system->start();
}