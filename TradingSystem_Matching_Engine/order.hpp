#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <memory>
#include <map>

enum Type {
	BUY,
	SELL
};

class order {

public:
	Type BuyOrSell;
	int mVolume;
	std::string mName;
	int mOrderID;
	int mPriceLevel;


	order(Type type, int volume, std::string name, int orderID, int priceLevel)
		: BuyOrSell(type), mVolume(volume), mName(name), mOrderID(orderID), mPriceLevel(priceLevel) {
	}

private:

	// functions that need to be implemented
	// placeOrder(Order)
	// cancelOrder(OrderID) // this applies to when an order is not filled and you can cancel it then. cant cancel an order that has been filled
	// GetVolumeAtPriceLevel(PriceLevel, BuyOrSell)

};