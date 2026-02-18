#include <iostream>
#include <vector>
#include "./Matching_Engine/MatchingEngine.hpp"
// #include "./Network Servers/TCP_Client.hpp"
#include "./Matching_Engine/OrderBook/OrderBook.hpp"
#include "./TradingSystem.hpp"


TradingSystem::TradingSystem(int tcp_port, int udp_port)
    : orderBook(std::make_unique<OrderBook>(101)),
    matchingEngine(*orderBook),
    TCPServerI(std::make_unique<TCPServer>(tcp_port, matchingEngine))
{
    
}

TradingSystem::~TradingSystem(){

}

void TradingSystem::start(){

    // start making random orders for testing the udp multicast.


    // start the TCP server for clients to connect in a seperate thread.
    std::thread tcpServerThread([this](){
        TCPServerI->run();
    });
    tcpServerThread.detach(); // so it runs on its own.
    
    //start the UDP server for distributing market data in a seperate thread also.

    // Match limit orders:    
    while(1){
        matchingEngine.matchLimitOrders();
    }
}

void TradingSystem::stop(){

}