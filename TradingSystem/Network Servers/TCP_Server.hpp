#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include "../Matching_Engine/MatchingEngine.hpp"
#include "Order_Request.hpp"

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
        
        io_context context;
        ip::tcp::acceptor acceptor(context, ip::tcp::endpoint(ip::tcp::v4(), 5555));
        std::cout << "order entry via tcp on port 5555" << std::endl;

        while(true){
            ip::tcp::socket socket(context);
            acceptor.accept(socket);
            
            char order[1024];
            size_t bytes = socket.read_some(buffer(order));
            std::string receivedData(order, bytes);

            // TODO: check if the order is valid

            OrderRequest request = OrderRequest::deserialize(receivedData);
            
            if(request.type == requestType::New){
                Order newOrder = request.requestOrder;

                // check if there is a need for a process order or if
                //send it to the matching engine
                
                u_int64_t assignedId = matchingEngine.addOrder(newOrder);

                // std::cout << "Adding the order to the matching engine" << std::endl;
                
                std::string response = std::to_string(assignedId);
                socket.write_some(boost::asio::buffer(response));
            }
            else if (request.type == requestType::Cancel) {
                // Handle the cancel orderType 
            }
        }
        
    }

    void stop(){

    }

};