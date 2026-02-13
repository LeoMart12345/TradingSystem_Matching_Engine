#pragma once
#include <iostream>
#include <cstdint>
#include <cstring>
#include <sstream>
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

    std::string serialize() const {
        std::string result;
        result += std::to_string(clientId);
        result += ",";
        result += (type == requestType::New) ? "NEW" : "CANCEL";
        result += ",";
        result += requestOrder.serialize();
        return result;
    }

    static OrderRequest deserialize(const std::string& data) {
        std::vector<std::string> parts;
        std::stringstream ss(data);
        std::string part;
        
        while (std::getline(ss, part, ',')) {
            parts.push_back(part);
        }
        
        // First two parts are clientId and type
        // Remaining 5 parts are the Order
        if (parts.size() != 7) {  // 2 + 5 = 7
            throw std::runtime_error("Invalid order request data");
        }
        
        uint64_t clientId = std::stoull(parts[0]);
        requestType type = (parts[1] == "NEW") ? requestType::New : requestType::Cancel;
        
        // Reconstruct the Order string (5 fields)
        std::string orderData = parts[2] + "," + parts[3] + "," + 
                               parts[4] + "," + parts[5] + "," + parts[6];
        Order order = Order::deserialize(orderData);
        
        return OrderRequest(clientId, type, order);
    }

};