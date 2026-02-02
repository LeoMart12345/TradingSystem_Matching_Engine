#pragma once
#include <iostream>
#include "order.hpp"

class Trade{

public:

private:
    Order mBidOrder;
    Order mAskOrder;
    // int mAskOrderId;
    // int mBidOrderId;
    int mTradeVolume;

public:
    Trade(Order& bidOrderParam, Order& askOrderParam, int tradeVolumeParam)
    : mBidOrder(bidOrderParam)
    , mAskOrder(askOrderParam)
    , mTradeVolume(tradeVolumeParam)
    {
    
    }

};