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

// TODO: remove this to test
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
  int BidLevel = priceToIndex(order->mPrice);
  mBidpriceLevel[BidLevel].emplace_back(order);
  DEBUG_PRINT("ORDERBOOK::ADDBID printing price of the order below.");
  setBidBitTo1(order->getPrice());
  // add the orderid to the array
  orderIdtoPriceMapping[order->mOrderID % POOL_SIZE] = {order->mPrice, Bid,
                                                        true};
  DEBUG_PRINT("ORDERBOOK::ADDBID after printing price");
}

void OrderBook::addAsk(Order *order) {
  DEBUG_PRINT("OrderBook::addAsk - Order ID: " << order->mOrderID);
  int AskLevel = priceToIndex(order->mPrice);
  mAskPriceLevel[AskLevel].emplace_back(order);
  setAskBitTo1(order->getPrice());
  orderIdtoPriceMapping[order->mOrderID % POOL_SIZE] = {order->mPrice, Ask,
                                                        true};
  DEBUG_PRINT("ORDERBOOK::ADDASK after printing price");
}

// fixe to use deque with pointers.
void OrderBook::removeBid(u_int64_t orderId) {
  int bestLevel = findBestBidLevel();
  if (bestLevel == -1)
    return;

  auto &deque = mBidpriceLevel[bestLevel];

  if (deque.empty()) {
    setBidBitTo0(indexToPrice(bestLevel)); // clear stale bit
    return;
  }

  DEBUG_PRINT("front of deque Id: " << deque.front()->getOrderId()
                                    << " OrderId: " << orderId);

  if (deque.front()->getOrderId() != orderId) {
    std::__throw_logic_error(
        "orderId does not match the order at the front of that queue level");
  }
  // Release the orderPool here:
  Order *orderTop = deque.front();

  mOrderPool.release(orderTop);
  // pop from the deque.
  deque.pop_front();
  // removing from the
  orderIdtoPriceMapping[orderId % POOL_SIZE] = {};

  if (deque.empty()) {
    setBidBitTo0(indexToPrice(bestLevel));
  }
}

void OrderBook::removeAsk(u_int64_t orderId) {
  int bestLevel = findBestAskLevel();
  if (bestLevel == -1)
    return;
  auto &deque = mAskPriceLevel[bestLevel];

  if (deque.empty()) {
    setAskBitTo0(indexToPrice(bestLevel)); // clear stale bit
    return;
  }

  if (deque.front()->getOrderId() != orderId) {
    std::__throw_logic_error(
        "orderId does not match the order at thr front of that queue level");
  }
  // release order to the orderPool again!
  Order *orderTop = deque.front();
  mOrderPool.release(orderTop);
  // Then pop
  deque.pop_front();
  orderIdtoPriceMapping[orderId % POOL_SIZE] = {};
  if (deque.empty()) {
    setAskBitTo0(indexToPrice(bestLevel));
  }
}

