#pragma once
#include <iostream>

enum class requestType {
    New,
    Cancel
};

struct OrderRequest {
    requestType type;
    u_int64_t clientId;
    u_int64_t orderId;
    char side;
    double price;
    u_int32_t quantity;
};