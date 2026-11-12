#pragma once
#include "./MatchingEngine/MatchingEngine.hpp"
#include "./MatchingEngine/OrderBook/OrderBook.hpp"
#include "./Network/TCPOrderSession.hpp"
#include "./Network/TCP_Server.hpp"
#include "./Network/TraderClient.hpp"
#include "./Network/UDPMarketDataReceiver.hpp"
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
