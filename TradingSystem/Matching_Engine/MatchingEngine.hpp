#pragma once
#include "./OrderBook/order.hpp"
#include "./OrderBook/OrderBook.hpp"
#include "./OrderBook/Trade.hpp"

#include <map>
#include <unordered_map>
#include <deque>
#include <queue>


class MatchingEngine {
    
    private:
        OrderBook orderBook;

    public:
        explicit MatchingEngine(OrderBook& orderBook);
        
        std::optional<Trade> matchLimitOrders();
        bool addBid(const Order& order);
};