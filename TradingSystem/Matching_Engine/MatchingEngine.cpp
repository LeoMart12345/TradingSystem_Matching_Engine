// #include "MatchingEngine.hpp"

#include "./Matching_Engine/MatchingEngine.hpp"

#include "Trade.hpp"
#include <optional>
#include <thread>
#include <mutex>
#include <sstream>
#include "../Debug/Debug.hpp"
#include "../UtilityClass/OrderIdGenerator.hpp"

// Constructor
MatchingEngine::MatchingEngine(OrderBook& orderBook)
    : orderBook(orderBook)
{
    DEBUG_PRINT("matchingEngine was constructed!");
}

u_int64_t MatchingEngine::addOrder(Order* order){
    std::lock_guard<std::mutex> lock(mtx);
    order->mOrderID = OrderIdGenerator::incrementOrder();
    
    DEBUG_PRINT("Server Assigned Order ID " << order->mOrderID);
    //need backslash for multi-line debug
    DEBUG_PRINT("SERVER: Order details - " \
    << (order->BidOrAsk == Side::Bid ? "BUY" : "SELL") \
    << " " << order->mVolume \
    << " " << order->mName \
    << " @ " << order->mPrice.getPriceInTicks());


    if(order->BidOrAsk == Side::Bid){
        orderBook.addBid(order);
    }else{
        orderBook.addAsk(order);
    }

    return order->mOrderID;
}

std::optional<Trade> MatchingEngine::matchLimitOrders(){ 
    std::lock_guard<std::mutex> lock(mtx);
    DEBUG_PRINT("Starting Matching Orders Cycle");
    // get bestask/bid can fail so need try
    try {
        Order* bestAsk = orderBook.getBestAsk();
        Order* bestBid = orderBook.getBestBid();
        
        u_int64_t bidPrice = bestBid->getPrice().getPriceInTicks();
        u_int64_t askPrice = bestAsk->getPrice().getPriceInTicks();

        if(bidPrice >= askPrice){
            DEBUG_PRINT("Match found! Bid(" << bidPrice << ") >= Ask(" << askPrice << ")");

            u_int64_t tradeVolume = std::min(bestAsk->GetVolume(), bestBid->GetVolume());
            u_int64_t tradePrice = askPrice;

            bestAsk->reduceVolume(tradeVolume);
            bestBid->reduceVolume(tradeVolume);

            Trade trade(bestBid, bestAsk, tradeVolume, tradePrice);
            
            if(bestAsk->GetVolume() == 0){
                orderBook.removeAsk(bestAsk->getOrderId());
            }
            if(bestBid->GetVolume() == 0){
                orderBook.removeBid(bestBid->getOrderId());
            }
            DEBUG_PRINT("Trade executed successfully!");
            return trade;
        } else {
            DEBUG_PRINT("no trade available spread > 0");
        }
    } catch(const std::runtime_error& e){
        return std::nullopt;
    }

    return std::nullopt;
}