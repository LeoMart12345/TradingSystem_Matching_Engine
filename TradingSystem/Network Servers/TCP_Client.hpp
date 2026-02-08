// #pragma once
#include <iostream>
#include <boost/asio.hpp>

class TCPclient {
public:
    void run() {
        boost::asio::io_context context;
        boost::asio::ip::tcp::socket socket(context);
        
        socket.connect(boost::asio::ip::tcp::endpoint(
            boost::asio::ip::address::from_string("127.0.0.1"), 
            5555
        ));
        
        std::string order = "BUY AAPL 100 @ 150.25";
        
        socket.write_some(boost::asio::buffer(order));
        
        char response[1024];
        size_t bytes = socket.read_some(boost::asio::buffer(response));
        std::cout << std::string(response, bytes) << std::endl;
    }
};