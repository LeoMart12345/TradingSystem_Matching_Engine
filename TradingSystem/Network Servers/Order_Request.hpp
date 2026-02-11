#pragma once
#include <iostream>
#include <cstdint>
#include <cstring>
#include "../Matching_Engine/OrderBook/order.hpp"

enum class requestType {
    New,
    Cancel
};

struct OrderRequest {
    Order requestOrder;
    
    u_int64_t clientId;
    requestType type;
    
  
    OrderRequest(u_int64_t clientId, requestType type, const Order& order)
        : clientId(clientId), type(type), requestOrder(order) 
        {

        }

};