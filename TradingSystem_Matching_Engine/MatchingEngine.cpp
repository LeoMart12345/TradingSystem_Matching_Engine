#include "MatchingEngine.hpp"

// Constructor
MatchingEngine::MatchingEngine(OrderBook orderBook){
    std::cout << "matchingEngine was constructed!" << std::endl;
}

void MatchingEngine::matchLimitOrders(OrderBook orderBook){ 
    orderBook.getBestAsk();
    orderBook.getBestBid();

    
}

int main(){

    OrderBook Book2(101);

    MatchingEngine APPL_matchingEngine(Book2);

    APPL_matchingEngine.matchLimitOrders(Book2);
   
}