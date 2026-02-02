#include <iostream>
#include <array>
#include <vector>
#include <bitset>

#include "OrderBook.hpp"
#include "order.hpp"
#include <bit>

static constexpr int CHUNK = 64;
// this represents the number that will be used to determine the + & - 5% of the prices that can be bid and ask.
// This will need to be dynamic eventually to have a sliding window approach.
constexpr int StartOfPrice = 950; // aka stock price is 10 euro so this represents 5% below that price
constexpr int NumOfLevels = 101;  // allowing for +- 5% of 10

OrderBook::OrderBook(size_t size) 
    : mBidpriceLevel(size),
      mAskPriceLevel(size),
      mBidBitmap((size + CHUNK - 1) / CHUNK, 0ULL),
      mAskBitmap((size + CHUNK - 1) / CHUNK, 0ULL)
{
    std::cout << "OrderBook created with " << size << " price levels" << std::endl;
    std::cout << "BitMap create with " << mBidBitmap.size() << " 64 bit elements" << std::endl;
}

void OrderBook::addBid(Order order)
{
    // get the price of the bid
    int BidLevel = priceToIndex(order.mPrice);
    mBidpriceLevel[BidLevel].emplace_back(order);
    setBidBitTo1(order.getPrice());
}

void OrderBook::addAsk(Order order)
{
    int AskLevel = priceToIndex(order.mPrice);
    mAskPriceLevel[AskLevel].emplace_back(order);
    setAskBitTo1(order.getPrice());
}

void OrderBook::removeBid(u_int64_t orderId)
{
    int bestLevel = findBestAskLevel();
    if(bestLevel == -1) return;

    auto& deque = mBidpriceLevel[bestLevel];
    if (deque.empty()) return;

    if(deque.front().getOrderId() != orderId){
        std::__throw_logic_error("orderId does not match the order at the front of that queue level");
    }
    
    deque.pop_front();
    
    if (deque.empty()) {
        setBidBitTo0(indexToPrice(bestLevel));
    }
}


void OrderBook::removeAsk(u_int64_t orderId)
{

}

void OrderBook::getVolumeAtLevel(Price price)
{

}

void OrderBook::getTotalVolume()
{

}

// Helper functions
int OrderBook::priceToIndex(Price price) const
{
    int index = price.mPriceValueInCent - StartOfPrice;

    // need to do bounds checking here. // TODO
    if (index < 0 || index >= NumOfLevels)
    {
        std::cout << "ERROR that price is not a price that this asset can be traded at" << std::endl;
        throw std::invalid_argument("Invalid price to bid/ask");
    }
    return index;
}

Price OrderBook::indexToPrice(int levelIndex) const
{
    
    // need to do bounds checking here. // TODO
    if (levelIndex < 0 || levelIndex >= static_cast<int>(mBidpriceLevel.size())) {
        throw std::invalid_argument("Invalid level index: " + std::to_string(levelIndex));
    }

    int priceInCents = StartOfPrice + levelIndex;

    return Price(priceInCents);
}

std::pair<size_t, size_t> OrderBook::priceToBitmapIndex(Price price)
{
    // TODO imeplement

    int levelIndex = priceToIndex(price);

    return indexToBitmapIndex(levelIndex);
}

std::pair<size_t, size_t> OrderBook::indexToBitmapIndex(int levelIndex)
{
    // TODO implement.

    size_t wordIndex = levelIndex / CHUNK;
    size_t bitIndex = levelIndex % CHUNK;

    // Printing for testing
    std::cout << wordIndex << std::endl;

    std::cout << bitIndex << std::endl;

    return {wordIndex, bitIndex};
}

