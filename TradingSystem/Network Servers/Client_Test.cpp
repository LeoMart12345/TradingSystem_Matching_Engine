#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "TCP_Client.hpp"
#include "Order_Request.hpp"

int main() {
    bool running = true;
    
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
            
            std::string symbol, side, qty, price;
            
            std::cout << "Symbol: ";
            std::cin >> symbol;
            
            std::cout << "Side (BUY/SELL): ";
            std::cin >> side;
            
            std::cout << "Quantity: ";
            std::cin >> qty;
            
            std::cout << "Price: ";
            std::cin >> price;
            
            std::cout << "\nSending: " << side << " " << symbol 
                      << " " << qty << " @ " << price << "\n";
            

            // TODO:  send the order to the server.


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