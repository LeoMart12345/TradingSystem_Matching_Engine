// MarketData.hpp
#pragma once
#include <cstdint>
#include <mutex>
#include <string>
#include <sstream>    

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

    // for the server to serialise and send to the client.
    std::string serialise() const {
        std::lock_guard<std::mutex> lock(mMutex);

        return std::to_string(mSnapshot.bestBid) + "," 
        + std::to_string(mSnapshot.bestAsk) + ","
        + std::to_string(mSnapshot.bidVolume) + ","
        + std::to_string(mSnapshot.askVolume);
    }

    void deserialise(const std::string& marketDataString){
        std::lock_guard<std::mutex> lock(mMutex);
        std::stringstream ss(marketDataString);
        std::string token;

        std::getline(ss ,token, ',');
        mSnapshot.bestBid = std::stoull(token);

        std::getline(ss, token, ',');
        mSnapshot.bestAsk = std::stoull(token);

        std::getline(ss, token, ',');
        mSnapshot.bidVolume = std::stoull(token);

        std::getline(ss, token, ',');
        mSnapshot.askVolume = std::stoull(token);
    }

private:
    MarketDataSnapshot mSnapshot{};
    mutable std::mutex mMutex;
};