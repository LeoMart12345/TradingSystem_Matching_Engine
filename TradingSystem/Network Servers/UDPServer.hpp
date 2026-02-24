#pragma once
#include <boost/asio.hpp>
#include "../Matching_Engine/MatchingEngine.hpp"
#include <MarketData.hpp>
#include <thread>
#include <chrono>

class UDPServer {
private:
    MatchingEngine& matchingEngine;
    boost::asio::io_context UDPcontext;
    boost::asio::ip::udp::socket UDPsocket;
    boost::asio::ip::udp::endpoint multicastEndpoint;
public:
    explicit UDPServer(MatchingEngine& engine) 
        : matchingEngine(engine),
          UDPsocket(UDPcontext),
          multicastEndpoint(
            boost::asio::ip::address::from_string("239.0.0.1"), 9999)
    {
        UDPsocket.open(boost::asio::ip::udp::v4());
        UDPsocket.set_option(boost::asio::ip::multicast::enable_loopback(true));
    }

    void run() {
        std::cout << "UDP server started" << std::endl;
        std::thread udpThread([this](){
            while(true) {
                try {
                    MarketDataSnapshot snapshot;
                    snapshot.bestBid = matchingEngine.getOrderBook().getBestBid()->getPrice().getPriceInTicks();
                    snapshot.bestAsk = matchingEngine.getOrderBook().getBestAsk()->getPrice().getPriceInTicks();
                    snapshot.bidVolume = 0;
                    snapshot.askVolume = 0;
                    
                    std::string data = snapshot.serialise();
                    // std::cout << "UDP Sending serlialised data: " << data << std::endl;
                    UDPsocket.send_to(boost::asio::buffer(data), multicastEndpoint);

                } catch(...) {}

                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        });
        int arr[100];

        udpThread.detach();
    }
};