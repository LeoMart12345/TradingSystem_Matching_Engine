// TCPOrderSession.hpp
// A client has this to submit orders
#pragma once
#include "../Matching_Engine/OrderBook/order.hpp"
#include "Order_Request.hpp"
#include <boost/asio.hpp>
#include <iostream>

class TCPOrderSession {
private:
  boost::asio::io_context ipContext;
  boost::asio::ip::tcp::socket socket;
  u_int64_t nextClientId;

public:
  TCPOrderSession() : socket(ipContext), nextClientId(1000) {
    std::cout << "Tcp Client Created" << std::endl;
  }

  void connect(const std::string &host, int port) {
    socket.connect(boost::asio::ip::tcp::endpoint(
        boost::asio::ip::address::from_string(host), port));
  }

  u_int64_t placeOrder(const Order &order) {
    OrderRequest request(nextClientId++, requestType::New, order);
    socket.write_some(boost::asio::buffer(request.serialize()));

    char response[1024];
    size_t bytes = socket.read_some(boost::asio::buffer(response));

    std::string resp(response, bytes);
    try {
      return std::stoull(resp);
    } catch (...) {
      throw std::runtime_error("Order rejected: " + resp);
    }
  }

  std::string cancelOrder(uint64_t orderId) {
    Price dummyPrice(0);
    Order dummyOrder(Bid, 0, "", orderId, dummyPrice);
    OrderRequest request(nextClientId++, requestType::Cancel, dummyOrder);
    socket.write_some(boost::asio::buffer(request.serialize()));

    char response[1024];
    size_t bytes = socket.read_some(boost::asio::buffer(response));
    return std::string(response, bytes);
  }
};
