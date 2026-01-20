#include <iostream>
#include <array>
#include <vector>
#include "Price.hpp"
#include "order.hpp"

class OrderBook{

private:
    // each of these holds 64 bits each bit represents a level, if bit = 1 the level is active, if bit is 0 level is inactive
    std::vector<u_int64_t> mBidBitmap;
    std::vector<u_int64_t> mAskBitmap;

    // each of these levels will hold a queue of orders
    std::vector<int> mBidpriceLevel;
    std::vector<int> mAskPriceLevel;

public:
    OrderBook(size_t size);
    
    // APi for the OrderBook
    void addBid(Order order);
    void removeBid(Order Order);
    void getVolumeAtLevel(Price price);
    void getTotalVolume();


    //writing to the bitmap
    

};

int main()
{
    std::cout << "hello" << std::endl;
    OrderBook testing(1000);
}