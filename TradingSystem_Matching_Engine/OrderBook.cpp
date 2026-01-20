#include <iostream> 
#include <array> 
#include <vector>
#include "OrderBook.hpp"

static constexpr int CHUNK = 64; 
// this represents the number that will be used to determine the + & - 5% of the prices that can be bid and ask.
// This will need to be dynamic eventually to have a sliding window approach. 
constexpr int StartOfPrice = 950; //aka stock price is 10 euro so this represents 5% below that price 
constexpr int NumOfLevels = 101; // allowing for +- 5% of 10

OrderBook::OrderBook(size_t size):
mBidpriceLevel(size), mAskPriceLevel(size)
{
    // Initializing bitmaps based on the size
    size_t mBitmapSize = (size + CHUNK - 1) / CHUNK; // add 63 to allow size to always be divisable

    mBidBitmap.resize(mBitmapSize, 0ULL); 
    mAskBitmap.resize(mBitmapSize, 0ULL);

    std::cout << "OrderBook created with" << size << "price levels" << std::endl;
    std::cout << "BitMap create with" << mBitmapSize << " 64 bit elements" << std::endl;

}

void OrderBook::addBid(Order order){
    // get the price of the bid

}

void OrderBook::removeBid(Order Order) {
    // get the price of the bid

}

void OrderBook::getVolumeAtLevel(Price price){

}

void OrderBook::getTotalVolume(){

}


// helper
int OrderBook::priceToIndex(Price price){

}