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

    //TODO : Update the bitmap if its 0 else do nothing
}

void OrderBook::removeBid(Order Order) {
    // get the price of the bid

}

void OrderBook::getVolumeAtLevel(Price price){

}

void OrderBook::getTotalVolume(){

}


// helper
int OrderBook::priceToIndex(Price price) const{
    int index = price.mPriceValueInCent - StartOfPrice;
    
    // need to do bounds checking here. // TODO
    if(index < 0 || index >= NumOfLevels){
        std::cout << "ERROR that price is not a price that this asset can be traded at" << std::endl;
        throw std::invalid_argument( "Invalid price to bid/ask");

    }

    return index;
}

void OrderBook::printOrderBook() const{
    std::cout << "------------------------------" << std::endl;
    
}


//testing the API
int main(){
    OrderBook Book1(NumOfLevels);

    
    Order myOrder = Order(Bid, 100, "apple", 100, 1000);

    Book1.addBid(myOrder);

    Book1.printOrderBook();
   
}