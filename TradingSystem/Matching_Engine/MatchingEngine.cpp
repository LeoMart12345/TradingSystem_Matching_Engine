// #include "MatchingEngine.hpp"

#include "./Matching_Engine/MatchingEngine.hpp"

#include "../Debug/Debug.hpp"
#include "../UtilityClass/OrderIdGenerator.hpp"
#include "Trade.hpp"
#include <algorithm>
#include <mutex>
#include <optional>
#include <sstream>
#include <thread>

// Constructor
MatchingEngine::MatchingEngine(OrderBook &orderBook) : orderBook(orderBook) {
  DEBUG_PRINT("matchingEngine was constructed!");
}

u_int64_t MatchingEngine::addOrder(const Order &orderData) {
  std::lock_guard<std::mutex> lock(mtx);
  Order *order = orderBook.getOrderPool().acquire();
  *order = orderData;
  order->mOrderID = OrderIdGenerator::incrementOrder();

  DEBUG_PRINT("Server Assigned Order ID " << order->mOrderID);
  // need backslash for multi-line debug
  DEBUG_PRINT("SERVER: Order details - "
              << (order->BidOrAsk == Side::Bid ? "BUY" : "SELL") << " "
              << order->mVolume << " " << order->mName << " @ "
              << order->mPrice.getPriceInTicks());

  if (order->BidOrAsk == Side::Bid) {
    orderBook.addBid(order);
  } else {
    orderBook.addAsk(order);
  }

  return order->mOrderID;
}

std::optional<Trade> MatchingEngine::matchLimitOrders() {
  std::lock_guard<std::mutex> lock(mtx);

  Order *bestAsk = orderBook.tryGetBestAsk();
  if (bestAsk == nullptr) {
    return std::nullopt;
  }

  Order *bestBid = orderBook.tryGetBestBid();
  if (bestBid == nullptr) {
    return std::nullopt;
  }

  u_int64_t bidPrice = bestBid->getPrice().getPriceInTicks();
  u_int64_t askPrice = bestAsk->getPrice().getPriceInTicks();

  if (bidPrice < askPrice) {
    return std::nullopt;
  }

  u_int64_t tradeVolume = std::min(bestAsk->GetVolume(), bestBid->GetVolume());
  u_int64_t tradePrice = askPrice;

  bestAsk->reduceVolume(tradeVolume);
  bestBid->reduceVolume(tradeVolume);

  Trade trade(bestBid, bestAsk, tradeVolume, tradePrice);

  if (bestAsk->GetVolume() == 0) {
    orderBook.removeOrder(bestAsk);
  }
  if (bestBid->GetVolume() == 0) {
    orderBook.removeOrder(bestBid);
  }

  return trade;
}
