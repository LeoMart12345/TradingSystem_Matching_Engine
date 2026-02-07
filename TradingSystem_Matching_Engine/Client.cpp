#include  <iostream>
#include <iostream>
#include <chrono>
#include <boost/asio.hpp>




void server(){
    using namespace boost::asio;
    
    io_context context;
    ip::udp::socket serverSocket(context, ip::udp::endpoint(ip::udp::v4(), 5000));
    
    char bufferData[1024];
    
    ip::udp::endpoint senderEndpoint;

    context.run();
}


void client(){
    using namespace boost::asio;

    io_context context;


    context.run();
}

int main(){

    
} 