// Doesnt conditionally set the bit it always does, to avoid branches
void OrderBook::setBidBitTo1(const Price &price)
{
    auto result = priceToBitmapIndex(price);
    size_t wordPos = result.first; // the position of the word in the bitmap array
    size_t bitPos = result.second; // the position of the bit in that word

    // make a bitmask that has all 0s apart from the place where im checking for 1. 0001000000 etc
    // then OR the original word with this bitmask

    // std::cout << "64-bit: Current: " << std::bitset<64>(mBidBitmap[wordPos]) << std::endl;

    u_int64_t bitmask = (1ULL << bitPos);

    // // TODO take this out: Testing
    // std::cout << "64-bit: Current: " << std::bitset<64>(mBidBitmap[wordPos]) << std::endl;
    // std::cout << "64-bit Mask:     " << std::bitset<64>(bitmask) << std::endl;

    mBidBitmap[wordPos] |= bitmask;

    // std::cout << "64-bit: After  : " << std::bitset<64>(mBidBitmap[wordPos]) << std::endl;
}

void OrderBook::setBidBitTo0(const Price &price)
{
    std::cout << "entering setBidBitTo0" << std::endl;
    auto result = priceToBitmapIndex(price);
    size_t wordPos = result.first; // the position of the word in the bitmap array
    size_t bitPos = result.second; // the position of the bit in that word

    // Before resetting TODO remove these
    // std::cout << "64-bit: Current: " << std::bitset<64>(mBidBitmap[wordPos]) << std::endl;

    mBidBitmap[wordPos] &= ~(1ULL << bitPos);    

    // After resetting
    // std::cout << "64-bit: After  : " << std::bitset<64>(mBidBitmap[wordPos]) << std::endl;
}





void OrderBook::setAskBitTo1(const Price &price)
{
    auto result = priceToBitmapIndex(price);
    size_t wordPos = result.first; // the position of the word in the bitmap array
    size_t bitPos = result.second; // the position of the bit in that word

    // make a bitmask that has all 0s apart from the place where im checking for 1. 0001000000 etc
    // then OR the original word with this bitmask

    // std::cout << "64-bit: Current: " << std::bitset<64>(mBidBitmap[wordPos]) << std::endl;

    u_int64_t bitmask = (1ULL << bitPos);

    // // TODO take this out: Testing
    // std::cout << "64-bit: Current: " << std::bitset<64>(mAskBitmap[wordPos]) << std::endl;
    // std::cout << "64-bit Mask:     " << std::bitset<64>(bitmask) << std::endl;

    mAskBitmap[wordPos] |= bitmask;

    // std::cout << "64-bit: After  : " << std::bitset<64>(mAskBitmap[wordPos]) << std::endl;
}

// Makes bitmask and bitwise AND to set bit in bitmap to 0
void OrderBook::setAskBitTo0(const Price &price)
{
    std::cout << "entering setAskBitTo0" << std::endl;
    auto result = priceToBitmapIndex(price);
    size_t wordPos = result.first; // the position of the word in the bitmap array
    size_t bitPos = result.second; // the position of the bit in that word

    // Before resetting TODO remove these
    // std::cout << "64-bit: Current: " << std::bitset<64>(mAskBitmap[wordPos]) << std::endl;

    mAskBitmap[wordPos] &= ~(1ULL << bitPos);    

    // After resetting
    // std::cout << "64-bit: After  : " << std::bitset<64>(mAskBitmap[wordPos]) << std::endl;
}

Order& OrderBook::getBestBid(){
    int bestBidLevel = findBestBidLevel();

    if(bestBidLevel == -1){
        throw std::runtime_error("No bids in the OrderBook!");
    }
    // deque at that level
    auto& deque = mBidpriceLevel[bestBidLevel];

    if(deque.empty()){
        std::runtime_error("bitmapp shows the level marked but the deque is empty");
    }

    return deque.front();
}

Order& OrderBook::getBestAsk(){
    int bestAskLevel = findBestAskLevel();

    if(bestAskLevel == -1){
        throw std::runtime_error("No Asks in the OrderBook");
    }
    // use ref
    auto& deque = mAskPriceLevel[bestAskLevel];

    if(deque.empty()){
        std::runtime_error("bitmapp shows the level marked but the deque is empty");
    }

    return deque.front();
}

