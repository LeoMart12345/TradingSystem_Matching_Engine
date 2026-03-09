#pragma once
#include "./Matching_Engine/MatchingEngine.hpp"
#include "./Matching_Engine/OrderBook/OrderBook.hpp"
#include "./Network Servers/TCPOrderSession.hpp"
#include "./Network Servers/TCP_Server.hpp"
#include "./Network Servers/TraderClient.hpp"
#include "./Network Servers/UDPMarketDataReceiver.hpp"
#include <iostream>
#include <vector>

class TradingSystem {
private:
  std::unique_ptr<OrderBook> orderBook;
  MatchingEngine matchingEngine;
  std::unique_ptr<TCPServer> TCPServerI;
  std::unique_ptr<UDPServer> UDPServerI;

public:
  TradingSystem(int tcp_port, int udp_port);

  ~TradingSystem();

  void start();
  void stop();
  void process_order(const std::string &order_str);

  void addOrder(const Order &order) { matchingEngine.addOrder(order); }

  OrderBook &getOrderBook() { return *orderBook; }
  MatchingEngine &getMatchingEngine() { return matchingEngine; }
};
