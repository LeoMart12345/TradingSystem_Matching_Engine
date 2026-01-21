#pragma once
#include <iostream>
#include <array>
#include <vector>
#include "Price.hpp"
#include "order.hpp"
#include <deque>

class OrderBook{

private:
    // each of these holds 64 bits each bit represents a level, if bit = 1 the level is active, if bit is 0 level is inactive
    std::vector<u_int64_t> mBidBitmap;
    std::vector<u_int64_t> mAskBitmap;

    // each of these levels will hold a queue of orders
    std::vector<std::deque<Order>> mBidpriceLevel;
    std::vector<std::deque<Order>> mAskPriceLevel;

public:
    explicit OrderBook(size_t size);
    
    // APi for the OrderBook
    void addBid(Order order);
    void removeBid(Order Order);
    void getVolumeAtLevel(Price price);
    void getTotalVolume();

    // Bitmap getters
    void getBestBid();
    void getBestAsk();

    //helper functions
    int priceToIndex(Price price) const;
    

    
    // int indexToBit();

    std::pair<size_t, size_t> priceToBitmapIndex(Price price);
    std::pair<size_t, size_t> indexToBitmapIndex(int priceIndex);

    void printOrderBook() const;

};