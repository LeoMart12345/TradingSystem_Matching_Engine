#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include "../Matching_Engine/MatchingEngine.hpp"
#include "Order_Request.hpp"
#include <string>
#include <MarketData.hpp>

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
        
        // UDP
        boost::asio::io_context UDPcontext;
        boost::asio::ip::udp::socket UDPsocket(UDPcontext);
        UDPsocket.open(boost::asio::ip::udp::v4());

        //endpoint that the udp multicast will send to
        ip::udp::endpoint multicastEndpoint(
            ip::address::from_string("239.0.0.1"), // multicast address. clients join this group
            9999
        );


        // thread for udp muticast market data dissemination:
        std::thread udpMarketdataDisseminationThread([&](){
            while(true){
                std::string serializedMarketData;
                // get the latest data from the orderBook
                // u_int64_t bestBidPrice = matchingEngine.getOrderBook().getBestBid().getPrice().getPriceInTicks();
                // u_int64_t bestAskPrice = matchingEngine.getOrderBook().getBestAsk().getPrice().getPriceInTicks(); 
                // u_int64_t bidVolume = 0; // DUMMY HARDCODED VALUE FOR TESTING.
                // u_int64_t askVolume = 0; // DUMMY HARDCODED VALUE FOR TESTING.
                try{
                    MarketDataSnapshot snapsshot;
                    snapsshot.askVolume = 0;
                    snapsshot.bidVolume = 0;
                    // NEED TO DO A TYPECAST HERE:FROM UINT TO U_INT_64_T ETC. 
                    snapsshot.bestBid = matchingEngine.getOrderBook().getBestBid().getPrice().getPriceInTicks();
                    snapsshot.bestAsk = matchingEngine.getOrderBook().getBestAsk().getPrice().getPriceInTicks();
                    snapsshot.bidVolume = 0;
                    snapsshot.askVolume = 0;

                    std::string data = snapsshot.serialise();
                    
                    //sending the data to the clients.
                    UDPsocket.send_to(boost::asio::buffer(data), multicastEndpoint);
                    
                    // multicast the data to all the clients.
                }catch(std::exception& e){
                    std::cout << "UDP error: " << e.what() << std::endl;
                }catch(...){
                    std::cout << "some other udp error" << std::endl;
                }
                // sleep so it doesnt take up resources.
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            }
        });
        udpMarketdataDisseminationThread.detach();


        // Handling TCP connections for order submittion:
        io_context context;
        ip::tcp::acceptor acceptor(context, ip::tcp::endpoint(ip::tcp::v4(), 5555));
        std::cout << "order entry via tcp on port 5555" << std::endl;


        // spawning a thread for each TCP connection. dont close the socket after each connection.
        // moving the ownership of the socket from the main thread to the detached one.
        while(true){
            ip::tcp::socket socket(context);
            acceptor.accept(socket);
            
            // spawn a thread for each client connection
            std::thread clientThread([this](boost::asio::ip::tcp::socket socket){
                while(true){
                    try {
                        char order[1024];
                        size_t bytes = socket.read_some(buffer(order));
                        std::string receivedData(order, bytes);

                        OrderRequest request = OrderRequest::deserialize(receivedData);
                        
                        if(request.type == requestType::New){
                            Order newOrder = request.requestOrder;
                            u_int64_t assignedId = matchingEngine.addOrder(newOrder);
                            
                            std::string response = std::to_string(assignedId);
                            socket.write_some(boost::asio::buffer(response));
                        }
                    } catch(const std::exception& e) {
                        std::cout << "Client disconnected: " << e.what() << std::endl;
                        break;
                    }
                }
            }, std::move(socket));
            
            clientThread.detach();
        }
    }

    void stop(){

    }

};