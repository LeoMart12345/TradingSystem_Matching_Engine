#pragma once
#include "order.hpp"
#include <iostream>
#include <optional>

class Trade{

private:
    Order mBidOrder;
    Order mAskOrder;
    u_int64_t mTradeVolume;
    u_int64_t mTradePrice;

public:
    // TODO make a default constructor:
    Trade() = default;

    Trade(const Order& bidOrderParam, const Order& askOrderParam, u_int64_t tradeVolumeParam, u_int64_t priceParam)
    : mBidOrder(bidOrderParam)
    , mAskOrder(askOrderParam)
    , mTradeVolume(tradeVolumeParam)
    , mTradePrice(priceParam)
    {
    
    }

    void printTrade() const {
        std::cout << "Trade Details:" << std::endl;
        std::cout << "Ask Price: " << mAskOrder.getPrice().mPriceValueInCent << std::endl;
        std::cout << "Bid Price: " << mBidOrder.getPrice().mPriceValueInCent << std::endl;
        std::cout << "Volume: " << mTradeVolume << std::endl;
    }
    
};