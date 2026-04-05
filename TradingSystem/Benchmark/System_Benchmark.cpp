// System_Benchmark.cpp

// measures client serialize ->tcp -> server deserialize -> matching engine ->
// client response
#include "../Matching_Engine/MatchingEngine.hpp"
#include "../Matching_Engine/OrderBook/OrderBook.hpp"
#include "../Network Servers/TCPOrderSession.hpp"
#include "../Network Servers/TCP_Server.hpp"
#include "../UtilityClass/OrderIdGenerator.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

static constexpr size_t WARMUP = 200;
static constexpr size_t N = 5000;
static constexpr size_t THROUGHPUT_CLIENTS = 3;
static constexpr size_t THROUGHPUT_N = 1000;

namespace {
std::mt19937 rng(42);
std::uniform_int_distribution<int> priceDist(950, 1050);
std::uniform_int_distribution<int> sideDist(0, 1);
std::uniform_int_distribution<int> volDist(1, 100);
} // namespace

void printPercentiles(std::vector<long long> &latencies) {
  std::sort(latencies.begin(), latencies.end());
  size_t n = latencies.size();
  std::cout << "  p50:  " << latencies[n * 0.50] << " ns\n";
  std::cout << "  p99:  " << latencies[n * 0.99] << " ns\n";
  std::cout << "  p999: " << latencies[n * 0.999] << " ns\n";
}

int main() {
  // Start server components directly
  OrderBook orderBook(101);
  MatchingEngine engine(orderBook);
  TCPServer server(5555, engine);

  std::atomic<bool> running{true};

  std::thread serverThread([&server]() { server.run(); });
  serverThread.detach();

  std::thread matchThread([&engine, &running]() {
    while (running.load(std::memory_order_relaxed)) {
      engine.matchLimitOrders();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });
  matchThread.detach();

  // Give TCP server time to bind
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  TCPOrderSession session;
  session.connect("127.0.0.1", 5555);

  // Pre-generate orders mOrderID left 0, server overwrites it
  std::vector<Order> orders;
  orders.reserve(N + WARMUP);
  for (size_t i = 0; i < N + WARMUP; i++) {
    Order o;
    o.BidOrAsk = (sideDist(rng) == 0) ? Bid : Ask;
    o.mVolume = volDist(rng);
    o.mName = "TSLA";
    o.mPrice = Price(priceDist(rng));
    orders.push_back(o);
  }

  // Warmup: not timed, heats up TCP buffers and branch predictors
  for (size_t i = 0; i < WARMUP; i++) {
    uint64_t id = session.placeOrder(orders[i]);
    session.cancelOrder(id);
  }

  // Timed run: cancel is outside the timing window to keep pool free
  std::vector<long long> latencies;
  latencies.reserve(N);

  for (size_t i = WARMUP; i < N + WARMUP; i++) {
    auto t1 = std::chrono::steady_clock::now();
    uint64_t id = session.placeOrder(orders[i]);
    auto t2 = std::chrono::steady_clock::now();

    latencies.push_back(
        std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count());

    session.cancelOrder(id);
  }

  std::cout << "System Benchmark — TCP round-trip (" << N << " orders)\n\n";
  std::cout << "PLACE ORDER:\n";
  printPercentiles(latencies);

  // Throughput benchmarking
  std::cout << "throughput benchmarking " << THROUGHPUT_CLIENTS << " Clients x "
            << THROUGHPUT_N << "orders each" << std::endl;

  std::vector<std::vector<Order>> clientOrders(THROUGHPUT_CLIENTS);
  for (auto &orders : clientOrders) {
    orders.reserve(THROUGHPUT_N);
    for (size_t i = 0; i < THROUGHPUT_N; i++) {
      Order o;
      o.BidOrAsk = (sideDist(rng) == 0) ? Bid : Ask;
      o.mVolume = volDist(rng);
      o.mName = "TSLA";
      o.mPrice = Price(priceDist(rng));
      orders.push_back(o);
    }
  }

  std::vector<std::thread> throughputThreads;

  for (size_t c = 0; c < THROUGHPUT_CLIENTS; c++) {
    throughputThreads.emplace_back([&, c]() {
      TCPOrderSession session;
      session.connect("127.0.0.1", 5555);
      for (size_t i = 0; i < THROUGHPUT_N; i++) {
        uint64_t id = session.placeOrder(clientOrders[c][i]);
        session.cancelOrder(id);
      }
    });
  }

  // Give threads time to connect before timing starts
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  auto tp1 = std::chrono::steady_clock::now();
  for (auto &t : throughputThreads)
    t.join();
  auto tp2 = std::chrono::steady_clock::now();

  double elapsed_s = std::chrono::duration<double>(tp2 - tp1).count();
  size_t total = THROUGHPUT_CLIENTS * THROUGHPUT_N;

  std::cout << "  Total orders: " << total << "\n";
  std::cout << "  Elapsed:      " << elapsed_s * 1000.0 << " ms\n";
  std::cout << "  Throughput:   " << (size_t)(total / elapsed_s)
            << " orders/sec\n";

  running = false;
  return 0;
}
