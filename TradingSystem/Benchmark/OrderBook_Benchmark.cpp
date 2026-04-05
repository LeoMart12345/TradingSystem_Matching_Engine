// OrderBook_Benchmark.cpp
#include "../Matching_Engine/MatchingEngine.hpp"
#include "../Matching_Engine/OrderBook/OrderBook.hpp"
#include "../UtilityClass/OrderIdGenerator.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <vector>

static constexpr size_t WARMUP = 500;
// Pool is 10k slots, keeping totals under that
std::mt19937 bench_rng(42); // fixed seed for reproducible runs
std::uniform_int_distribution<int> bench_priceDist(950, 1050);
std::uniform_int_distribution<int> bench_sideDist(0, 1);
std::uniform_int_distribution<int> bench_volDist(1, 100);

void printPercentiles(std::vector<long long> &latencies) {
  std::sort(latencies.begin(), latencies.end());
  size_t n = latencies.size();
  std::cout << "  p50:  " << latencies[n * 0.50] << " ns\n";
  std::cout << "  p99:  " << latencies[n * 0.99] << " ns\n";
  std::cout << "  p999: " << latencies[n * 0.999] << " ns\n";
}

// INSERT
void benchmarkInsert(size_t numOrders) {
  OrderBook orderBook(101);
  auto &pool = orderBook.getOrderPool();

  // Pre-acquire and fill ALL orders before any timing starts
  std::vector<Order *> orders;
  orders.reserve(numOrders + WARMUP);

  for (size_t i = 0; i < numOrders + WARMUP; i++) {
    Order *o = pool.acquire();
    o->BidOrAsk = (bench_sideDist(bench_rng) == 0) ? Bid : Ask;
    o->mVolume = bench_volDist(bench_rng);
    o->mName = "TSLA";
    o->mOrderID = OrderIdGenerator::incrementOrder();
    o->mPrice = Price(bench_priceDist(bench_rng));
    orders.push_back(o);
  }

  // Warmup is not timed
  for (size_t i = 0; i < WARMUP; i++) {
    if (orders[i]->BidOrAsk == Bid)
      orderBook.addBid(orders[i]);
    else
      orderBook.addAsk(orders[i]);
  }

  // Timed run
  std::vector<long long> latencies;
  latencies.reserve(numOrders);

  for (size_t i = WARMUP; i < numOrders + WARMUP; i++) {
    auto start = std::chrono::high_resolution_clock::now();
    if (orders[i]->BidOrAsk == Bid)
      orderBook.addBid(orders[i]);
    else
      orderBook.addAsk(orders[i]);
    auto end = std::chrono::high_resolution_clock::now();
    latencies.push_back(
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
            .count());
  }

  std::cout << "INSERT (" << numOrders << " orders):\n";
  printPercentiles(latencies);
}

// Cancel
void benchmarkCancel(size_t numOrders) {
  OrderBook orderBook(101);
  auto &pool = orderBook.getOrderPool();

  // Pre-load book, record IDs for cancelling
  std::vector<u_int64_t> orderIds;
  orderIds.reserve(numOrders);

  for (size_t i = 0; i < numOrders; i++) {
    Order *o = pool.acquire();
    o->BidOrAsk = (bench_sideDist(bench_rng) == 0) ? Bid : Ask;
    o->mVolume = bench_volDist(bench_rng);
    o->mName = "TSLA";
    o->mOrderID = OrderIdGenerator::incrementOrder();
    o->mPrice = Price(bench_priceDist(bench_rng));
    orderIds.push_back(o->mOrderID);
    if (o->BidOrAsk == Bid)
      orderBook.addBid(o);
    else
      orderBook.addAsk(o);
  }

  // Time just the cancels
  std::vector<long long> latencies;
  latencies.reserve(numOrders);

  for (size_t i = 0; i < numOrders; i++) {
    auto start = std::chrono::high_resolution_clock::now();
    orderBook.removeOrderFromOrderId(orderIds[i]);
    auto end = std::chrono::high_resolution_clock::now();
    latencies.push_back(
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
            .count());
  }

  std::cout << "CANCEL (" << numOrders << " orders):\n";
  printPercentiles(latencies);
}

// Match
void benchmarkMatch(size_t numPairs) {
  // numPairs * 2 pool slots — keep under 10k
  OrderBook orderBook(101);
  MatchingEngine engine(orderBook);
  auto &pool = orderBook.getOrderPool();

  // Pre-load crossing orders: bid@1000 vs ask@990 always matchable
  for (size_t i = 0; i < numPairs; i++) {
    Order *bid = pool.acquire();
    bid->BidOrAsk = Bid;
    bid->mVolume = 1;
    bid->mName = "TSLA";
    bid->mOrderID = OrderIdGenerator::incrementOrder();
    bid->mPrice = Price(1000);
    orderBook.addBid(bid);

    Order *ask = pool.acquire();
    ask->BidOrAsk = Ask;
    ask->mVolume = 1;
    ask->mName = "TSLA";
    ask->mOrderID = OrderIdGenerator::incrementOrder();
    ask->mPrice = Price(990);
    orderBook.addAsk(ask);
  }

  std::vector<long long> latencies;
  latencies.reserve(numPairs);

  for (size_t i = 0; i < numPairs; i++) {
    auto start = std::chrono::high_resolution_clock::now();
    engine.matchLimitOrders();
    auto end = std::chrono::high_resolution_clock::now();
    latencies.push_back(
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
            .count());
  }

  std::cout << "MATCH (" << numPairs << " pairs):\n";
  printPercentiles(latencies);
}

int main() {
  std::cout << "OrderBook Microbenchmark \n\n";

  benchmarkInsert(5000); // 5500 pool slots used (with warmup)
  benchmarkCancel(5000); // 5000 slots, all released during cancel
  benchmarkMatch(4000);  // 8000 slots (2 per pair), under 10k limit

  return 0;
}
