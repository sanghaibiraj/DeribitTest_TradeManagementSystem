#include <iostream>
#include <string>
#include "auth/AuthManager.h"
#include "order_management/OrderManager.h"
#include "account_management/AccountManager.h"
#include "market_data/MarketDataManager.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Beautify JSON Output Function
std::string beautifyJson(const std::string& jsonString) {
    try {
        json parsedJson = json::parse(jsonString);
        return parsedJson.dump(4); // Indent with 4 spaces
    } catch (const std::exception& e) {
        std::cerr << "Error while beautifying JSON: " << e.what() << std::endl;
        return jsonString; // Return original string on failure
    }
}

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
    std::cout << "\n";
}

int main() {
    // Step 1: Take API credentials as input
    std::string clientId, clientSecret;
    std::cout << "Enter your Deribit client_id: ";
    std::getline(std::cin, clientId);
    std::cout << "Enter your Deribit client_secret: ";
    std::getline(std::cin, clientSecret);

    // Step 2: Authenticate
    AuthManager authManager(clientId, clientSecret);
    std::string token = authManager.authenticate();

    if (token.empty()) {
        std::cerr << "Authentication Failed!" << std::endl;
        return 1;
    }

    std::cout << "\nAuthentication Successful! Token: " << token << std::endl;

    // Initialize managers
    OrderManager orderManager(token);
    AccountManager accountManager(token);
    MarketDataManager marketDataManager;

    // Step 3: Display menu and handle user input
    int choice;
    while (true) {
        displayMenu();
        std::cin >> choice;
        std::cin.ignore(); // Clear the newline character

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
                std::cout << "Order Placement Response: " << beautifyJson(response) << std::endl;
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
                std::cout << "Modify Order Response: " << beautifyJson(response) << std::endl;
                break;
            }

            case 3: { // Cancel Order
                std::string orderId;
                std::cout << "Enter order ID to cancel: ";
                std::cin >> orderId;

                std::string response = orderManager.cancelOrder(orderId);
                std::cout << "Cancel Order Response: " << beautifyJson(response) << std::endl;
                break;
            }

            case 4: { // Get Account Summary
                std::string response = accountManager.getAccountSummary();
                std::cout << "Account Summary: " << beautifyJson(response) << std::endl;
                break;
            }

            case 5: { // Get Current Positions
                std::string response = accountManager.getPositions();
                std::cout << "Current Positions: " << beautifyJson(response) << std::endl;
                break;
            }

            case 6: { // Get Order Book
                std::string instrument;
                std::cout << "Enter instrument name for order book (e.g., BTC-PERPETUAL): ";
                std::cin >> instrument;

                std::string response = marketDataManager.getOrderBook(instrument);
                std::cout << "Order Book: " << beautifyJson(response) << std::endl;
                break;
            }

            default:
                std::cerr << "Invalid choice. Please try again." << std::endl;
        }
    }

    return 0;
}
