#include "Order.hpp"
#include <string>
#include <iostream>
#include <array>


Order::Order(Type type, int volume, std::string name, int orderID, int priceLevel)
    : BuyOrSell(type), mVolume(volume), mName(name), mOrderID(orderID), mPriceLevel(priceLevel) {}

void Order::PrintOrder() const{

    std::cout << "Printing Order...\n";
    std::cout << "Type: " << (BuyOrSell == BUY ? "BUY" : "SELL") << "\n";
    std::cout << "Name: " << mName << "\n";
    std::cout << "Volume: " << mVolume << "\n";
    std::cout << "Order ID: " << mOrderID << "\n";
    std::cout << "Price Level: " << mPriceLevel << "\n";
}

void PrintArray(int arr[]){
    std::cout << sizeof(arr[0]); // this will print the size of the decayed pointer and not the array size!
}

int main(){
    
    std::string mystr = "very long string wow!";
    
    std::cout << mystr;
    return 0; 
}