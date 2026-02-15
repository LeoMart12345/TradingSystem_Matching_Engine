#include "TradingSystem.hpp"
#include "Matching_Engine/OrderBook/OrderBook.hpp"

int main(){

    auto TS  = std::make_unique<TradingSystem>(5555, 12345);

    OrderBook& orderBook = TS->getOrderBook();

    // Benchmarking START
    for(int i = 0; i < 1000; i++){
        Order order = orderBook.generateRandomOrder();
        TS->addOrder(order);       
    }
    // Benchmarking END
    
    // starts the tcp server and then does some benchmarking.
    TS->start();
}