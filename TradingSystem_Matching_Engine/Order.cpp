#include "order.hpp"
#include "Price.hpp"

#include <string>
#include <iostream>
#include <array>
#include <algorithm>


Order::Order(Side side, u_int64_t volume, std::string name, u_int64_t orderID, Price priceLevel)
    : BidOrAsk(side), mVolume(volume), mName(name), mOrderID(orderID), mPrice(priceLevel) {}

void Order::PrintOrder() const{

    std::cout << "-------------------------------------" << std::endl;
    std::cout << "Printing Order\n";
    std::cout << "Side: " << (BidOrAsk == Bid ? "Bid" : "Ask") << "\n";
    std::cout << "Name: " << mName << "\n";
    std::cout << "Volume: " << mVolume << "\n";
    std::cout << "Order ID: " << mOrderID << "\n";
    std::cout << "Price Level: " << mPrice.mPriceValueInCent << "\n";
    std::cout << "-------------------------------------" << std::endl;
}
//returns the new volume that
u_int64_t Order::reduceVolume(u_int64_t volumeReduction){
    if(mVolume == 0) return 0;

    // cant take away more than there is in the volume.
    int feasableReduction = std::min(mVolume, volumeReduction);
    mVolume -= feasableReduction;

    return feasableReduction;
}

// Getters
int Order::GetVolume() const{return mVolume;}
Side Order::getBidOrAsk() const{return BidOrAsk;}
std::string Order::getName() const {return mName;}
u_int64_t Order::getOrderId() const {return mOrderID;}
Price Order::getPrice() const {return mPrice;}
// setters
