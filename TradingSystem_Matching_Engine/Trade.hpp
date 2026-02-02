#pragma once
#include "order.hpp"
#include <iostream>
#include <optional>

class Trade{

private:
    Order mBidOrder;
    Order mAskOrder;
    int mTradeVolume;

public:
    // TODO make a default constructor:
    Trade() = default;

    Trade(const Order& bidOrderParam, const Order& askOrderParam, int tradeVolumeParam)
    : mBidOrder(bidOrderParam)
    , mAskOrder(askOrderParam)
    , mTradeVolume(tradeVolumeParam)
    {
    
    }

    void printTrade() const {
        std::cout << "Trade Details:" << std::endl;
        std::cout << "Ask Price: " << mAskOrder.getPrice().mPriceValueInCent << std::endl;
        std::cout << "Bid Price: " << mBidOrder.getPrice().mPriceValueInCent << std::endl;
        std::cout << "Volume: " << mTradeVolume << std::endl;
    }

};