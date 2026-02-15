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
    //start the TCP server for clients to connect.
    TCPServerI->run(); 
    //start the UDP server for distributing market data.

    // Match limit orders:    
    matchingEngine.matchLimitOrders();

}

void TradingSystem::stop(){

}

int main(){

    auto TS  = std::make_unique<TradingSystem>(5555, 12345);

    OrderBook& orderBook = TS->getOrderBook();

    // Benchmarking START
    for(int i = 0; i < 1000; i++){
        Order order = orderBook.generateRandomOrder();
        TS->addOrder(order);       
    }
    // Benchmarking END
    
    // starts the tcp server and then does some benchmarking.
    TS->start();

    

}