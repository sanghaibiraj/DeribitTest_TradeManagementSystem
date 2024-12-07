#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>

#include "auth/AuthManager.h"
#include "order_management/OrderManager.h"
#include "account_management/AccountManager.h"
#include "market_data/MarketDataManager.h"
#include "WebSocketClient.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Utility Functions
void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void waitForKey() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string beautifyJson(const std::string& jsonString) {
    try {
        json parsedJson = json::parse(jsonString);
        return parsedJson.dump(4); // Indent with 4 spaces
    } catch (const std::exception& e) {
        std::cerr << "Error while beautifying JSON: " << e.what() << std::endl;
        return jsonString;
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
    std::cout << "7. Start WebSocket for Real-Time Data\n";
    std::cout << "8. Exit\n";
    std::cout << "Enter your choice: ";
}

int main() {
    // Take API credentials as input
    std::string clientId, clientSecret;
    std::cout << "Enter your Deribit client_id: ";
    std::getline(std::cin, clientId);
    std::cout << "Enter your Deribit client_secret: ";
    std::getline(std::cin, clientSecret);

    // Authenticate
    AuthManager authManager(clientId, clientSecret);
    std::string token = authManager.authenticate();

    if (token.empty()) {
        std::cerr << "Authentication Failed!" << std::endl;
        return 1;
    }

    std::cout << "Authentication Successful! Token: " << token << std::endl;

    // Initialize managers
    OrderManager orderManager(token);
    AccountManager accountManager(token);
    MarketDataManager marketDataManager;

    // Atomic flag for WebSocket thread control
    std::atomic<bool> keepRunning(true);

    int choice;
    while (true) {
        clearConsole();
        displayMenu();
        std::cin >> choice;
        std::cin.ignore();

        clearConsole();

        if (choice == 8) {
            std::cout << "Exiting the system. Goodbye!" << std::endl;
            break;
        }

        switch (choice) {
            case 1: { // Place Order
                std::string instrument, side;
                double amount, price;
                std::cout << "Enter instrument name (e.g., BTC-PERPETUAL): ";
                std::getline(std::cin, instrument);
                std::cout << "Enter side (buy/sell): ";
                std::getline(std::cin, side);
                std::cout << "Enter amount: ";
                std::cin >> amount;
                std::cout << "Enter price: ";
                std::cin >> price;
                std::cin.ignore();

                std::string response = orderManager.placeOrder(instrument, side, amount, price);
                std::cout << "Order Placement Response:\n" << beautifyJson(response) << std::endl;
                break;
            }

            case 2: { // Modify Order
                std::string orderId;
                double newAmount, newPrice;
                std::cout << "Enter order ID to modify: ";
                std::getline(std::cin, orderId);
                std::cout << "Enter new amount: ";
                std::cin >> newAmount;
                std::cout << "Enter new price: ";
                std::cin >> newPrice;
                std::cin.ignore();

                std::string response = orderManager.modifyOrder(orderId, newAmount, newPrice);
                std::cout << "Modify Order Response:\n" << beautifyJson(response) << std::endl;
                break;
            }

            case 3: { // Cancel Order
                std::string orderId;
                std::cout << "Enter order ID to cancel: ";
                std::getline(std::cin, orderId);

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
                std::getline(std::cin, instrument);

                std::string response = marketDataManager.getOrderBook(instrument);
                std::cout << "Order Book:\n" << beautifyJson(response) << std::endl;
                break;
            }

            case 7: { // Start WebSocket for Real-Time Data
                std::string symbol;
                std::cout << "Enter symbol to subscribe for real-time updates (e.g., BTC-PERPETUAL): ";
                std::getline(std::cin, symbol);

                // Configure WebSocket client
                WebSocketClient::Config wsConfig;
                wsConfig.host = "test.deribit.com";
                wsConfig.port = "443";
                wsConfig.path = "/ws/api/v2";
                wsConfig.connect_timeout = std::chrono::seconds(10);
                wsConfig.read_timeout = std::chrono::seconds(30);

                try {
                    WebSocketClient wsClient(wsConfig);
                    wsClient.connect();

                    // Prepare subscription message
                    json subscribeMessage = {
                        {"jsonrpc", "2.0"},
                        {"method", "public/subscribe"},
                        {"id", 42},
                        {"params", {
                            {"channels", {"book." + symbol + ".100ms"}}
                        }}
                    };

                    // Send subscription message
                    wsClient.send(subscribeMessage.dump());

                    // Start a thread for receiving messages
                    std::thread receiveThread([&wsClient, &keepRunning]() {
                        try {
                            while (keepRunning) {
                                wsClient.receive([](const std::string& message) {
                                    std::cout << "Real-time Data: " 
                                              << beautifyJson(message) 
                                              << std::endl;
                                });
                            }
                        }
                        catch (const std::exception& e) {
                            std::cerr << "WebSocket receive error: " 
                                      << e.what() << std::endl;
                            keepRunning = false;
                        }
                    });

                    // Wait for user to stop
                    std::cout << "Press Enter to stop WebSocket stream..." << std::endl;
                    std::cin.get();

                    // Stop the thread
                    keepRunning = false;
                    receiveThread.join();

                    // Disconnect WebSocket
                    wsClient.disconnect();
                }
                catch (const std::exception& e) {
                    std::cerr << "WebSocket error: " << e.what() << std::endl;
                }
                break;
            }

            default:
                std::cerr << "Invalid choice. Please try again." << std::endl;
        }

        waitForKey();
    }

    return 0;
}