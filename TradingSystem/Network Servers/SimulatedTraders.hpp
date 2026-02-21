// SimulatedTraders.hpp
#pragma once
#include <thread>
#include <iostream>
#include <boost/asio.hpp>
#include "Order_Request.hpp"
#include <vector>
#include <random>
#include "./Network Servers/TCPOrderSession.hpp"
#include "./Network Servers/UDPMarketDataReceiver.hpp"
#include <atomic>
#include "./Network Servers/TraderClient.hpp"
#include <random>

class SimulatedTraders{
private:
    std::vector<TraderClient> Clients;
    std::string mHost;
    int mTcpServerPort;
    std::vector<std::thread> clientThreads;

public:
    SimulatedTraders(std::string& host, int tcpServerPort) 
    : mHost(host), mTcpServerPort(tcpServerPort)  
    {
        std::cout << "sumulated Traders constructed";

    }

    // Spawns the clients on individual threads and they start trading.
    void run(int numClients){
        for(int i = 0; i < numClients; i++){
            Clients.emplace_back(mHost, mTcpServerPort, "client" + std::to_string(i));
        
            clientThreads.emplace_back([this, i](){
                TraderClient::startRandomClientThread(Clients[i]);
            });
        }
    }
};