Order OrderBook::popBestBid(){
    int bestBidLevel = findBestBidLevel();

    if(bestBidLevel == -1){
        throw std::runtime_error("No bids in the OrderBook!");
    }
    // deque at that level
    auto& deque = mBidpriceLevel[bestBidLevel];

    if(deque.empty()){
        std::runtime_error("bitmapp shows the level marked but the deque at that level is empty!");
    }

    Order orderCopy = deque.front();
    deque.pop_front();

    if (deque.empty()) {
        setBidBitTo0(indexToPrice(bestBidLevel));
    }

    return orderCopy;
}

Order OrderBook::popBestAsk(){
    int bestAskLevel = findBestAskLevel();

    if(bestAskLevel == -1){
        throw std::runtime_error("no asks in the OrderBook!");
    }
    auto& deque = mAskPriceLevel[bestAskLevel];
    if(deque.empty()){
        std::runtime_error("bitmap shows the level marked but the deque at that level is empty!");
    }
    Order orderCopy = deque.front();
    deque.pop_front();

    if(deque.empty()){
        setAskBitTo0(indexToPrice(bestAskLevel));
    }

    return orderCopy;
}

// void OrderBook::fillBestBid(u_int16_t quantity){

// }


int OrderBook::findBestBidLevel() const{
    // 
    for(int i = static_cast<int>(mBidBitmap.size() - 1); i >= 0; --i){
        u_int64_t word = mBidBitmap[i];

        if(word != 0){
            int leadingZeros = std::__countl_zero(word);
            
            int bitPositionFromRight = 63 - leadingZeros;

            std::cout << "word index: " << i << std::endl;
            std::cout << "bit Index from LSB: " << bitPositionFromRight << std::endl;
            std::cout << "gloval bit position: " << i * CHUNK  + bitPositionFromRight << std::endl;
            
            return i * CHUNK  + bitPositionFromRight; 
        }
    }
    return -1;
}

int OrderBook::findBestAskLevel() const{
    // TODO double check this logic to make sure its correct.
    for(int i = 0; i < static_cast<int>(mAskBitmap.size()); ++i){

        u_int64_t word = mAskBitmap[i];

        // use trailing as opposed to leading zero

        if(word != 0){
            
            int trailingZeros = std::__countr_zero(word);
            // should the bit position not be from the left?

            std::cout << "word index: " << i << std::endl;
            std::cout << "bit Index from LSB: " << trailingZeros << std::endl;
            std::cout << "gloval bit position: " << i * CHUNK  + trailingZeros << std::endl;
            
            return i * CHUNK  + trailingZeros; 
        }
    }
    return -1;
}

void OrderBook::printOrderBook() const
{
    std::cout << "--------  OrderBook  ---------------------------------------------" << std::endl;
    std::cout << "\nBIDS (high -> low)\n";
    for (int i = static_cast<int>(mBidpriceLevel.size()) - 1; i >= 0; --i)
    {
        if (mBidpriceLevel[i].empty())
            continue;

        int priceTicks = StartOfPrice + i;
        float TicksToPrice = static_cast<float>(priceTicks / 100.0f);

        std::cout << "Level " << i << " (Price €" << TicksToPrice<< "): ";

        for (const auto &o : mBidpriceLevel[i])
        {
            std::cout << "[id=" << o.mOrderID
                      << ", qty=" << o.mVolume
                      << ", user=" << o.mName
                      << "] ";
        }
        std::cout << "\n";
    }

    std::cout << "\nASKS (low -> high)\n";
    for (int i = 0; i < static_cast<int>(mAskPriceLevel.size()); ++i)
    {
        if (mAskPriceLevel[i].empty())
            continue;

        int priceTicks = StartOfPrice + i;
        std::cout << "Level " << i << " (ticks " << priceTicks << "): ";

        for (const auto &o : mAskPriceLevel[i])
        {
            std::cout << "[id=" << o.mOrderID
                      << ", qty=" << o.mVolume
                      << ", Asset=" << o.mName
                      << "] ";
        }
        std::cout << "\n";
    }
    std::cout << "------------------------------------------------------------------" << std::endl;
}