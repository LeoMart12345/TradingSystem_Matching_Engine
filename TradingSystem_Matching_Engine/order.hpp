#pragma once
#include <string>
#include "Price.hpp"

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
    Price mPrice;

    Order(Type type, int volume, std::string name, int orderID, Price Price);

	void PrintOrder() const;
    //getters
    Type GetBuyOrSell() const;
    int GetVolume() const;
    std::string getName()const;
    int getOrderId() const;
    Price getPrice() const;

private:
	
};