// // SimulatedTraders.hpp
// #pragma once
// #include <thread>
// #include <iostream>
// #include <boost/asio.hpp>
// #include "Order_Request.hpp"
// #include <vector>
// #include <random>
// #include "./Network Servers/TCPOrderSession.hpp"
// #include "./Network Servers/UDPMarketDataReceiver.hpp"
// #include <atomic>

// class SimulatedTraders{
// private:
//     TCPOrderSession TCPsession;
//     UDPMarketDataReceiver UDPReceiver;
//     std::string traderId;
//     std::vector<SimulatedTraders> traders;
//     std::thread traderThread;
//     std::mt19937 rng{std::random_device{}()};


//     SimulatedTraders()
//         : traderId(traderId), 
//         TCPsession(),
//         UDPReceiver(md)
//     {
//         TCPsession.connect(host, TCPport);
//         UDPReceiver.start();
//     }
  

//     void start();
//     void end();

//     void spawnAndRunTraders(int numTraders){
//         for(int i = 0; i < numTraders; i++){

//         }    
//     }
    
//     void printAutotraderLog();

//     public:

// };