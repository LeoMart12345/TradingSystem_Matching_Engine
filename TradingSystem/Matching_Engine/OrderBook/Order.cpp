#include "order.hpp"
#include "Price.hpp"

#include <string>
#include <iostream>
#include <array>
#include <algorithm>
#include <vector>
#include <sstream>


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

    std::string Order::serialize() const {
        std::string result;
        result += (BidOrAsk == Bid) ? "BUY" : "SELL";
        result += ",";
        result += std::to_string(mVolume);
        result += ",";
        result += mName;  // ticker symbol
        result += ",";
        result += std::to_string(mOrderID);
        result += ",";
        result += std::to_string(mPrice.mPriceValueInCent);  // cents
        return result;
    }

Order Order::deserialize(const std::string& data) {
    std::vector<std::string> parts;
    std::stringstream ss(data);
    std::string part;        
        
    while (std::getline(ss, part, ',')) {
        parts.push_back(part);
    }
        
    if (parts.size() != 5) {
        throw std::runtime_error("Invalid order data");
    }
        
    Side side = (parts[0] == "BUY") ? Bid : Ask;
    u_int64_t volume = std::stoull(parts[1]);
    std::string name = parts[2];
    u_int64_t orderId = std::stoull(parts[3]);
    u_int64_t priceVal = std::stoull(parts[4]);
        
    return Order(side, volume, name, orderId, Price(priceVal));
}

// Getters
int Order::GetVolume() const{return mVolume;}
Side Order::getBidOrAsk() const{return BidOrAsk;}
std::string Order::getName() const {return mName;}
u_int64_t Order::getOrderId() const {return mOrderID;}
Price Order::getPrice() const {return mPrice;}
// setters
