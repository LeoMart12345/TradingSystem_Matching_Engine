#pragma once
#include <string>

enum Type {
    BUY,
    SELL
};

class Order {
public:
    Type BuyOrSell;
    int mVolume;
    std::string mName;
    int mOrderID;
    int mPriceLevel;

    Order(Type type, int volume, std::string name, int orderID, int priceLevel);

	void PrintOrder() const;


private:
    // Future functions:
    // void placeOrder(const Order& order);
    // void cancelOrder(int orderID);
    // int getVolumeAtPriceLevel(int priceLevel, Type side) const;


	//just used to print the order for now 
	

	
};