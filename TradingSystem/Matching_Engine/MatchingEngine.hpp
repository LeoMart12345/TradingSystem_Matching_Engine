// MatchingEngine.hpp
#pragma once
#include "../Network Servers/Order_Request.hpp"
#include "./OrderBook/OrderBook.hpp"
#include "./OrderBook/Trade.hpp"
#include "./OrderBook/order.hpp"

#include <atomic>
#include <deque>
#include <map>
#include <mutex>
#include <queue>
#include <unordered_map>

class MatchingEngine {

private:
  OrderBook &orderBook;
  std::atomic<u_int64_t> nextOrderId{1000};
  std::mutex mtx;

public:
  explicit MatchingEngine(OrderBook &orderBook);

  std::optional<Trade> matchLimitOrders();
  u_int64_t addOrder(const Order &orderData);
  void processOrder(OrderRequest orderRequest);
  OrderBook &getOrderBook() { return orderBook; }
  void cancelOrder(u_int64_t orderId) {
    std::lock_guard<std::mutex> lock(mtx);
    orderBook.removeOrderFromOrderId(orderId);
  }
};
