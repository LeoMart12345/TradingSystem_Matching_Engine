// TcpServer listens on port 5555 for incoming client connections
// Each connection haned off the a detached thread
#pragma once
#include <MarketData.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>

#include "../Matching_Engine/MatchingEngine.hpp"
#include "./Matching_Engine/OrderBook/ObjectPool.hpp"
#include "./Network Servers/UDPServer.hpp"
#include "Order_Request.hpp"

class TCPServer {

private:
  int port;
  MatchingEngine &matchingEngine;

public:
  TCPServer(int p, MatchingEngine &engine) : port(p), matchingEngine(engine) {}

  void run() {
    using namespace boost::asio;

    // TCP only
    io_context context;
    ip::tcp::acceptor acceptor(context, ip::tcp::endpoint(ip::tcp::v4(), 5555));
    std::cout << "order entry via tcp on port 5555" << std::endl;

    while (true) {
      ip::tcp::socket socket(context);
      acceptor.accept(socket); // blocks until a client connects

      std::thread clientThread(
          [this](ip::tcp::socket socket) {
            while (true) {
              try {
                char order[1024];
                size_t bytes = socket.read_some(buffer(order));
                std::string receivedData(order, bytes);
                // deserialoses the wrapper around the order (order request)
                OrderRequest request = OrderRequest::deserialize(receivedData);
                if (request.type == requestType::New) {
                  try {
                    u_int64_t assignedId =
                        matchingEngine.addOrder(request.requestOrder);
                    std::string response = std::to_string(assignedId);
                    socket.write_some(boost::asio::buffer(response));
                  } catch (const std::exception &e) {
                    std::string response =
                        "Order rejected: " + std::string(e.what());
                    socket.write_some(boost::asio::buffer(response));
                  }
                } else if (request.type == requestType::Cancel) {
                  u_int64_t orderId = request.requestOrder.mOrderID;
                  try {
                    // matchingEngine.getOrderBook().removeOrderFromOrderId(orderId);
                    matchingEngine.cancelOrder(orderId);
                    std::string response =
                        "Order cancelled: " + std::to_string(orderId);
                    socket.write_some(boost::asio::buffer(response));
                  } catch (...) {
                    std::string response = "Cancel failed: order not found";
                    socket.write_some(boost::asio::buffer(response));
                  }
                }
              } catch (const std::exception &e) {
                std::cout << "Client disconnected: " << e.what() << std::endl;
                break;
              }
            }
          },
          std::move(socket)); // move ownership to the lambda, cant copy a file
                              // desicriptor
      clientThread.detach();
    }
  }

  void stop() {}
};
