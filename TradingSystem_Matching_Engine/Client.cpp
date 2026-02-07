#include  <iostream>
#include <boost/asio.hpp>

void client(const std::string& order){

    using namespace boost::asio;

    io_context context;

    ip::tcp::socket socket(context);

    socket.connect(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 5555));

    std::cout << "Connected to server. Sending: " << order << std::endl;

    // send the order.
    socket.write_some(boost::asio::buffer(order));
    
    // Wait for response
    char response[1024];
    size_t bytes = socket.read_some(boost::asio::buffer(response));
    std::string response_str(response, bytes);
    
    std::cout << "Server response: " << response_str << std::endl;
    
    socket.close();
}