#pragma once
#include "./OrderBook/order.hpp"
#include "./OrderBook/OrderBook.hpp"
#include "./OrderBook/Trade.hpp"
#include <../Network Servers/Order_Request.hpp>

#include <map>
#include <unordered_map>
#include <deque>
#include <queue>
#include <atomic>

class MatchingEngine {
    
    private:
        OrderBook orderBook;
        std::atomic<u_int64_t> nextOrderId{1000};

    public:
        explicit MatchingEngine(OrderBook& orderBook);
        
        std::optional<Trade> matchLimitOrders();
        u_int64_t addOrder(Order order);
        void processOrder(OrderRequest orderRequest);
};