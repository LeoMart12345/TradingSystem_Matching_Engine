#include "order.hpp"
#include <string>
#include <iostream>
#include <array>
#include "Price.hpp"

Order::Order(Type type, int volume, std::string name, int orderID, Price priceLevel)
    : BuyOrSell(type), mVolume(volume), mName(name), mOrderID(orderID), mPrice(priceLevel) {}

void Order::PrintOrder() const{

    std::cout << "Printing Order...\n";
    std::cout << "Type: " << (BuyOrSell == BUY ? "BUY" : "SELL") << "\n";
    std::cout << "Name: " << mName << "\n";
    std::cout << "Volume: " << mVolume << "\n";
    std::cout << "Order ID: " << mOrderID << "\n";
    std::cout << "Price Level: " << mPrice.mPriceValueInCent << "\n";
}
// Getters
int Order::GetVolume() const{return mVolume;}
Type Order::GetBuyOrSell() const{return BuyOrSell;}

std::string Order::getName() const {return mName;}
int Order::getOrderId() const {return mOrderID;}
Price Order::getPrice() const {return mPrice;}

int main(){
 
    Order myOrder = Order(BUY, 100, "apple", 100, 10);
    
    myOrder.PrintOrder();

    return 0; 
}