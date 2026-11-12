#include "OrderBook.hpp"
#include "../Debug/Debug.hpp"
#include "../UtilityClass/OrderIdGenerator.hpp"
#include "order.hpp"
#include <array>
#include <bit>
#include <iostream>
#include <random>
#include <vector>

std::mt19937 rng(std::random_device{}());

std::uniform_int_distribution<int> PriceDist(950, 1050);
std::uniform_int_distribution<int> sideDist(0, 1);
std::uniform_int_distribution<int> volumeDist(1, 1000);

static constexpr int CHUNK = 64;
constexpr int StartOfPrice = 950;
constexpr int NumOfLevels = 101;
static constexpr size_t POOL_SIZE = 10000;

// Order Ids that are used for benchmarking

OrderBook::OrderBook(size_t size)
    : mBidpriceLevel(size), mAskPriceLevel(size),
      mBidBitmap((size + CHUNK - 1) / CHUNK, 0ULL),
      mAskBitmap((size + CHUNK - 1) / CHUNK, 0ULL) {
  DEBUG_PRINT("OrderBook created with " << size << " price levels");
  DEBUG_PRINT("BitMap create with " << mBidBitmap.size() << " 64 bit elements");
}

// generates random orders in a set distribution
Order OrderBook::generateRandomOrder() {
  int randPriceint = PriceDist(rng);
  int randSide = sideDist(rng);
  u_int64_t randVolume = volumeDist(rng);
  std::string notsoRandName = "TSLA";
  Side side = (randSide == 0) ? Bid : Ask;
  Price randPrice(randPriceint);

  Order randomOrder(side, randVolume, notsoRandName,
                    OrderIdGenerator::incrementOrder(), randPrice);

  return randomOrder;
}

// need to add tracking for orderid for cancelOrder
void OrderBook::addBid(Order *order) {
  DEBUG_PRINT("OrderBook::addBid - Order ID: " << order->mOrderID);
  int bidLevel = priceToIndex(order->mPrice);
  PriceLevel &level = mBidpriceLevel[bidLevel];

  order->mPrev = level.tail;
  order->mNext = nullptr;
  if (level.tail != nullptr) {
    level.tail->mNext = order;
  } else {
    level.head = order;
  }
  level.tail = order;

  mBidBitmap[bidLevel / CHUNK] |= (1ULL << (bidLevel % CHUNK));
  if (bidLevel > mBestBidLevel) {
    mBestBidLevel = bidLevel;
  }
  // add the orderid to the array
  orderIdtoOrderMapping[order->mOrderID % POOL_SIZE] = order;
  DEBUG_PRINT("ORDERBOOK::ADDBID after printing price");
}

void OrderBook::addAsk(Order *order) {
  DEBUG_PRINT("OrderBook::addAsk - Order ID: " << order->mOrderID);
  int askLevel = priceToIndex(order->mPrice);
  PriceLevel &level = mAskPriceLevel[askLevel];

  order->mPrev = level.tail;
  order->mNext = nullptr;
  if (level.tail != nullptr) {
    level.tail->mNext = order;
  } else {
    level.head = order;
  }
  level.tail = order;

  mAskBitmap[askLevel / CHUNK] |= (1ULL << (askLevel % CHUNK));
  if (mBestAskLevel == -1 || askLevel < mBestAskLevel) {
    mBestAskLevel = askLevel;
  }
  orderIdtoOrderMapping[order->mOrderID % POOL_SIZE] = order;
  DEBUG_PRINT("ORDERBOOK::ADDASK after printing price");
}

// removing a bid from an orderId
void OrderBook::removeBid(u_int64_t orderId) { removeOrderFromOrderId(orderId); }

void OrderBook::removeAsk(u_int64_t orderId) { removeOrderFromOrderId(orderId); }

