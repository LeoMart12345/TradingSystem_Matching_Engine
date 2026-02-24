#pragma once

#include <format>
#include <iostream>
#include <array>
#include <vector>
#include "Price.hpp"
#include "order.hpp"
#include <deque>
#include <random>
#include <map>
#include <unordered_map>
#include <mutex>
#include "./ObjectPool.hpp"

class OrderBook{

private:
    ObjectPool<Order, 100000> mOrderPool; // pool of objects that will be acquired and released to as opposed to a syscall.

    // each of these holds 64 bits each bit represents a level, if bit = 1 the level is active, if bit is 0 level is inactive
    std::vector<u_int64_t> mBidBitmap;
    std::vector<u_int64_t> mAskBitmap;

    // These levels hold a deque of orders for both sides.
    std::vector<std::deque<Order*>> mBidpriceLevel;
    std::vector<std::deque<Order*>> mAskPriceLevel;
    
    // Hashmap for keeping track of orderId (useful for cancelling orders by orderId)
    struct OrderLocation{
        Price price;
        Side side;
    };
    // maps a unique orderID to a location
    std::unordered_map<u_int64_t, OrderLocation> orderIdtoPriceMapping;

    // Mutex for locking the orderbook.
    std::mutex mtx;

public:
    explicit OrderBook(size_t size);
    
    // Functions to benchmark the orderBook
    
    Order generateRandomOrder();

    // Bid
    void addBid(Order* order);
    void removeBid(u_int64_t orderID);
    void setBidBitTo1(const Price& price); // takes in the price, uses helpers to set bitmap bit to 1 to indicate active price level.
    void setBidBitTo0(const Price& price);
    
    // updated API that takes into account quantity and the posibility of a partial fill
    Order* getBestBid();
    //todo peek best bid
    Order popBestBid();
    
    // void fillBestBid(u_int16_t quantity); // todo decide if this needs to be matching engine logic
    Order* getBestAsk();
    //todo peek best ask
    Order popBestAsk();
    
    ObjectPool<Order, 100000>& getOrderPool() { return mOrderPool; }
    
    // void fillBestAsk(u_int16_t quantity);
    // Ask
    void addAsk(Order* order);
    void removeAsk(u_int64_t orderId);
    void setAskBitTo1(const Price& price);
    void setAskBitTo0(const Price& price);

    void getVolumeAtLevel(const Price& price);
    void getTotalVolume();
    
    //helper functions
    int priceToIndex(Price price) const;
    Price indexToPrice(int levelIndex) const;
    std::pair<Price, Side> orderIdToPrice(const u_int64_t orderId)const;
    void removeOrderFromOrderId(u_int64_t orderId);
    std::pair<size_t, size_t> priceToBitmapIndex(Price price);
    std::pair<size_t, size_t> indexToBitmapIndex(int priceIndex);

    int findBestBidLevel() const; //returns -1 if there are no bids
    int findBestAskLevel() const; // same for the asks

    void printOrderBook() const;    
};