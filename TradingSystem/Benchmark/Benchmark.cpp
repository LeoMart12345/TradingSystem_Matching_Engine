// benchmark.cpp
#include <iostream>
#include <chrono>
#include "../TradingSystem.hpp"
#include "../Matching_Engine/OrderBook/OrderBook.hpp"

void runInsertBenchmark(size_t numOrders) {
    auto TS = std::make_unique<TradingSystem>(5555, 12345);
    OrderBook& orderBook = TS->getOrderBook();
    
    auto start = std::chrono::high_resolution_clock::now();

    for(size_t i = 0; i < numOrders; i++){
        Order order = orderBook.generateRandomOrder();
        TS->addOrder(order);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Added " << numOrders << " orders in " << duration.count() << " μs" << std::endl;
    std::cout << "Average latency: " << duration.count() / static_cast<double>(numOrders) << " μs" << std::endl;
}

int main() {
    runInsertBenchmark(1000);
    runInsertBenchmark(10000);
    return 0;
}