#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "Order_Request.hpp"
#include "../Matching_Engine/OrderBook/order.hpp"
#include <MarketData.hpp>
#include "../Debug/Debug.hpp"
#include <boost/asio.hpp>
#include "UDPMarketDataReceiver.hpp"
#include "TCPOrderSession.hpp"

int main() {    
    //UDP handler
    MarketData marketData;
    UDPMarketDataReceiver udpReceiver(marketData);
    udpReceiver.start();
    //TCP handler
    TCPOrderSession TCPOrderEntry;
    TCPOrderEntry.connect("127.0.0.1", 5555);

    static uint64_t nextClientId = 1000;

    bool running = true;
    while (running) {
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
        std::cout << "3. Refresh\n";
        std::cout << "4. Exit\n\n";
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

            // use TCPOrderSession instead of socket directly
            uint64_t assignedId = TCPOrderEntry.placeOrder(order);
            std::cout << "Order accepted with ID: " << assignedId << std::endl;
            std::cout << "\nPress Enter...";
            std::cin.ignore();
            std::cin.get();
        }
        else if (choice == 2) {
            system("clear");
            std::string orderId;
            std::cout << "Order ID: ";
            std::cin >> orderId;

            // use TCPOrderSession instead of socket directly
            std::string response = TCPOrderEntry.cancelOrder(std::stoull(orderId));
            std::cout << "Server Response: " << response << std::endl;

            std::cout << "\nPress Enter...";
            std::cin.ignore();
            std::cin.get();
        }
        else if (choice == 3) {
            //refresh
        }else if (choice == 4) {
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