std::pair<u_int64_t, u_int64_t>
OrderBook::getVolumeAtLevelFromPrice(const Price &price) {
  // TODO: IMPLEMENT
  int level = priceToIndex(price);

  uint64_t totalBidVolume = 0;
  uint64_t totalAskVolume = 0;

  for (const auto &order : mBidpriceLevel[level]) {
    totalBidVolume += order->mVolume;
  }

  for (const auto &order : mAskPriceLevel[level]) {
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
  int bestBidLevel = findBestBidLevel();

  if (bestBidLevel == -1) {
    throw std::runtime_error("No bids in the OrderBook!");
  }
  auto &deque = mBidpriceLevel[bestBidLevel];

  if (deque.empty()) {
    throw std::runtime_error(
        "bitmapp shows the level marked but the deque is empty");
  }

  return deque.front();
}

Order *OrderBook::getBestAsk() {
  int bestAskLevel = findBestAskLevel();

  if (bestAskLevel == -1) {
    throw std::runtime_error("No Asks in the OrderBook");
  }
  auto &deque = mAskPriceLevel[bestAskLevel];

  if (deque.empty()) {
    throw std::runtime_error(
        "bitmapp shows the level marked but the deque is empty");
  }

  return deque.front();
}

Order *OrderBook::popBestBid() {
  int bestBidLevel = findBestBidLevel();

  if (bestBidLevel == -1) {
    throw std::runtime_error("No bids in the OrderBook!");
  }
  auto &deque = mBidpriceLevel[bestBidLevel];

  if (deque.empty()) {
    throw std::runtime_error(
        "bitmapp shows the level marked but the deque at that level is empty!");
  }

  Order *order = deque.front();
  deque.pop_front();

  if (deque.empty()) {
    setBidBitTo0(indexToPrice(bestBidLevel));
  }

  return order;
}

Order *OrderBook::popBestAsk() {
  int bestAskLevel = findBestAskLevel();

  if (bestAskLevel == -1) {
    throw std::runtime_error("no asks in the OrderBook!");
  }
  auto &deque = mAskPriceLevel[bestAskLevel];
  if (deque.empty()) {
    throw std::runtime_error(
        "bitmap shows the level marked but the deque at that level is empty!");
  }
  Order *order = deque.front();
  deque.pop_front();

  if (deque.empty()) {
    setAskBitTo0(indexToPrice(bestAskLevel));
  }

  return order;
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
  const auto &loc = orderIdtoPriceMapping[orderId % POOL_SIZE];

  if (!loc.isValid)
    throw std::runtime_error("there was no orderID with this number:" +
                             std::to_string(orderId));
  return std::make_pair(loc.price, loc.side);
}

// TODO: add the objectPool release to this:
void OrderBook::removeOrderFromOrderId(u_int64_t orderId) {
  try {
    std::pair<Price, Side> orderPair = orderIdToPrice(orderId);
    int orderIndex = priceToIndex(orderPair.first);

    if (orderPair.second == Side::Bid) {
      std::deque<Order *> &pOrderDeQue = mBidpriceLevel[orderIndex];
      for (auto it = pOrderDeQue.begin(); it != pOrderDeQue.end(); it++) {
        if ((*it)->mOrderID == orderId) {
          mOrderPool.release(*it);
          pOrderDeQue.erase(it);
          if (pOrderDeQue.empty()) {
            setBidBitTo0(orderPair.first);
          }
          break;
        }
      }
    } else {
      std::deque<Order *> &pOrderDeQue = mAskPriceLevel[orderIndex];
      for (auto it = pOrderDeQue.begin(); it != pOrderDeQue.end(); it++) {
        if ((*it)->mOrderID == orderId) {
          mOrderPool.release(*it);
          pOrderDeQue.erase(it);
          if (pOrderDeQue.empty()) {
            setAskBitTo0(orderPair.first);
          }
          break;
        }
      }
    }
    orderIdtoPriceMapping[orderId % POOL_SIZE] = {};
  } catch (const std::runtime_error &e) {
    // can ignore if its not found as it migh have been matched already
    // std::cout << "Error: " << e.what() << std::endl;
  }
}

void OrderBook::printOrderBook() const {
  std::cout
      << "--------  OrderBook  ---------------------------------------------"
      << std::endl;
  std::cout << "\nBIDS (high -> low)\n";
  for (int i = static_cast<int>(mBidpriceLevel.size()) - 1; i >= 0; --i) {
    if (mBidpriceLevel[i].empty())
      continue;

    int priceTicks = StartOfPrice + i;
    float TicksToPrice = static_cast<float>(priceTicks / 100.0f);

    std::cout << "Level " << i << " (Price €" << TicksToPrice << "): ";

    for (const auto &o : mBidpriceLevel[i]) {
      std::cout << "[id=" << o->mOrderID << ", qty=" << o->mVolume
                << ", user=" << o->mName << "] ";
    }
    std::cout << "\n";
  }

  std::cout << "\nASKS (low -> high)\n";
  for (int i = 0; i < static_cast<int>(mAskPriceLevel.size()); ++i) {
    if (mAskPriceLevel[i].empty())
      continue;

    int priceTicks = StartOfPrice + i;
    std::cout << "Level " << i << " (ticks " << priceTicks << "): ";

    for (const auto &o : mAskPriceLevel[i]) {
      std::cout << "[id=" << o->mOrderID << ", qty=" << o->mVolume
                << ", Asset=" << o->mName << "] ";
    }
    std::cout << "\n";
  }
  std::cout
      << "------------------------------------------------------------------"
      << std::endl;
}
