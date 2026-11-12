#include "./TradingSystem.hpp"
#include "./MatchingEngine/MatchingEngine.hpp"
#include "./MatchingEngine/OrderBook/OrderBook.hpp"
#include "./Network/SimulatedTraders.hpp"

// udp_port is not wired through yet: UDPServer publishes to the hard-coded
// multicast port in its own constructor.
TradingSystem::TradingSystem(int tcp_port, [[maybe_unused]] int udp_port)
    : orderBook(std::make_unique<OrderBook>(101)), matchingEngine(*orderBook),
      TCPServerI(std::make_unique<TCPServer>(tcp_port, matchingEngine)),
      UDPServerI(std::make_unique<UDPServer>(matchingEngine)) {}

TradingSystem::~TradingSystem() {}

void TradingSystem::start() {
  // start making random orders for testing the udp multicast.

  // TCP internally this creates a new thread for each connection
  std::thread tcpThread([this]() { TCPServerI->run(); });
  tcpThread.detach();

  // UDP
  UDPServerI->run();

  // give time for the servers to acquire their resources. for TCP will get a
  // boost error if I try to then connect when its not finished construction
  std::this_thread::sleep_for(std::chrono::milliseconds(400));

  // simulated traders in the background using client interface
  std::string host = "127.0.0.1";

  SimulatedTraders simulatedTraders(host, 5555);
  // Spawns x clients and lets them randomly trade on their own threads.
  simulatedTraders.run(5);

  // Match limit orders:
  while (1) {
    auto trade = matchingEngine.matchLimitOrders();
    if (trade.has_value()) {
      UDPServerI->sendTrade(trade.value());
    }
  }
}

void TradingSystem::stop() {
  // TCPServer->stop();
  // stop the udp server also
}
