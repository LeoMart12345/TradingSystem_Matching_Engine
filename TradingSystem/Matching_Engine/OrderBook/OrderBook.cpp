#include <iostream>
#include <array>
#include <vector>
#include <bitset>

#include "OrderBook.hpp"
#include "order.hpp"
#include "../Debug/Debug.hpp"
#include <bit>
#include <atomic>
#include "../UtilityClass/OrderIdGenerator.hpp"

std::mt19937 rng(std::random_device{}()); 

std::uniform_int_distribution<int> PriceDist(950, 1050);
std::uniform_int_distribution<int> sideDist(0, 1);
std::uniform_int_distribution<int> volumeDist(0, 1000);

static constexpr int CHUNK = 64;
constexpr int StartOfPrice = 950;
constexpr int NumOfLevels = 101;

//Order Ids that are used for benchmarking

OrderBook::OrderBook(size_t size) 
    : mBidpriceLevel(size),
      mAskPriceLevel(size),
      mBidBitmap((size + CHUNK - 1) / CHUNK, 0ULL),
      mAskBitmap((size + CHUNK - 1) / CHUNK, 0ULL)
{
    DEBUG_PRINT("OrderBook created with " << size << " price levels");
    DEBUG_PRINT("BitMap create with " << mBidBitmap.size() << " 64 bit elements");
}

// Functions for benchmarking the OrderBook START
//     Side BidOrAsk; u_int64_t mVolume; std::string mName; u_int64_t mOrderID; Price mPrice;

Order OrderBook::generateRandomOrder(){
    
    int randPriceint = PriceDist(rng);
    int randSide = sideDist(rng);
    u_int64_t randVolume = volumeDist(rng);
    std::string notsoRandName = "TSLA";
    Side side = (randSide == 0) ? Bid : Ask;
    Price randPrice(randPriceint);

    //     Order(Side Side, u_int64_t volume, std::string name, u_int64_t orderID, Price Price);

    Order randomOrder(side, randVolume, notsoRandName, OrderIdGenerator::incrementOrder(), randPrice);
    
    return randomOrder;
}

// need to add tracking for orderid for cancelOrder
void OrderBook::addBid(Order order)
{
    DEBUG_PRINT("OrderBook::addBid - Order ID: " << order.mOrderID);
    int BidLevel = priceToIndex(order.mPrice);
    mBidpriceLevel[BidLevel].emplace_back(order);
    DEBUG_PRINT("ORDERBOOK::ADDBID printing price of the order below.");
    setBidBitTo1(order.getPrice());
    // add the orderid to the map
    orderIdtoPriceMapping[order.mOrderID] = { order.mPrice, Bid};
    DEBUG_PRINT("ORDERBOOK::ADDBID after printing price");
}

void OrderBook::addAsk(Order order)
{
    DEBUG_PRINT("OrderBook::addAsk - Order ID: " << order.mOrderID);
    int AskLevel = priceToIndex(order.mPrice);
    mAskPriceLevel[AskLevel].emplace_back(order);
    setAskBitTo1(order.getPrice());
    orderIdtoPriceMapping[order.mOrderID] = { order.mPrice, Ask};
    DEBUG_PRINT("ORDERBOOK::ADDASK after printing price");
}

