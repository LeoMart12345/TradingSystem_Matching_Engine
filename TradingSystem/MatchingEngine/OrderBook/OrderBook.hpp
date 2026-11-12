#pragma once

#include "./ObjectPool.hpp"
#include "Price.hpp"
#include "Order.hpp"
#include <array>
#include <vector>

class OrderBook {

private:
  ObjectPool<Order, 10000>
      mOrderPool; // pool of objects that will be acquired and released to as
                  // opposed to a syscall.

  // FIFO queue of resting orders for one price level. The orders are linked
  // through their own mPrev/mNext pointers, so appending and cancelling is
  // O(1) and never touches the allocator.
  struct PriceLevel {
    Order *head = nullptr;
    Order *tail = nullptr;
  };

  // each of these holds 64 bits each bit represents a level, if bit = 1 the
  // level is active, if bit is 0 level is inactive
  std::vector<u_int64_t> mBidBitmap;
  std::vector<u_int64_t> mAskBitmap;

  // best active level of each side, -1 when the side is empty. Kept up to date
  // on insert/remove so the hot path never has to scan the bitmaps.
  int mBestBidLevel = -1;
  int mBestAskLevel = -1;

  // these levels queue the orders of both sides in price-time priority
  std::vector<PriceLevel> mBidpriceLevel;
  std::vector<PriceLevel> mAskPriceLevel;

  // Hashmap for keeping track of orderId (useful for cancelling orders by
  // orderId). The order itself holds the price and the side, so a pointer is
  // all we need and the table stays cache friendly.
  std::array<Order *, 10000> orderIdtoOrderMapping{};

  // refresh a cached best level from the bitmap (used when the best level
  // empties out)
  int rescanBestBidLevel();
  int rescanBestAskLevel();

public:
  explicit OrderBook(size_t size);

  // Functions to benchmark the orderBook

  Order generateRandomOrder();

  // Bid
  void addBid(Order *order);
  void removeBid(u_int64_t orderID);
  void setBidBitTo1(
      const Price &price); // takes in the price, uses helpers to set bitmap bit
                           // to 1 to indicate active price level.
  void setBidBitTo0(const Price &price);

  // updated API that takes into account quantity and the posibility of a
  // partial fill
  Order *getBestBid();

  Order *getBestAsk();

  // non-throwing variants, return nullptr when the side is empty
  Order *tryGetBestBid();

  Order *tryGetBestAsk();

  ObjectPool<Order, 10000> &getOrderPool() { return mOrderPool; }

  // void fillBestAsk(u_int16_t quantity);
  // Ask
  void addAsk(Order *order);
  void removeAsk(u_int64_t orderId);
  void setAskBitTo1(const Price &price);
  void setAskBitTo0(const Price &price);

  // unlinks the order from its price level and returns it to the pool
  void removeOrder(Order *order);

  std::pair<u_int64_t, u_int64_t> getVolumeAtLevelFromPrice(const Price &price);
  void getTotalVolume();

  // helper functions
  int priceToIndex(Price price) const;
  Price indexToPrice(int levelIndex) const;
  std::pair<Price, Side> orderIdToPrice(const u_int64_t orderId) const;
  void removeOrderFromOrderId(u_int64_t orderId);
  std::pair<size_t, size_t> priceToBitmapIndex(Price price);
  std::pair<size_t, size_t> indexToBitmapIndex(int priceIndex);

  int findBestBidLevel() const; // returns -1 if there are no bids
  int findBestAskLevel() const; // same for the asks

  void printOrderBook() const;
};
