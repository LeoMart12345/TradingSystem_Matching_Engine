#pragma once
#include <string>
#include "Price.hpp"

enum Side {
    Bid,
    Ask 
};

class Order {
public:
    Side BidOrAsk;
    u_int64_t mVolume;
    std::string mName;
    u_int64_t mOrderID;
    Price mPrice;

    Order(Side Side, u_int64_t volume, std::string name, u_int64_t orderID, Price Price);

	void PrintOrder() const;
    //getters
    Side getBidOrAsk() const;
    int GetVolume() const;
    std::string getName()const;
    u_int64_t getOrderId() const;
    Price getPrice() const;

private:
	
};