void OrderBook::removeBid(u_int64_t orderId)
{
    int bestLevel = findBestBidLevel();
    if(bestLevel == -1) return;
    auto& deque = mBidpriceLevel[bestLevel];
    
    if (deque.empty()) return;

    DEBUG_PRINT("front of deque Id: " << deque.front().getOrderId() << " OrderId: " << orderId);

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
    int bestLevel = findBestAskLevel();
    if(bestLevel == -1) return;
    auto& deque = mAskPriceLevel[bestLevel];

    if(deque.empty()) return;

    if(deque.front().getOrderId() != orderId){
        std::__throw_logic_error("orderId does not matche the order at thr front of that queue level");
    }
    deque.pop_front();

    if(deque.empty()){
        setAskBitTo0(indexToPrice(bestLevel));
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

    if (index < 0 || index >= NumOfLevels)
    {
        DEBUG_PRINT("ERROR that price is not a price that this asset can be traded at");
        throw std::invalid_argument("Invalid price to bid/ask");
    }
    return index;
}

Price OrderBook::indexToPrice(int levelIndex) const
{
    if (levelIndex < 0 || levelIndex >= static_cast<int>(mBidpriceLevel.size())) {
        throw std::invalid_argument("Invalid level index: " + std::to_string(levelIndex));
    }

    int priceInCents = StartOfPrice + levelIndex;

    return Price(priceInCents);
}

std::pair<size_t, size_t> OrderBook::priceToBitmapIndex(Price price)
{
    int levelIndex = priceToIndex(price);

    return indexToBitmapIndex(levelIndex);
}

std::pair<size_t, size_t> OrderBook::indexToBitmapIndex(int levelIndex)
{
    size_t wordIndex = levelIndex / CHUNK;
    size_t bitIndex = levelIndex % CHUNK;

    DEBUG_PRINT("Word Index: " << wordIndex);
    DEBUG_PRINT("Bit Index: " << bitIndex);

    return {wordIndex, bitIndex};
}

void OrderBook::setBidBitTo1(const Price &price)
{
    auto result = priceToBitmapIndex(price);
    size_t wordPos = result.first;
    size_t bitPos = result.second;

    u_int64_t bitmask = (1ULL << bitPos);

    mBidBitmap[wordPos] |= bitmask;
}

void OrderBook::setBidBitTo0(const Price &price)
{
    DEBUG_PRINT("entering setBidBitTo0");
    auto result = priceToBitmapIndex(price);
    size_t wordPos = result.first;
    size_t bitPos = result.second;

    mBidBitmap[wordPos] &= ~(1ULL << bitPos);    
}

void OrderBook::setAskBitTo1(const Price &price)
{
    auto result = priceToBitmapIndex(price);
    size_t wordPos = result.first;
    size_t bitPos = result.second;

    u_int64_t bitmask = (1ULL << bitPos);

    mAskBitmap[wordPos] |= bitmask;
}

void OrderBook::setAskBitTo0(const Price &price)
{
    DEBUG_PRINT("entering setAskBitTo0");
    auto result = priceToBitmapIndex(price);
    size_t wordPos = result.first;
    size_t bitPos = result.second;

    mAskBitmap[wordPos] &= ~(1ULL << bitPos);    
}

Order& OrderBook::getBestBid(){
    int bestBidLevel = findBestBidLevel();

    if(bestBidLevel == -1){
        throw std::runtime_error("No bids in the OrderBook!");
    }
    auto& deque = mBidpriceLevel[bestBidLevel];

    if(deque.empty()){
        throw std::runtime_error("bitmapp shows the level marked but the deque is empty");
    }

    return deque.front();
}

Order& OrderBook::getBestAsk(){
    int bestAskLevel = findBestAskLevel();

    if(bestAskLevel == -1){
        throw std::runtime_error("No Asks in the OrderBook");
    }
    auto& deque = mAskPriceLevel[bestAskLevel];

    if(deque.empty()){
        throw std::runtime_error("bitmapp shows the level marked but the deque is empty");
    }

    return deque.front();
}

Order OrderBook::popBestBid(){
    int bestBidLevel = findBestBidLevel();

    if(bestBidLevel == -1){
        throw std::runtime_error("No bids in the OrderBook!");
    }
    auto& deque = mBidpriceLevel[bestBidLevel];

    if(deque.empty()){
        throw std::runtime_error("bitmapp shows the level marked but the deque at that level is empty!");
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
        throw std::runtime_error("bitmap shows the level marked but the deque at that level is empty!");
    }
    Order orderCopy = deque.front();
    deque.pop_front();

    if(deque.empty()){
        setAskBitTo0(indexToPrice(bestAskLevel));
    }

    return orderCopy;
}

int OrderBook::findBestBidLevel() const{
    for(int i = static_cast<int>(mBidBitmap.size() - 1); i >= 0; --i){
        u_int64_t word = mBidBitmap[i];

        if(word != 0){
            int leadingZeros = std::__countl_zero(word);
            
            int bitPositionFromRight = 63 - leadingZeros;
            
            return i * CHUNK  + bitPositionFromRight; 
        }
    }
    return -1;
}

int OrderBook::findBestAskLevel() const{
    for(int i = 0; i < static_cast<int>(mAskBitmap.size()); ++i){

        u_int64_t word = mAskBitmap[i];

        if(word != 0){
            
            int trailingZeros = std::__countr_zero(word);
            
            return i * CHUNK  + trailingZeros; 
        }
    }
    return -1;
}

std::pair<Price, Side> OrderBook::orderIdToPrice(const u_int64_t orderId) const {
    auto it = orderIdtoPriceMapping.find(orderId);
    
    if(it != orderIdtoPriceMapping.end()){
        Price price = it->second.price;
        Side side = it->second.side;
        
        return std::make_pair(price, side);
    }
    throw std::runtime_error("there was no orderID with this number:" + std::to_string(orderId));
}

void OrderBook::removeOrderFromOrderId(u_int64_t orderId){
    // use the orderIdToPrice to find the level
    std::pair<Price, Side>orderPair = orderIdToPrice(orderId);   
    // Translate the price to the index
    int orderIndex = priceToIndex(orderPair.first);
    // Search through to find the order.

    if(orderPair.second == Side::Bid){
        // logic for the bid side:
        std::deque<Order>& orderDeQue = mBidpriceLevel[orderIndex];
        // for loop looping over the deque.
        for(auto it = orderDeQue.begin(); it != orderDeQue.end(); it++){
            if(it->mOrderID == orderId){
                orderDeQue.erase(it);
                if(orderDeQue.empty()){
                    setBidBitTo0(orderPair.first);
                }
                break;
            }
        }
    }
    else{// TODO implement this side
        std::deque<Order>& orderDeQue = mAskPriceLevel[orderIndex];
         
        for(auto it = orderDeQue.begin(); it != orderDeQue.end(); it++){
            if(it->mOrderID == orderId){
                orderDeQue.erase(it);
                if(orderDeQue.empty()){
                    setAskBitTo0(orderPair.first);
                }
                break;
            }
        }
    }

    orderIdtoPriceMapping.erase(orderId);
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