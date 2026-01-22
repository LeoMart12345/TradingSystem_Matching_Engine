#include <iostream> 
#include <array> 
#include <vector>
#include "OrderBook.hpp"
#include "order.hpp"

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

    std::cout << "OrderBook created with " << size << " price levels" << std::endl;
    std::cout << "BitMap create with " << mBitmapSize << " 64 bit elements" << std::endl;
}

void OrderBook::addBid(Order order){
    // get the price of the bid
    int BidLevel = priceToIndex(order.mPrice.mPriceValueInCent);
    mBidpriceLevel[BidLevel].emplace_back(order);

    // TODO : Update the bitmap if its 0 else do nothing
    // if(mBidBitmap[])
    // mBidBitmap
}

void OrderBook::removeBid(Order Order) {
    // get the price of the bid

}

void OrderBook::getVolumeAtLevel(Price price){

}

void OrderBook::getTotalVolume(){

}


// Helper functions
int OrderBook::priceToIndex(Price price) const{
    int index = price.mPriceValueInCent - StartOfPrice;
    
    // need to do bounds checking here. // TODO
    if(index < 0 || index >= NumOfLevels){
        std::cout << "ERROR that price is not a price that this asset can be traded at" << std::endl;
        throw std::invalid_argument( "Invalid price to bid/ask");

    }
    return index;
}

std::pair<size_t, size_t> OrderBook::priceToBitmapIndex(Price price){
    // TODO imeplement

    int levelIndex = priceToIndex(price);
    
    return indexToBitmapIndex(levelIndex);
}

std::pair<size_t, size_t> OrderBook::indexToBitmapIndex(int levelIndex){
    //TODO implement.


    size_t wordIndex = levelIndex / CHUNK;
    size_t bitIndex = levelIndex % CHUNK;

    // Printing for testing
    std::cout << wordIndex << std::endl; 


    std::cout << bitIndex << std::endl; 

    return{wordIndex, bitIndex};
}

    void OrderBook::setBidBitTo1(const Price& price){

        // TODO check if the bit is already 1   
        // use a logical AND with a bitmask with all 0 apart from the bit its checking 
        
        auto result = priceToBitmapIndex(price);
        size_t wordPos = result.first; // the position of the word in the bitmap array
        size_t bitPos = result.second; // the position of the bit in that word

        // make a bitmask that has all 0s apart from the place where im checking for 1. 0001000000 etc
        //then OR the original word with this bitmask

    }  

    void OrderBook::setBidBitTo0(const Price& price){

    }
    


void OrderBook::printOrderBook() const{
    std::cout << "------ OrderBook  -----" << std::endl;
        std::cout << "\nBIDS (high -> low)\n";
    for (int i = static_cast<int>(mBidpriceLevel.size()) - 1; i >= 0; --i) {
        if (mBidpriceLevel[i].empty()) continue;

        int priceTicks = StartOfPrice + i;
        std::cout << "Level " << i << " (Price €" << priceTicks/100 << "): ";

        for (const auto& o : mBidpriceLevel[i]) {
            std::cout << "[id=" << o.mOrderID
                      << ", qty=" << o.mVolume
                      << ", user=" << o.mName
                      << "] ";
        }
        std::cout << "\n";
    }

    std::cout << "\nASKS (low -> high)\n";
    for (int i = 0; i < static_cast<int>(mAskPriceLevel.size()); ++i) {
        if (mAskPriceLevel[i].empty()) continue;

        int priceTicks = StartOfPrice + i;
        std::cout << "Level " << i << " (ticks " << priceTicks << "): ";

        for (const auto& o : mAskPriceLevel[i]) {
            std::cout << "[id=" << o.mOrderID
                      << ", qty=" << o.mVolume
                      << ", Asset=" << o.mName
                      << "] ";
        }
        std::cout << "\n";
    }
    std::cout << "------------------------------" << std::endl;
}

//testing the API
int main(){
    OrderBook Book1(NumOfLevels);

    // make a price then make the order or make the 
    Order myOrder = Order(Bid, 100, "apple", 100, 1000);

    Book1.addBid(myOrder);
    
    Book1.printOrderBook();
    

    Book1.priceToBitmapIndex(myOrder.getPrice());

    // Book1.setBidBitTo1(1000);

}