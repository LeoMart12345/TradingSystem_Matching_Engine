#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "TCP_Client.hpp"
#include "Order_Request.hpp"
#include "../Matching_Engine/OrderBook/order.hpp"
#include "MarketData.hpp"
#include "thread"

int main() {
    bool running = true;
    
    using namespace boost::asio;
        // market data that the client will receive
        MarketData marketData;

        // setting up the receiver socket
        boost::asio::io_context UDPcontext;
        boost::asio::ip::udp::socket UDPSocket(UDPcontext);

        // spin up a thead to receive the data

        std::thread udpThread([&](){
            while(true){
                char buff[1024];
                UDPSocket.receive(boost::asio::buffer(buff));
                // deserialize that data.
                // update the market data so it can be displayed.
            }

        });
        
        
        
        //TCP
        boost::asio::io_context context;
        boost::asio::ip::tcp::socket socket(context);

        socket.connect(boost::asio::ip::tcp::endpoint(
            boost::asio::ip::address::from_string("127.0.0.1"), 
            5555
        ));
    while (running) {
        // clearing screen
        system("clear");
        
        std::cout << "=== TRADING TERMINAL ===\n\n";
        std::cout << "1. Place Order\n";
        std::cout << "2. Cancel Order\n";
        std::cout << "3. Exit\n\n";
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        
        if (choice == 1) {
            system("clear");
            std::cout << "=== PLACE ORDER ===\n\n";
            
            std::string tickerSymbol, side, qty, price;
            
            std::cout << "Ticker Symbol: ";
            std::cin >> tickerSymbol;
            
            std::cout << "Side (BUY/SELL): ";
            std::cin >> side;
            
            std::cout << "Quantity: ";
            std::cin >> qty;
            
            std::cout << "Price: ";
            std::cin >> price;
            
            std::cout << "\nSending: " << side << " " << tickerSymbol 
                      << " " << qty << " @ " << price << "\n";
            
            // standardise the inputs
            std::transform(side.begin(), side.end(), side.begin(), ::toupper);

            // Making price object:
            u_int64_t priceInCents = std::stoull(price);

            Price orderPrice(priceInCents); 
            // Make Order Object
            Order order(
                (side == "Buy") ? Bid : Ask,
                std::stoull(qty),
                tickerSymbol,
                0,
                orderPrice
            );
            // Make Order request.
            static uint64_t nextClientId = 1000; // For order id generation

            OrderRequest request(nextClientId++, requestType::New, order);

            socket.write_some(boost::asio::buffer(request.serialize()));

            // server response    
            char response[1024];
            size_t bytes = socket.read_some(boost::asio::buffer(response));
            std::string realOrderId(response, bytes);
            std::cout << "Order accepted with ID: " << realOrderId << std::endl;


            std::cout << "\nPress Enter...";
            std::cin.ignore();
            std::cin.get();
        }
        else if (choice == 2) {
            system("clear");
            std::cout << "=== CANCEL ORDER ===\n\n";
            
            std::string orderId;
            std::cout << "Order ID: ";
            std::cin >> orderId;
            
            std::cout << "\nCancelling order: " << orderId << "\n";
            
            std::cout << "\nPress Enter...";
            std::cin.ignore();
            std::cin.get();
        }
        else if (choice == 3) {
            running = false;
            std::cout << "\nExiting...\n";
        }
        else {
            std::cout << "\nInvalid choice!\n";
            std::cout << "Press Enter...";
            std::cin.ignore();
            std::cin.get();
        }
    }
    
    return 0;
}