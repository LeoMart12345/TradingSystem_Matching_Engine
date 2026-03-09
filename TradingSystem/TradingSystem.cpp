#include "./TradingSystem.hpp"
#include "./Matching_Engine/MatchingEngine.hpp"
#include "./Matching_Engine/OrderBook/OrderBook.hpp"
#include "./Network Servers/SimulatedTraders.hpp"
#include <iostream>
#include <vector>

TradingSystem::TradingSystem(int tcp_port, int udp_port)
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
  // TESTING!
  std::string host = "127.0.0.1";

  SimulatedTraders simulatedTraders(host, 5555);
  // Spawns 10 clients and lets them randomly trade on their own threads.
  simulatedTraders.run(10);

  // Match limit orders:
  while (1) {
    matchingEngine.matchLimitOrders();

    // REMOVE IN PROD!
    // std::this_thread::sleep_for(std::chrono::milliseconds(400));
  }
}

void TradingSystem::stop() {
  // TCPServer->stop();
  // stop the udp server also
}
