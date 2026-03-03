#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <MarketData.hpp>

#include "../Matching_Engine/MatchingEngine.hpp"
#include "Order_Request.hpp"
#include "./Network Servers/UDPServer.hpp"
#include "./Matching_Engine/OrderBook/ObjectPool.hpp"

class TCPServer{

    private:
        int port;
        MatchingEngine& matchingEngine;
    public:
        TCPServer(int p, MatchingEngine& engine) : 
        port(p),
        matchingEngine(engine)
        {
            
        }
        
void run(){
    using namespace boost::asio;

    // TCP only
    io_context context;
    ip::tcp::acceptor acceptor(context, ip::tcp::endpoint(ip::tcp::v4(), 5555));
    std::cout << "order entry via tcp on port 5555" << std::endl;

    while(true){
        ip::tcp::socket socket(context);
        acceptor.accept(socket);
         
        std::thread clientThread([this](ip::tcp::socket socket){
            while(true){
                try {
                    char order[1024];
                    size_t bytes = socket.read_some(buffer(order));
                    std::string receivedData(order, bytes);

                    OrderRequest request = OrderRequest::deserialize(receivedData);
                    if(request.type == requestType::New){
                      try {
                          Order* newOrder = matchingEngine.getOrderBook().getOrderPool().acquire();
                          *newOrder = request.requestOrder;
                          u_int64_t assignedId = matchingEngine.addOrder(newOrder);
                          std::string response = std::to_string(assignedId);
                          socket.write_some(boost::asio::buffer(response));
                      } catch(const std::exception& e){
                          std::string response = "Order rejected: " + std::string(e.what());
                          socket.write_some(boost::asio::buffer(response));
                      }
                  }else if(request.type == requestType::Cancel){
                        u_int64_t orderId = request.requestOrder.mOrderID;
                        try{
                            // matchingEngine.getOrderBook().removeOrderFromOrderId(orderId);
                            matchingEngine.cancelOrder(orderId);
                            std::string response = "Order cancelled: " + std::to_string(orderId);
                            socket.write_some(boost::asio::buffer(response));
                        }catch(...){
                            std::string response = "Cancel failed: order not found";
                            socket.write_some(boost::asio::buffer(response));
                        }
                    }
                } catch(const std::exception& e) {
                    std::cout << "Client disconnected: " << e.what() << std::endl;
                    break;
                }
            }
        }, std::move(socket));
        
        clientThread.detach();
    }
}

    void stop(){

    }

};
