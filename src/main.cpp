#include <iostream>
#include <string>
#include <cstdlib>
#include "auth/AuthManager.h"
#include "order_management/OrderManager.h"
#include "account_management/AccountManager.h"
#include "market_data/MarketDataManager.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Clear Console
void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Wait for User Input
void waitForKey() {
    std::cout << "\nPress any key to continue...";
    std::cin.ignore(); // Clear the newline character from the input buffer
    std::cin.get();    // Wait for the user to press a key
}

// Beautify JSON Output
std::string beautifyJson(const std::string& jsonString) {
    try {
        json parsedJson = json::parse(jsonString);
        return parsedJson.dump(4); // Indent with 4 spaces
    } catch (const std::exception& e) {
        std::cerr << "Error while beautifying JSON: " << e.what() << std::endl;
        return jsonString;
    }
}

// Menu Display
void displayMenu() {
    std::cout << "\n=== Deribit Trading System Menu ===\n";
    std::cout << "1. Place Order\n";
    std::cout << "2. Modify Order\n";
    std::cout << "3. Cancel Order\n";
    std::cout << "4. Get Account Summary\n";
    std::cout << "5. Get Current Positions\n";
    std::cout << "6. Get Order Book\n";
    std::cout << "7. Exit\n";
    std::cout << "Enter your choice: ";
}

int main() {
    std::string clientId, clientSecret;
    std::cout << "Enter your Deribit client_id: ";
    std::getline(std::cin, clientId);
    std::cout << "Enter your Deribit client_secret: ";
    std::getline(std::cin, clientSecret);

    AuthManager authManager(clientId, clientSecret);
    std::string token = authManager.authenticate();

    if (token.empty()) {
        std::cerr << "Authentication Failed!" << std::endl;
        return 1;
    }

    std::cout << "Authentication Successful! Token: " << token << std::endl;

    OrderManager orderManager(token);
    AccountManager accountManager(token);
    MarketDataManager marketDataManager;

    int choice;
    while (true) {
        clearConsole();
        displayMenu();
        std::cin >> choice;
        std::cin.ignore(); // Clear the newline character

        clearConsole();

        if (choice == 7) {
            std::cout << "Exiting the system. Goodbye!" << std::endl;
            break;
        }

        switch (choice) {
            case 1: { // Place Order
                std::string instrument, side;
                double amount, price;
                std::cout << "Enter instrument name (e.g., BTC-PERPETUAL): ";
                std::cin >> instrument;
                std::cout << "Enter side (buy/sell): ";
                std::cin >> side;
                std::cout << "Enter amount: ";
                std::cin >> amount;
                std::cout << "Enter price: ";
                std::cin >> price;

                std::string response = orderManager.placeOrder(instrument, side, amount, price);
                std::cout << "Order Placement Response:\n" << beautifyJson(response) << std::endl;
                break;
            }

            case 2: { // Modify Order
                std::string orderId;
                double newAmount, newPrice;
                std::cout << "Enter order ID to modify: ";
                std::cin >> orderId;
                std::cout << "Enter new amount: ";
                std::cin >> newAmount;
                std::cout << "Enter new price: ";
                std::cin >> newPrice;

                std::string response = orderManager.modifyOrder(orderId, newAmount, newPrice);
                std::cout << "Modify Order Response:\n" << beautifyJson(response) << std::endl;
                break;
            }

            case 3: { // Cancel Order
                std::string orderId;
                std::cout << "Enter order ID to cancel: ";
                std::cin >> orderId;

                std::string response = orderManager.cancelOrder(orderId);
                std::cout << "Cancel Order Response:\n" << beautifyJson(response) << std::endl;
                break;
            }

            case 4: { // Get Account Summary
                std::string response = accountManager.getAccountSummary();
                std::cout << "Account Summary:\n" << beautifyJson(response) << std::endl;
                break;
            }

            case 5: { // Get Current Positions
                std::string response = accountManager.getPositions();
                std::cout << "Current Positions:\n" << beautifyJson(response) << std::endl;
                break;
            }

            case 6: { // Get Order Book
                std::string instrument;
                std::cout << "Enter instrument name for order book (e.g., BTC-PERPETUAL): ";
                std::cin >> instrument;

                std::string response = marketDataManager.getOrderBook(instrument);
                std::cout << "Order Book:\n" << beautifyJson(response) << std::endl;
                break;
            }

            default:
                std::cerr << "Invalid choice. Please try again." << std::endl;
        }

        waitForKey(); // Wait for user input and clear console for the next menu
    }

    return 0;
}
