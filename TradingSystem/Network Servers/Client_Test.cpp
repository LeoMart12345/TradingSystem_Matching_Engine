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
#include "./Network Servers/MarketData.hpp"

class TraderClient{
    private: 
    MarketData marketData;
    UDPMarketDataReceiver udpReceiver;
    TCPOrderSession TCPsession;
    u_int64_t nextClientId;

    public:
    TraderClient(const std::string& host, int tcpPort) : 
    udpReceiver(marketData)
    {
        TCPsession.connect(host, tcpPort);
        udpReceiver.start();
    }


    u_int64_t placeOrder(const std::string& ticker, const std::string& side, uint64_t qty, uint64_t priceInCents) {
        Price orderPrice(priceInCents);
        Order order((side == "BUY") ? Bid : Ask, qty, ticker, 0, orderPrice);
        return TCPsession.placeOrder(order);
    }

    std::string cancelOrder(uint64_t orderId) {
        return TCPsession.cancelOrder(orderId);
    }

    MarketDataSnapshot getMarketData() {
        return marketData.get();
    }
    
    void runTerminal() {
        bool running = true;
        while (running) {
            system("clear");
            MarketDataSnapshot snapshot = getMarketData();
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

                std::transform(side.begin(), side.end(), side.begin(), ::toupper);

                uint64_t assignedId = placeOrder(tickerSymbol, side, std::stoull(qty), std::stoull(price));
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

                std::string response = cancelOrder(std::stoull(orderId));
                std::cout << "Server Response: " << response << std::endl;

                std::cout << "\nPress Enter...";
                std::cin.ignore();
                std::cin.get();
            }
            else if (choice == 3) {
                // To Refresh: it just repeats.
            }
            else if (choice == 4) {
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
}

};

int main() {
    std::string host = "127.0.0.1";
    TraderClient trader(host, 5555);
    trader.runTerminal();
}