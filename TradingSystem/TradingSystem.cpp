#include <iostream>
#include <vector>
#include "./Matching_Engine/MatchingEngine.hpp"
#include "./Matching_Engine/OrderBook/OrderBook.hpp"
#include "./TradingSystem.hpp"

TradingSystem::TradingSystem(int tcp_port, int udp_port)
    : orderBook(std::make_unique<OrderBook>(101)),
    matchingEngine(*orderBook),
    TCPServerI(std::make_unique<TCPServer>(tcp_port, matchingEngine)),
    UDPServerI(std::make_unique<UDPServer>(matchingEngine))
{
    
}

TradingSystem::~TradingSystem(){

}

void TradingSystem::start(){

    // start making random orders for testing the udp multicast.

    //TCP internally this creates a new thread for each connection
    std::thread tcpThread([this](){
        TCPServerI->run();
    });
    tcpThread.detach();

    //UDP
    UDPServerI->run();

    // Match limit orders:    
    while(1){
        matchingEngine.matchLimitOrders();
    }
}

void TradingSystem::stop(){
    // TCPServer->stop();
    // stop the udp server also
}