// Benchmark.cpp
#include <iostream>
#include <chrono>
#include "../TradingSystem.hpp"
#include "../Matching_Engine/OrderBook/OrderBook.hpp"

void runInsertBenchmark(size_t numOrders, size_t warmupOrders = 500) {
    auto TS = std::make_unique<TradingSystem>(5555, 12345);
    OrderBook& orderBook = TS->getOrderBook();
    
    // Warmup phase not measured
    for(size_t i = 0; i < warmupOrders; i++){
        Order order = orderBook.generateRandomOrder();
        TS->addOrder(order);
    }

    // Measure individual latencies instead of bulk
    std::vector<long long> latencies;
    latencies.reserve(numOrders);

    for(size_t i = 0; i < numOrders; i++){
        Order order = orderBook.generateRandomOrder();  // I will pre-generate these after

        auto start = std::chrono::high_resolution_clock::now();
        TS->addOrder(order);
        auto end = std::chrono::high_resolution_clock::now();

        latencies.push_back(
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
        );
    }

    // Percentiles
    std::sort(latencies.begin(), latencies.end());
    std::cout << "p50: " << latencies[numOrders * 0.50] << " ns\n";
    std::cout << "p99: " << latencies[numOrders * 0.99] << " ns\n";
    std::cout << "p999: " << latencies[numOrders * 0.999] << " ns\n";
}

int main() {
    runInsertBenchmark(100);
    runInsertBenchmark(1000);
    runInsertBenchmark(10000);
    runInsertBenchmark(100000);
    std::cout<< "Benchmark Code is running" << std::endl;

    return 0;
}