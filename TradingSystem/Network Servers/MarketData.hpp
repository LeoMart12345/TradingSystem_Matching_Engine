// MarketData.hpp
#pragma once
#include <cstdint>
#include <atomic>

struct MarketDataSnapshot {
    uint64_t BestBid = 0;
    uint64_t BestAsk = 0;
    uint64_t BidVolume = 0;
    uint64_t AskVolume = 0;
};

class MarketData{
    
public:
    void update(MarketDataSnapshot updatedData){
        mSnapshot.store(updatedData);
    }

private:
    std::atomic<MarketDataSnapshot> mSnapshot{};
};
