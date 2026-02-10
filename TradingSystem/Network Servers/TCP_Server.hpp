#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include "../Matching_Engine/MatchingEngine.hpp"

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
            std::string order_str(order, bytes);

            // TODO: check if the order is valid
            std::string orderSide;
            std::string ticker;
            int volume;
            int price;

            std::stringstream ss(order_str);

            ss >> orderSide;
            ss >> ticker;
            ss >> volume;
            ss >> price;

            std::cout << "Parsed messges" << orderSide << ticker << volume << price << std::endl;

        
            std::string confirm = "ORDER_ACCEPTED: " + order_str;
            std::cout << confirm << std::endl;

            socket.write_some(boost::asio::buffer(confirm));
        }
        
    }

    void stop(){

    }

};