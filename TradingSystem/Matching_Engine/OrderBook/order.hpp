#pragma once
#include <algorithm>
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

    // intrusive links used by the order book's price level queues
    Order *mPrev = nullptr;
    Order *mNext = nullptr;

    // default constructor for init the Order Pool
    Order() : BidOrAsk(Bid), mVolume(0), mName(""), mOrderID(0), mPrice(Price(0)) {}
    Order(Side Side, u_int64_t volume, std::string name, u_int64_t orderID, Price Price);

	void PrintOrder() const;
    //getters
    Side getBidOrAsk() const { return BidOrAsk; }
    int GetVolume() const { return static_cast<int>(mVolume); }
    std::string getName()const;
    u_int64_t getOrderId() const { return mOrderID; }
    Price getPrice() const { return mPrice; }

    // takes at most volumeReduction away and reports how much was taken
    u_int64_t reduceVolume(u_int64_t volumeReduction) {
        if (mVolume == 0) return 0;

        // cant take away more than there is in the volume.
        const u_int64_t feasableReduction = std::min(mVolume, volumeReduction);
        mVolume -= feasableReduction;

        return feasableReduction;
    }

    std::string serialize() const;
    static Order deserialize(const std::string& data);

    
private:
	
};