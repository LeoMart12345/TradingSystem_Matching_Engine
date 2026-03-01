#include "TradingSystem.hpp"
#include "Matching_Engine/OrderBook/OrderBook.hpp"

int main(){

    auto TS  = std::make_unique<TradingSystem>(5555, 12345);

    OrderBook& orderBook = TS->getOrderBook();
    
    // google benchmark?
    
    // starts the tcp server and then does some benchmarking.
    TS->start();
}