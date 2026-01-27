#pragma once

#include <format>
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
    
    // Bid
    void addBid(Order order);
    void removeBid(Order order);
    void setBidBitTo1(const Price& price); // takes in the price, uses helpers to set bitmap bit to 1 to indicate active price level.
    void setBidBitTo0(const Price& price);
    
    // updated API that takes into account quantity and the posibility of a partial fill
    const Order& getBestBid() const;
    Order popBestBid();
    // void fillBestBid(u_int16_t quantity); // todo decide if this needs to be matching engine logic

    const Order& getBestAsk() const;
    Order popBestAsk();
    
    // void fillBestAsk(u_int16_t quantity);
    // Ask
    void addAsk(Order order);
    void removeAsk(Order order);
    void setAskBitTo1(const Price& price);
    void setAskBitTo0(const Price& price);

    void getVolumeAtLevel(Price price);
    void getTotalVolume();
    
    //helper functions
    int priceToIndex(Price price) const;
    Price indexToPrice(int levelIndex) const;
    
    std::pair<size_t, size_t> priceToBitmapIndex(Price price);
    std::pair<size_t, size_t> indexToBitmapIndex(int priceIndex);

    int findBestBidLevel() const; //returns -1 if there are no bids
    int findBestAskLevel() const; // same for the asks

    void printOrderBook() const;

    
};