// #include "MatchingEngine.hpp"

#include "./Matching_Engine/MatchingEngine.hpp"

#include "../Debug/Debug.hpp"
#include "../UtilityClass/OrderIdGenerator.hpp"
#include "Trade.hpp"
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
  try {
    Order *bestAsk = orderBook.getBestAsk();
    Order *bestBid = orderBook.getBestBid();

    u_int64_t bidPrice = bestBid->getPrice().getPriceInTicks();
    u_int64_t askPrice = bestAsk->getPrice().getPriceInTicks();

    // std::cout << "matchLimitOrders: bid=" << bidPrice
    //           << " ask=" << askPrice
    //           << " bidVol=" << bestBid->GetVolume()
    //           << " askVol=" << bestAsk->GetVolume() << std::endl;

    if (bidPrice >= askPrice) {
      u_int64_t tradeVolume =
          std::min(bestAsk->GetVolume(), bestBid->GetVolume());
      u_int64_t tradePrice = askPrice;

      bestAsk->reduceVolume(tradeVolume);
      bestBid->reduceVolume(tradeVolume);

      // std::cout << "matchLimitOrders: after reduce - bidVol=" <<
      // bestBid->GetVolume() << " askVol=" << bestAsk->GetVolume() <<
      // std::endl;

      Trade trade(bestBid, bestAsk, tradeVolume, tradePrice);

      if (bestAsk->GetVolume() == 0) {
        // std::cout << "matchLimitOrders: removing ask " <<
        // bestAsk->getOrderId() << std::endl;
        orderBook.removeAsk(bestAsk->getOrderId());
      }
      if (bestBid->GetVolume() == 0) {
        // std::cout << "matchLimitOrders: removing bid " <<
        // bestBid->getOrderId() << std::endl;
        orderBook.removeBid(bestBid->getOrderId());
      }
      return trade;
    } else {
      // std::cout << "matchLimitOrders: no match, spread=" << (askPrice -
      // bidPrice) << std::endl;
    }
  } catch (const std::runtime_error &e) {
    // commenting out for benchmarks and not clutter the results
    // std::cout << "matchLimitOrders: exception - " << e.what() << std::endl;
    return std::nullopt;
  }

  return std::nullopt;
}
