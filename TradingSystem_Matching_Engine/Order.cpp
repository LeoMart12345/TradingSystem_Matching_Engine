#include "order.hpp"
#include <string>
#include <iostream>
#include <array>
#include "Price.hpp"

Order::Order(Side side, u_int64_t volume, std::string name, u_int64_t orderID, Price priceLevel)
    : BidOrAsk(side), mVolume(volume), mName(name), mOrderID(orderID), mPrice(priceLevel) {}

void Order::PrintOrder() const{

    std::cout << "Printing Order...\n";
    std::cout << "Side: " << (BidOrAsk == Bid ? "Bid" : "Ask") << "\n";
    std::cout << "Name: " << mName << "\n";
    std::cout << "Volume: " << mVolume << "\n";
    std::cout << "Order ID: " << mOrderID << "\n";
    std::cout << "Price Level: " << mPrice.mPriceValueInCent << "\n";
}
// Getters
int Order::GetVolume() const{return mVolume;}
Side Order::getBidOrAsk() const{return BidOrAsk;}
std::string Order::getName() const {return mName;}
u_int64_t Order::getOrderId() const {return mOrderID;}
Price Order::getPrice() const {return mPrice;}

int main(){
 
    Order myOrder = Order(Bid, 100, "apple", 100, 10);
    
    myOrder.PrintOrder();

    return 0; 
}