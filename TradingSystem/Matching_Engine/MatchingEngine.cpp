// #include "MatchingEngine.hpp"

#include "./Matching_Engine/MatchingEngine.hpp"

#include "Trade.hpp"
#include <optional>
#include <atomic>
#include <thread>
#include <mutex>
#include <sstream>


// Constructor
MatchingEngine::MatchingEngine(OrderBook& orderBook)
    : orderBook(orderBook)
{
    std::cout << "matchingEngine was constructed!" << std::endl;
}

// void MatchingEngine::processOrder(OrderRequest orderRequest){
    
//     std::cout << "TESTING" << std::endl;
//     orderRequest.requestOrder.PrintOrder();

// }

u_int64_t MatchingEngine::addOrder(Order order){

    order.mOrderID = ++nextOrderId;

    std::cout << "SERVER: Assigned Order ID: " << order.mOrderID << std::endl;
    std::cout << "SERVER: Order details - " 
              << (order.BidOrAsk == Side::Bid ? "BUY" : "SELL")
              << " " << order.mVolume << " " << order.mName 
              << " @ " << order.mPrice.getPriceInTicks() << std::endl;

    if(order.BidOrAsk == Side::Bid){
        orderBook.addBid(order);
    }else{
        orderBook.addAsk(order);
    }

    return order.mOrderID;
}

std::optional<Trade> MatchingEngine::matchLimitOrders(){ 

    Order& bestAsk = orderBook.getBestAsk();
    Order& bestBid = orderBook.getBestBid();
    
    u_int64_t bidPrice = bestBid.getPrice().getPriceInTicks();
    u_int64_t askPrice = bestAsk.getPrice().getPriceInTicks();

    if(bidPrice >= askPrice){
        std::cout << "Match found! Bid(" << bidPrice << ") >= Ask(" << askPrice << ")" << std::endl;
        
        u_int64_t tradeVolume = std::min(bestAsk.GetVolume(), bestBid.GetVolume());

        
        u_int64_t tradePrice; 

         if (bidPrice > askPrice) {
            tradePrice = askPrice;
            std::cout << "Negative spread! Using ask price: " << tradePrice << std::endl;
        } else {
            tradePrice = askPrice;
            std::cout << "Exact match at price: " << tradePrice << std::endl;
        }

        u_int64_t askReduced = bestAsk.reduceVolume(tradeVolume);
        u_int64_t bidReduced = bestBid.reduceVolume(tradeVolume);

        Trade trade(bestBid, bestAsk, tradeVolume, tradePrice);

        // remove the filled orders from the orderBook.
        if(bestAsk.GetVolume() == 0){
            orderBook.removeAsk(bestAsk.getOrderId());
        }
        if(bestBid.GetVolume() == 0){
            orderBook.removeBid(bestBid.getOrderId());
        }
        
        std::cout << "Trade executed successfully!" << std::endl;
        return trade;
    
    }else{
        std::cout << " no trade available spread > 0" << std::endl;
    }

    return std::nullopt;
}

std::atomic<int> gOrderIdCounter = 0;

int generateUniqueOrderId(){
    
    return ++gOrderIdCounter; 
}

// int main(){
//     //testing atomic counter logic
    
//     std::mutex gLock;
//     std::lock_guard<std::mutex> gGuard(gLock);

//     OrderBook Book2(101);
//     Order ask1 = Order(Ask, 100, "apple", generateUniqueOrderId(), Price(960));
//     Order bid1 = Order(Bid, 100, "apple", generateUniqueOrderId(), Price(1041));
    
//     Order ask2 = Order(Ask, 100, "apple", generateUniqueOrderId(), Price(1000));
//     Order bid2 = Order(Bid, 100, "apple", generateUniqueOrderId(), Price(1000));
    
//     Order ask3 = Order(Ask, 100, "apple", generateUniqueOrderId(), Price(1035));
//     Order bid3 = Order(Bid, 100, "apple", generateUniqueOrderId(), Price(1035));
     
//     Book2.addAsk(ask1);
//     Book2.addBid(bid1);
    
//     Book2.addAsk(ask2);
//     Book2.addBid(bid2);
    
//     Book2.addAsk(ask3);
//     Book2.addBid(bid3);

//     // MatchingEngine APPL_matchingEngine(Book2);
//     MatchingEngine APPL_matchingEngine(Book2);
    
//     int index = 1;
//     while(1){
//         std::optional<Trade> testTrade = APPL_matchingEngine.matchLimitOrders();
//         std::cout << index << "\n";
//         ++index;
//     }
// }