void OrderBook::removeOrder(Order *order) {
  const int level = priceToIndex(order->mPrice);
  const bool isBid = order->BidOrAsk == Bid;
  PriceLevel &priceLevel =
      isBid ? mBidpriceLevel[level] : mAskPriceLevel[level];

  if (order->mPrev != nullptr) {
    order->mPrev->mNext = order->mNext;
  } else {
    priceLevel.head = order->mNext;
  }
  if (order->mNext != nullptr) {
    order->mNext->mPrev = order->mPrev;
  } else {
    priceLevel.tail = order->mPrev;
  }
  order->mPrev = nullptr;
  order->mNext = nullptr;

  orderIdtoOrderMapping[order->mOrderID % POOL_SIZE] = nullptr;
  mOrderPool.release(order);

  if (priceLevel.head != nullptr) {
    return; // the level still holds orders, so the best level cannot change
  }

  if (isBid) {
    mBidBitmap[level / CHUNK] &= ~(1ULL << (level % CHUNK));
    if (mBestBidLevel == level) {
      rescanBestBidLevel();
    }
  } else {
    mAskBitmap[level / CHUNK] &= ~(1ULL << (level % CHUNK));
    if (mBestAskLevel == level) {
      rescanBestAskLevel();
    }
  }
}

std::pair<u_int64_t, u_int64_t>
OrderBook::getVolumeAtLevelFromPrice(const Price &price) {

  int level = priceToIndex(price);

  uint64_t totalBidVolume = 0;
  uint64_t totalAskVolume = 0;

  for (Order *order = mBidpriceLevel[level].head; order != nullptr;
       order = order->mNext) {
    totalBidVolume += order->mVolume;
  }

  for (Order *order = mAskPriceLevel[level].head; order != nullptr;
       order = order->mNext) {
    totalAskVolume += order->mVolume;
  }

  return {totalBidVolume, totalAskVolume};
}

void OrderBook::getTotalVolume() {}

// Helper functions
int OrderBook::priceToIndex(Price price) const {
  int index = price.mPriceValueInCent - StartOfPrice;

  if (index < 0 || index >= NumOfLevels) {
    DEBUG_PRINT(
        "ERROR that price is not a price that this asset can be traded at");
    throw std::invalid_argument("Invalid price to bid/ask");
  }
  return index;
}

Price OrderBook::indexToPrice(int levelIndex) const {
  if (levelIndex < 0 || levelIndex >= static_cast<int>(mBidpriceLevel.size())) {
    throw std::invalid_argument("Invalid level index: " +
                                std::to_string(levelIndex));
  }

  int priceInCents = StartOfPrice + levelIndex;

  return Price(priceInCents);
}

std::pair<size_t, size_t> OrderBook::priceToBitmapIndex(Price price) {
  int levelIndex = priceToIndex(price);

  return indexToBitmapIndex(levelIndex);
}

std::pair<size_t, size_t> OrderBook::indexToBitmapIndex(int levelIndex) {
  size_t wordIndex = levelIndex / CHUNK;
  size_t bitIndex = levelIndex % CHUNK;

  DEBUG_PRINT("Word Index: " << wordIndex);
  DEBUG_PRINT("Bit Index: " << bitIndex);

  return {wordIndex, bitIndex};
}

void OrderBook::setBidBitTo1(const Price &price) {
  auto result = priceToBitmapIndex(price);
  size_t wordPos = result.first;
  size_t bitPos = result.second;

  u_int64_t bitmask = (1ULL << bitPos);

  mBidBitmap[wordPos] |= bitmask;
}

void OrderBook::setBidBitTo0(const Price &price) {
  DEBUG_PRINT("entering setBidBitTo0");
  auto result = priceToBitmapIndex(price);
  size_t wordPos = result.first;
  size_t bitPos = result.second;

  mBidBitmap[wordPos] &= ~(1ULL << bitPos);
}

void OrderBook::setAskBitTo1(const Price &price) {
  auto result = priceToBitmapIndex(price);
  size_t wordPos = result.first;
  size_t bitPos = result.second;

  u_int64_t bitmask = (1ULL << bitPos);

  mAskBitmap[wordPos] |= bitmask;
}

void OrderBook::setAskBitTo0(const Price &price) {
  DEBUG_PRINT("entering setAskBitTo0");
  auto result = priceToBitmapIndex(price);
  size_t wordPos = result.first;
  size_t bitPos = result.second;

  mAskBitmap[wordPos] &= ~(1ULL << bitPos);
}

Order *OrderBook::getBestBid() {
  Order *bestBid = tryGetBestBid();

  if (bestBid == nullptr) {
    throw std::runtime_error("No bids in the OrderBook!");
  }
  return bestBid;
}

