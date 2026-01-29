#include "MatchingEngine.hpp"

// Constructor
MatchingEngine::MatchingEngine(OrderBook& orderBook)
    : orderBook(orderBook)
{
    std::cout << "matchingEngine was constructed!" << std::endl;
}

void MatchingEngine::matchLimitOrders(){ 

    Order bestAsk = orderBook.getBestAsk();
    Order BestBid = orderBook.getBestBid();
    std::cout << "Best Ask: ";
    bestAsk.PrintOrder();

    std::cout << "Best Bid: ";
    BestBid.PrintOrder();
}

int main(){

    OrderBook Book2(101);
    Order ask1 = Order(Ask, 101, "apple", 100, 1040);
    Order bid1 = Order(Bid, 102, "apple", 100, 1040);
    
    Order ask2 = Order(Ask, 103, "apple", 100, 1000);
    Order bid2 = Order(Bid, 104, "apple", 100, 1000);
    
    Order ask3 = Order(Ask, 105, "apple", 100, 1035);
    Order bid3 = Order(Bid, 106, "apple", 100, 1035);
    
    Book2.addAsk(ask1);
    Book2.addBid(bid1);
    
    Book2.addAsk(ask2);
    Book2.addBid(bid2);
    
    Book2.addAsk(ask3);
    Book2.addBid(bid3);

    MatchingEngine APPL_matchingEngine(Book2);

    APPL_matchingEngine.matchLimitOrders();
    
}