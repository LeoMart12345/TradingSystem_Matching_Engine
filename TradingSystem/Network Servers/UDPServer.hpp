#pragma once
#include "../Matching_Engine/MatchingEngine.hpp"
#include <MarketData.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <thread>

class UDPServer {
private:
  MatchingEngine &matchingEngine;
  boost::asio::io_context UDPcontext;
  boost::asio::ip::udp::socket UDPsocket;
  boost::asio::ip::udp::endpoint multicastEndpoint;

public:
  explicit UDPServer(MatchingEngine &engine)
      : matchingEngine(engine), UDPsocket(UDPcontext),
        multicastEndpoint(boost::asio::ip::address::from_string("239.0.0.1"),
                          9999) {
    UDPsocket.open(boost::asio::ip::udp::v4());
    UDPsocket.set_option(boost::asio::ip::multicast::enable_loopback(true));
  }

  void run() {
    std::cout << "UDP server started" << std::endl;
    std::thread udpThread([this]() {
      std::cout << "UDP thread started" << std::endl; // add this

      while (true) {
        try {
          MarketDataSnapshot snapshot;
          Order *bestBid = matchingEngine.getOrderBook().getBestBid();
          Order *bestAsk = matchingEngine.getOrderBook().getBestAsk();

          snapshot.bestBid = bestBid->getPrice().getPriceInTicks();
          snapshot.bestAsk = bestAsk->getPrice().getPriceInTicks();

          auto [bidVol, ignore1] =
              matchingEngine.getOrderBook().getVolumeAtLevelFromPrice(
                  bestBid->getPrice());
          auto [ignore2, askVol] =
              matchingEngine.getOrderBook().getVolumeAtLevelFromPrice(
                  bestAsk->getPrice());

          snapshot.bidVolume = bidVol;
          snapshot.askVolume = askVol;

          std::string data = snapshot.serialise();
          UDPsocket.send_to(boost::asio::buffer(data), multicastEndpoint);

        } catch (const std::exception &e) {
          MarketDataSnapshot empty;
          UDPsocket.send_to(boost::asio::buffer(empty.serialise()),
                            multicastEndpoint);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      }
    });
    int arr[100];

    udpThread.detach();
  }
};