Order *OrderBook::getBestAsk() {
  Order *bestAsk = tryGetBestAsk();

  if (bestAsk == nullptr) {
    throw std::runtime_error("No Asks in the OrderBook");
  }
  return bestAsk;
}

Order *OrderBook::tryGetBestBid() {
  if (mBestBidLevel == -1)
    return nullptr;
  return mBidpriceLevel[mBestBidLevel].head;
}

Order *OrderBook::tryGetBestAsk() {
  if (mBestAskLevel == -1)
    return nullptr;
  return mAskPriceLevel[mBestAskLevel].head;
}

int OrderBook::rescanBestBidLevel() {
  int level = findBestBidLevel();
  // drop stale bits left behind by levels that no longer hold orders
  while (level != -1 && mBidpriceLevel[level].head == nullptr) {
    mBidBitmap[level / CHUNK] &= ~(1ULL << (level % CHUNK));
    level = findBestBidLevel();
  }
  mBestBidLevel = level;
  return level;
}

int OrderBook::rescanBestAskLevel() {
  int level = findBestAskLevel();
  while (level != -1 && mAskPriceLevel[level].head == nullptr) {
    mAskBitmap[level / CHUNK] &= ~(1ULL << (level % CHUNK));
    level = findBestAskLevel();
  }
  mBestAskLevel = level;
  return level;
}

int OrderBook::findBestBidLevel() const {
  for (int i = static_cast<int>(mBidBitmap.size() - 1); i >= 0; --i) {
    u_int64_t word = mBidBitmap[i];

    if (word != 0) {
      int leadingZeros = std::__countl_zero(word);

      int bitPositionFromRight = 63 - leadingZeros;

      return i * CHUNK + bitPositionFromRight;
    }
  }
  return -1;
}

int OrderBook::findBestAskLevel() const {
  for (int i = 0; i < static_cast<int>(mAskBitmap.size()); ++i) {

    u_int64_t word = mAskBitmap[i];

    if (word != 0) {

      int trailingZeros = std::__countr_zero(word);

      return i * CHUNK + trailingZeros;
    }
  }
  return -1;
}

std::pair<Price, Side>
OrderBook::orderIdToPrice(const u_int64_t orderId) const {
  const Order *order = orderIdtoOrderMapping[orderId % POOL_SIZE];

  if (order == nullptr)
    throw std::runtime_error("there was no orderID with this number:" +
                             std::to_string(orderId));
  return std::make_pair(order->mPrice, order->BidOrAsk);
}

void OrderBook::removeOrderFromOrderId(u_int64_t orderId) {
  Order *order = orderIdtoOrderMapping[orderId % POOL_SIZE];

  // the slot is either empty or reused by a newer order whose id collides
  if (order == nullptr || order->mOrderID != orderId)
    return;

  removeOrder(order);
}

void OrderBook::printOrderBook() const {
  std::cout
      << "--------  OrderBook  ---------------------------------------------"
      << std::endl;
  std::cout << "\nBIDS (high -> low)\n";
  for (int i = static_cast<int>(mBidpriceLevel.size()) - 1; i >= 0; --i) {
    if (mBidpriceLevel[i].head == nullptr)
      continue;

    int priceTicks = StartOfPrice + i;
    float TicksToPrice = static_cast<float>(priceTicks / 100.0f);

    std::cout << "Level " << i << " (Price €" << TicksToPrice << "): ";

    for (Order *o = mBidpriceLevel[i].head; o != nullptr; o = o->mNext) {
      std::cout << "[id=" << o->mOrderID << ", qty=" << o->mVolume
                << ", user=" << o->mName << "] ";
    }
    std::cout << "\n";
  }

  std::cout << "\nASKS (low -> high)\n";
  for (int i = 0; i < static_cast<int>(mAskPriceLevel.size()); ++i) {
    if (mAskPriceLevel[i].head == nullptr)
      continue;

    int priceTicks = StartOfPrice + i;
    std::cout << "Level " << i << " (ticks " << priceTicks << "): ";

    for (Order *o = mAskPriceLevel[i].head; o != nullptr; o = o->mNext) {
      std::cout << "[id=" << o->mOrderID << ", qty=" << o->mVolume
                << ", Asset=" << o->mName << "] ";
    }
    std::cout << "\n";
  }
  std::cout
      << "------------------------------------------------------------------"
      << std::endl;
}
