// TradingSystemTest.cpp
// Using Google Test to Test the System
#include "MatchingEngine.hpp"
#include "OrderBook.hpp"
#include "Price.hpp"
#include "Order.hpp"
#include <gtest/gtest.h>

static Order makeOrder(Side side, int price, int volume) {
  Order o;
  o.BidOrAsk = side;
  o.mPrice = Price(price);
  o.mVolume = volume;
  o.mName = "TSLA";
  return o;
}

// A bid inserted at a given price should be retrievable as the best bid
TEST(TradingSystemTest, InsertBidGetsBestBid) {
  OrderBook book(101);
  MatchingEngine engine(book);
  engine.addOrder(makeOrder(Bid, 1000, 10));
  Order *best = book.getBestBid();
  ASSERT_NE(best, nullptr);
  EXPECT_EQ(best->getPrice().getPriceInTicks(), 1000);
}

// The highest-priced bid should always be the best bid
TEST(TradingSystemTest, BidPricePriority) {
  OrderBook book(101);
  MatchingEngine engine(book);
  engine.addOrder(makeOrder(Bid, 990, 10));
  engine.addOrder(makeOrder(Bid, 1000, 10));
  Order *best = book.getBestBid();
  ASSERT_NE(best, nullptr);
  EXPECT_EQ(best->getPrice().getPriceInTicks(), 1000);
}

// The lowest-priced ask should always be the best ask
TEST(TradingSystemTest, AskPricePriority) {
  OrderBook book(101);
  MatchingEngine engine(book);
  engine.addOrder(makeOrder(Ask, 1010, 10));
  engine.addOrder(makeOrder(Ask, 1000, 10));
  Order *best = book.getBestAsk();
  ASSERT_NE(best, nullptr);
  EXPECT_EQ(best->getPrice().getPriceInTicks(), 1000);
}

// A matching bid and ask should produce a trade with correct price and volume
TEST(TradingSystemTest, MatchingBidAndAskProducesTrade) {
  OrderBook book(101);
  MatchingEngine engine(book);
  engine.addOrder(makeOrder(Bid, 1000, 10));
  engine.addOrder(makeOrder(Ask, 1000, 10));
  auto trade = engine.matchLimitOrders();
  ASSERT_TRUE(trade.has_value());
  EXPECT_EQ(trade->getPrice(), 1000u);
  EXPECT_EQ(trade->getVolume(), 10u);
}

// Cancelling the only bid should leave the book empty
TEST(TradingSystemTest, CancelRemovesOrder) {
  OrderBook book(101);
  MatchingEngine engine(book);
  uint64_t id = engine.addOrder(makeOrder(Bid, 1000, 10));
  engine.cancelOrder(id);
  EXPECT_EQ(book.findBestBidLevel(), -1);
}

// Matching Engine Logic Tests

// A partial fill should produce a trade for the smaller volume with the bid
// remaining
TEST(TradingSystemTest, PartialFill) {
  OrderBook book(101);
  MatchingEngine engine(book);
  engine.addOrder(makeOrder(Bid, 1000, 10));
  engine.addOrder(makeOrder(Ask, 1000, 5));
  auto trade = engine.matchLimitOrders();
  ASSERT_TRUE(trade.has_value());
  EXPECT_EQ(trade->getVolume(), 5u);
  EXPECT_NE(book.getBestBid(), nullptr);
}

// No trade should occur when the best bid is below the best ask
TEST(TradingSystemTest, NoMatchWhenSpreadExists) {
  OrderBook book(101);
  MatchingEngine engine(book);
  engine.addOrder(makeOrder(Bid, 990, 10));
  engine.addOrder(makeOrder(Ask, 1010, 10));
  auto trade = engine.matchLimitOrders();
  EXPECT_FALSE(trade.has_value());
}

// A full fill should remove both orders leaving the book empty on both sides
TEST(TradingSystemTest, FullFillRemovesBothOrders) {
  OrderBook book(101);
  MatchingEngine engine(book);
  engine.addOrder(makeOrder(Bid, 1000, 10));
  engine.addOrder(makeOrder(Ask, 1000, 10));
  engine.matchLimitOrders();
  EXPECT_EQ(book.findBestBidLevel(), -1);
  EXPECT_EQ(book.findBestAskLevel(), -1);
}

// Order should survive a serialise/deserialise round trip
TEST(TradingSystemTest, OrderSerialiseDeserialise) {
  Order o = makeOrder(Bid, 1000, 10);
  o.mOrderID = 1;
  std::string serialised = o.serialize();
  Order deserialised = Order::deserialize(serialised);
  EXPECT_EQ(deserialised.mPrice.getPriceInTicks(), 1000);
  EXPECT_EQ(deserialised.mVolume, 10u);
  EXPECT_EQ(deserialised.BidOrAsk, Bid);
}

// Trade serialises to price:volume format
TEST(TradingSystemTest, TradeSerialise) {
  OrderBook book(101);
  MatchingEngine engine(book);
  engine.addOrder(makeOrder(Bid, 1000, 10));
  engine.addOrder(makeOrder(Ask, 1000, 10));
  auto trade = engine.matchLimitOrders();
  ASSERT_TRUE(trade.has_value());
  EXPECT_EQ(trade->serialise(), "1000:10");
}

// Cancelling an order in the middle of a level must keep price-time priority
TEST(TradingSystemTest, CancelMiddleOrderKeepsPriority) {
  OrderBook book(101);
  MatchingEngine engine(book);
  uint64_t first = engine.addOrder(makeOrder(Bid, 1000, 10));
  uint64_t second = engine.addOrder(makeOrder(Bid, 1000, 20));
  uint64_t third = engine.addOrder(makeOrder(Bid, 1000, 30));

  engine.cancelOrder(second);

  ASSERT_NE(book.getBestBid(), nullptr);
  EXPECT_EQ(book.getBestBid()->getOrderId(), first);

  engine.cancelOrder(first);
  ASSERT_NE(book.getBestBid(), nullptr);
  EXPECT_EQ(book.getBestBid()->getOrderId(), third);
  EXPECT_EQ(book.getBestBid()->mVolume, 30u);

  engine.cancelOrder(third);
  EXPECT_EQ(book.findBestBidLevel(), -1);
}

// Cancelling a resting order away from the best level must not disturb the top
TEST(TradingSystemTest, CancelNonBestLevelOrder) {
  OrderBook book(101);
  MatchingEngine engine(book);
  uint64_t best = engine.addOrder(makeOrder(Bid, 1000, 10));
  uint64_t worse = engine.addOrder(makeOrder(Bid, 990, 10));

  engine.cancelOrder(worse);

  ASSERT_NE(book.getBestBid(), nullptr);
  EXPECT_EQ(book.getBestBid()->getOrderId(), best);
  auto [bidVolume, askVolume] = book.getVolumeAtLevelFromPrice(Price(990));
  EXPECT_EQ(bidVolume, 0u);
  EXPECT_EQ(askVolume, 0u);
}

// Cancelling an unknown order id must be a no-op
TEST(TradingSystemTest, CancelUnknownOrderIsIgnored) {
  OrderBook book(101);
  MatchingEngine engine(book);
  uint64_t id = engine.addOrder(makeOrder(Ask, 1000, 10));

  engine.cancelOrder(id + 12345);
  engine.cancelOrder(id);

  EXPECT_EQ(book.findBestAskLevel(), -1);
}

// reduceVolume correctly decrements the order volume
TEST(TradingSystemTest, ReduceVolume) {
  Order o = makeOrder(Bid, 1000, 10);
  o.reduceVolume(4);
  EXPECT_EQ(o.GetVolume(), 6);
}
