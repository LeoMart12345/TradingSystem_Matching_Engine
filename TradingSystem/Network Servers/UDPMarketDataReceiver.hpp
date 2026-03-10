// UDPMarketDataReceiver
//  A client has this to receive udp market data from server
#pragma once
#include "../Matching_Engine/OrderBook/order.hpp"
#include "Order_Request.hpp"
#include <MarketData.hpp>
#include <boost/asio.hpp>
#include <iostream>

class UDPMarketDataReceiver {
private:
  boost::asio::io_context UDPcontext;
  boost::asio::ip::udp::socket UDPsocket;
  MarketData &marketData;
  /* data */
public:
  explicit UDPMarketDataReceiver(MarketData &md)
      : UDPsocket(UDPcontext), marketData(md) {
    UDPsocket.open(boost::asio::ip::udp::v4());
    UDPsocket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    UDPsocket.bind(boost::asio::ip::udp::endpoint(
        boost::asio::ip::address::from_string("239.0.0.1"), 9999));
    UDPsocket.set_option(boost::asio::ip::multicast::join_group(
        boost::asio::ip::address::from_string("239.0.0.1")));
  }

  void start() {
    std::thread([this]() {
      std::cout << "UDPReceiver: thread started, listening for packets"
                << std::endl;
      while (true) {
        char buff[1024];
        size_t bytes = UDPsocket.receive(boost::asio::buffer(buff));
        std::string data(buff, bytes);
        // std::cout << "UDPReceiver: got packet: " << data << std::endl;

        // This receves both the trade data and the market data just continue if
        // its the trade or deserialise wont work
        // just searching for trade at the position 0 in the message.

        if (data.starts_with("TRADE:"))
          continue;

        marketData.deserialise(data);
        // std::cout << "UDPReceiver: updated bid=" << marketData.get().bestBid
        // << " ask=" << marketData.get().bestAsk << std::endl;
      }
    }).detach();
  }
};
