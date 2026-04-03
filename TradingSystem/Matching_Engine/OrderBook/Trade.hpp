#pragma once
#include "order.hpp"
#include <iostream>
#include <string>
#include <sys/types.h>

class Trade {

private:
  Order *mBidOrder;
  Order *mAskOrder;
  u_int64_t mTradeVolume;
  u_int64_t mTradePrice;

public:
  // TODO make a default constructor:
  Trade() = default;

  Trade(Order *bidOrderParam, Order *askOrderParam, u_int64_t tradeVolumeParam,
        u_int64_t priceParam)
      : mBidOrder(bidOrderParam), mAskOrder(askOrderParam),
        mTradeVolume(tradeVolumeParam), mTradePrice(priceParam) {}

  void printTrade() const {
    std::cout << "Trade Details:" << std::endl;
    std::cout << "Ask Price: " << mAskOrder->getPrice().mPriceValueInCent
              << std::endl;
    std::cout << "Bid Price: " << mBidOrder->getPrice().mPriceValueInCent
              << std::endl;
    std::cout << "Volume: " << mTradeVolume << std::endl;
  }
  // For sending data to the dashboard.py
  u_int64_t getPrice() const { return mTradePrice; }
  u_int64_t getVolume() const { return mTradeVolume; }

  std::string serialise() const {
    return std::to_string(mTradePrice) + ":" + std::to_string(mTradeVolume);
  }
};
