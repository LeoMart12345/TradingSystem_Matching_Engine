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

    std::string serialise() const {
        return std::to_string(bestBid) + "," 
        + std::to_string(bestAsk) + ","
        + std::to_string(bidVolume) + ","
        + std::to_string(askVolume);
    }

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

    void deserialise(const std::string& marketDataString){
        std::lock_guard<std::mutex> lock(mMutex);
        std::stringstream ss(marketDataString);
        std::string token;
        // best bid
        std::getline(ss ,token, ',');
        mSnapshot.bestBid = std::stoull(token);
        // best ask
        std::getline(ss, token, ',');
        mSnapshot.bestAsk = std::stoull(token);
        // bid volume
        std::getline(ss, token, ',');
        mSnapshot.bidVolume = std::stoull(token);
        // ask volume
        std::getline(ss, token, ',');
        mSnapshot.askVolume = std::stoull(token);
    }

private:
    MarketDataSnapshot mSnapshot{};
    mutable std::mutex mMutex;
};