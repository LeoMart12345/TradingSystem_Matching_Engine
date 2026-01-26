#include <iostream>
#include <array>
#include <vector>
#include <bitset>
//
#include "OrderBook.hpp"
#include "order.hpp"
#include <bit>

static constexpr int CHUNK = 64;
// this represents the number that will be used to determine the + & - 5% of the prices that can be bid and ask.
// This will need to be dynamic eventually to have a sliding window approach.
constexpr int StartOfPrice = 950; // aka stock price is 10 euro so this represents 5% below that price
constexpr int NumOfLevels = 101;  // allowing for +- 5% of 10

OrderBook::OrderBook(size_t size) : mBidpriceLevel(size), mAskPriceLevel(size)
{
    // Initializing bitmaps based on the size
    size_t mBitmapSize = (size + CHUNK - 1) / CHUNK; // add 63 to allow size to always be divisable

    mBidBitmap.resize(mBitmapSize, 0ULL);
    mAskBitmap.resize(mBitmapSize, 0ULL);

    std::cout << "OrderBook created with " << size << " price levels" << std::endl;
    std::cout << "BitMap create with " << mBitmapSize << " 64 bit elements" << std::endl;
}

void OrderBook::addBid(Order order)
{
    // get the price of the bid
    int BidLevel = priceToIndex(order.mPrice);
    mBidpriceLevel[BidLevel].emplace_back(order);
    setBidBitTo1(order.getPrice());
}

void OrderBook::removeBid(Order order)
{
    // get the price of the bid
    int bidLevel = priceToIndex(order.mPrice);
    // check to avoid UB
    if(mBidpriceLevel[bidLevel].empty() == false){
        const Order& frontOrder = mBidpriceLevel[bidLevel].front();
        
        // check if the order is fully filled
        // if(frontOrder.getPrice().getPriceInTicks() == 0){
            mBidpriceLevel[bidLevel].pop_front();
        // }
    }

    // this shouldent be called every time just when the level is completely empty.
    if(mBidpriceLevel[bidLevel].empty()){
        setBidBitTo0(order.getPrice());
    }
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

    // TODO take this out: Testing
    std::cout << "64-bit: Current: " << std::bitset<64>(mBidBitmap[wordPos]) << std::endl;
    std::cout << "64-bit Mask:     " << std::bitset<64>(bitmask) << std::endl;

    mBidBitmap[wordPos] |= bitmask;

    std::cout << "64-bit: After  : " << std::bitset<64>(mBidBitmap[wordPos]) << std::endl;
}

void OrderBook::setBidBitTo0(const Price &price)
{
    std::cout << "entering setBidBitTo0" << std::endl;
    auto result = priceToBitmapIndex(price);
    size_t wordPos = result.first; // the position of the word in the bitmap array
    size_t bitPos = result.second; // the position of the bit in that word

    // Before resetting
    std::cout << "64-bit: Current: " << std::bitset<64>(mBidBitmap[wordPos]) << std::endl;

    mBidBitmap[wordPos] &= ~(1ULL << bitPos);    

    // After resetting
    std::cout << "64-bit: After  : " << std::bitset<64>(mBidBitmap[wordPos]) << std::endl;
}

void OrderBook::addAsk(Order order)
{

}

void OrderBook::removeAsk(Order order)
{

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

    // TODO take this out: Testing
    std::cout << "64-bit: Current: " << std::bitset<64>(mAskBitmap[wordPos]) << std::endl;
    std::cout << "64-bit Mask:     " << std::bitset<64>(bitmask) << std::endl;

    mAskBitmap[wordPos] |= bitmask;

    std::cout << "64-bit: After  : " << std::bitset<64>(mAskBitmap[wordPos]) << std::endl;
}

// Makes bitmask and bitwise AND to set bit in bitmap to 0
void OrderBook::setAskBitTo0(const Price &price)
{
    std::cout << "entering setAskBitTo0" << std::endl;
    auto result = priceToBitmapIndex(price);
    size_t wordPos = result.first; // the position of the word in the bitmap array
    size_t bitPos = result.second; // the position of the bit in that word

    // Before resetting
    std::cout << "64-bit: Current: " << std::bitset<64>(mAskBitmap[wordPos]) << std::endl;

    mAskBitmap[wordPos] &= ~(1ULL << bitPos);    

    // After resetting
    std::cout << "64-bit: After  : " << std::bitset<64>(mAskBitmap[wordPos]) << std::endl;
}

const Order& OrderBook::getBestBid() const{
    int bestBidLevel = findBestBidLevel();

    if(bestBidLevel != -1){
        throw std::runtime_error("No bids in the OrderBook!");
    }
    // deque at that level
    const auto deque = mBidpriceLevel[bestBidLevel];

    if(deque.empty()){
        std::runtime_error("bitmapp shows the level marked but the deque is empty");
    }
    // int word = static_cast<int>(bestBidLevel / 64); 
    // int bit = bestBidLevel % 64;

    return deque.front();
}

Order OrderBook::popBestBid(){
    int bestBidLevel = findBestBidLevel();

    if(bestBidLevel != -1){
        throw std::runtime_error("No bids in the OrderBook!");
    }
    // deque at that level
    auto& deque = mBidpriceLevel[bestBidLevel];

    if(deque.empty()){
        std::runtime_error("bitmapp shows the level marked but the deque is empty");
    }

    Order orderCopy = deque.front();

    deque.pop_front();

    if (deque.empty()) {
        //TODO set the bitmap to 0

        setBidBitTo0(index)
    }

}

void OrderBook::fillBestBid(u_int16_t quantity){

}


int OrderBook::findBestBidLevel() const{

    // 
    for(int i = static_cast<int>(mBidBitmap.size() - 1); i >= 0; --i){
        u_int64_t word = mBidBitmap[i];

        if(word != 0){
            int leadingZeros = std::__countl_zero(word);
            
            int bitPositionFromRight = 63 - leadingZeros;
            // should the bit position not be from the left?

            std::cout << "word index: " << i << std::endl;
            std::cout << "bit Index from LSB: " << bitPositionFromRight << std::endl;
            std::cout << "gloval bit position: " << i * CHUNK  + bitPositionFromRight << std::endl;
            
            return i * CHUNK  + bitPositionFromRight; 
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
        std::cout << "Level " << i << " (Price €" << priceTicks / 100 << "): ";

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

// testing the API
int main()
{
    OrderBook Book1(NumOfLevels);

    // make a price then make the order or make the
    Order myOrder = Order(Bid, 100, "apple", 100, 1050);

    Book1.addBid(myOrder);

    Book1.printOrderBook();

    // Book1.removeBid(myOrder);

    // Book1.printOrderBook();

    Book1.findBestBidLevel();

    // test the bits if it doesnt work.

}
