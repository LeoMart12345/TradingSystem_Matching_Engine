#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "TCP_Client.hpp"
#include "Order_Request.hpp"
#include "../Matching_Engine/OrderBook/order.hpp"
#include <MarketData.hpp>
#include "../Debug/Debug.hpp"

int main() {
    using namespace boost::asio;
    bool running = true;
    MarketData marketData;
    //TCP
    boost::asio::io_context context;
    boost::asio::ip::tcp::socket socket(context);

    socket.connect(boost::asio::ip::tcp::endpoint(
        boost::asio::ip::address::from_string("127.0.0.1"), 
        5555
    ));
    // UDP
    boost::asio::io_context UDPcontext;
    boost::asio::ip::udp::socket UDPsocket(UDPcontext);
    
    UDPsocket.open(boost::asio::ip::udp::v4());
    UDPsocket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    UDPsocket.bind(boost::asio::ip::udp::endpoint(
        boost::asio::ip::address::from_string("239.0.0.1"),
        9999
    ));

    std::string multicast_address = "239.0.0.1"; // multicast address.
    UDPsocket.set_option(
        boost::asio::ip::multicast::join_group(
            boost::asio::ip::address::from_string(multicast_address)
        )
    );

    std::thread MulticastReceive([&](){
        while(true){
            char buff[1024];
            size_t bytes = UDPsocket.receive(boost::asio::buffer(buff));
            std::string data(buff, bytes);
            // std::cout << "UPD received data: " << data << std::endl;
            marketData.deserialise(data);
        }
    });
    MulticastReceive.detach();
    
    
    while (running) {
        // clearing screen
        system("clear");
        
        MarketDataSnapshot snapshot = marketData.get();

        // Market data that will be updated via UDP multicasts
        std::cout << "=== TRADING TERMINAL ===\n\n";
        std::cout << "+-----------+----------+----------+\n";
        std::cout << "| APPL      |   BID    |   ASK    |\n";
        std::cout << "+-----------+----------+----------+\n";
        std::cout << "| Price     | " << std::setw(8) << snapshot.bestBid << " | " << std::setw(8) << snapshot.bestAsk << " |\n";
        std::cout << "| Volume    | " << std::setw(8) << snapshot.bidVolume << " | " << std::setw(8) << snapshot.askVolume << " |\n";
        std::cout << "+-----------+----------+----------+\n\n";

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
                (side == "BUY") ? Bid : Ask,
                std::stoull(qty),
                tickerSymbol,
                0,
                orderPrice
            );
            // Make Order request.
            static uint64_t nextClientId = 1000; // for order id generation
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