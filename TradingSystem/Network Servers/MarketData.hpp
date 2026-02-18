// MarketData.hpp
#pragma once
#include <cstdint>
#include <mutex>

struct MarketDataSnapshot {
    uint64_t bestBid = 0;
    uint64_t bestAsk = 0;
    uint64_t bidVolume = 0;
    uint64_t askVolume = 0;
};

class MarketData {
public:
    void update(MarketDataSnapshot snapshot) {
        std::lock_guard<std::mutex> lock(mMutex);
        mSnapshot = snapshot;
    }

    MarketDataSnapshot get() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mSnapshot;
    }

private:
    MarketDataSnapshot mSnapshot{};
    mutable std::mutex mMutex;
};