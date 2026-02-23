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

    // default constructor for init the Order Pool
    Order() : BidOrAsk(Bid), mVolume(0), mName(""), mOrderID(0), mPrice(Price(0)) {}
    Order(Side Side, u_int64_t volume, std::string name, u_int64_t orderID, Price Price);

	void PrintOrder() const;
    //getters
    Side getBidOrAsk() const;
    int GetVolume() const;
    std::string getName()const;
    u_int64_t getOrderId() const;
    Price getPrice() const;
    
    u_int64_t reduceVolume(u_int64_t volume);

    std::string serialize() const;
    static Order deserialize(const std::string& data);

    
private:
	
};