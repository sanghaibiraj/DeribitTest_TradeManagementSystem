#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <limits>

#include "auth/AuthManager.h"              // Handles authentication
#include "order_management/OrderManager.h" // Manages orders
#include "account_management/AccountManager.h" // Retrieves account data
#include "market_data/MarketDataManager.h"     // Fetches market data
#include "WebSocketClient.h"                   // Implements WebSocket communication
#include <nlohmann/json.hpp>                   // JSON parsing and serialization

using json = nlohmann::json;

// --- Utility Functions ---

/**
 * Clears the console screen for a clean user interface.
 * This function detects the platform and executes the appropriate system command.
 */
void clearConsole()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/**
 * Waits for the user to press Enter, pausing execution.
 * Ensures the user has time to read the output before proceeding.
 */
void waitForKey()
{
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/**
 * Beautifies JSON strings for better readability.
 * Attempts to parse the JSON string and format it with indentation.
 * Handles parsing errors gracefully.
 */
std::string beautifyJson(const std::string &jsonString)
{
    try
    {
        json parsedJson = json::parse(jsonString);
        return parsedJson.dump(4); // Indent with 4 spaces for readability
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error while beautifying JSON: " << e.what() << std::endl;
        return jsonString;
    }
}

/**
 * Displays the main menu for the Deribit Trading Management System.
 * Lists all available operations for user interaction.
 */
void displayMenu()
{
    std::cout << "\n=== Deribit Trading System Menu ===\n";
    std::cout << "1. Place Order\n";
    std::cout << "2. Modify Order\n";
    std::cout << "3. Cancel Order\n";
    std::cout << "4. Get All Orders\n";
    std::cout << "5. Get Account Summary\n";
    std::cout << "6. Get Current Positions\n";
    std::cout << "7. Get Order Book\n";
    std::cout << "8. Start WebSocket for Real-Time Data\n";
    std::cout << "9. Exit\n";
    std::cout << "Enter your choice: ";
}

int main()
{
    /*
     * Step 1: Authenticate with Deribit API.
     * The system begins by requesting `client_id` and `client_secret` from the user.
     * These credentials are used to authenticate with the Deribit API and retrieve an access token.
     */
    std::string clientId, clientSecret;
    std::cout << "Enter your Deribit client_id: ";
    std::getline(std::cin, clientId);
    std::cout << "Enter your Deribit client_secret: ";
    std::getline(std::cin, clientSecret);

    AuthManager authManager(clientId, clientSecret);
    std::string token = authManager.authenticate();

    if (token.empty())
    {
        std::cerr << "Authentication Failed! Please check your credentials.\n";
        return 1; // Exit if authentication fails
    }

    std::cout << "Authentication Successful! Token: " << token << "\n";

    /*
     * Step 2: Initialize Managers.
     * The system initializes several managers to handle specific functionalities:
     * - OrderManager: Handles order placement, modification, and cancellation.
     * - AccountManager: Retrieves account summaries and open positions.
     * - MarketDataManager: Fetches order book and market data.
     */
    OrderManager orderManager(token);
    AccountManager accountManager(token);
    MarketDataManager marketDataManager;

    /*
     * Step 3: Prepare for Real-Time Data Streaming.
     * An atomic flag (`keepRunning`) is used to control WebSocket threads for real-time market data.
     */
    std::atomic<bool> keepRunning(true);

    /*
     * Step 4: Main Menu Loop.
     * Displays a menu of options for user interaction.
     * The loop continues until the user selects the exit option.
     */
    int choice;
    while (true)
    {
        clearConsole();
        displayMenu();
        std::cin >> choice;
        std::cin.ignore(); // Clear any leftover input

        clearConsole();

        if (choice == 9) // Exit the program
        {
            std::cout << "Exiting the system. Goodbye!\n";
            break;
        }

        // --- Menu Options ---
        switch (choice)
        {
        case 1: // Place Order
        {
            /*
             * Allows the user to place a new limit order.
             * The user specifies the instrument, side (buy/sell), amount, and price.
             */
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
            std::cout << "Order Placement Response:\n"
                      << beautifyJson(response) << "\n";
            break;
        }

        case 2: // Modify Order
        {
            /*
             * Allows the user to modify an existing order.
             * The user specifies the order ID, new amount, and new price.
             */
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
            std::cout << "Modify Order Response:\n"
                      << beautifyJson(response) << "\n";
            break;
        }

        case 3: // Cancel Order
        {
            /*
             * Allows the user to cancel an active order.
             * The user provides the order ID of the order they wish to cancel.
             */
            std::string orderId;
            std::cout << "Enter order ID to cancel: ";
            std::getline(std::cin, orderId);

            std::string response = orderManager.cancelOrder(orderId);
            std::cout << "Cancel Order Response:\n"
                      << beautifyJson(response) << "\n";
            break;
        }

        case 4: // Get All Orders
        {
            /*
             * Fetches all open orders for a specific instrument.
             * The user provides the instrument name (e.g., BTC-PERPETUAL).
             */
            std::string instrument;
            std::cout << "Enter instrument name (e.g., BTC-PERPETUAL): ";
            std::getline(std::cin, instrument);

            std::string response = orderManager.getAllOrders(instrument);
            if (response.empty())
            {
                std::cerr << "Failed to fetch orders. No response received.\n";
                break;
            }

            try
            {
                auto responseJson = json::parse(response);
                if (responseJson.contains("result") && !responseJson["result"].empty())
                {
                    std::cout << "\n--- Open Orders ---\n";
                    for (const auto &order : responseJson["result"])
                    {
                        std::cout << "Order ID: " << order.value("order_id", "N/A") << "\n";
                        std::cout << "Instrument: " << order.value("instrument_name", "N/A") << "\n";
                        std::cout << "Amount: " << order.value("amount", 0.0) << "\n";
                        std::cout << "Price: " << order.value("price", 0.0) << "\n";
                        std::cout << "Direction: " << order.value("direction", "N/A") << "\n";
                        std::cout << "-----------------------------\n";
                    }
                }
                else
                {
                    std::cout << "No open orders found for instrument: " << instrument << "\n";
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error parsing JSON response: " << e.what() << "\n";
            }
            break;
        }

        case 5: // Get Account Summary
        {
            /*
             * Fetches the account summary, including balance and equity details.
             */
            std::string response = accountManager.getAccountSummary();
            std::cout << "Account Summary:\n"
                      << beautifyJson(response) << "\n";
            break;
        }

        case 6: // Get Current Positions
        {
            /*
             * Fetches all current open positions for the user.
             */
            std::string response = accountManager.getPositions();
            std::cout << "Current Positions:\n"
                      << beautifyJson(response) << "\n";
            break;
        }

        case 7: // Get Order Book
        {
            /*
             * Retrieves the top levels of the order book for a specific instrument.
             * The user provides the instrument name (e.g., BTC-PERPETUAL).
             */
            std::string instrument;
            std::cout << "Enter instrument name for order book (e.g., BTC-PERPETUAL): ";
            std::getline(std::cin, instrument);

            std::string response = marketDataManager.getOrderBook(instrument);
            std::cout << "Order Book:\n"
                      << beautifyJson(response) << "\n";
            break;
        }

        case 8: // Start WebSocket for Real-Time Data
        {
            /*
             * Starts a WebSocket client to stream real-time market data.
             * The user provides the symbol to subscribe to (e.g., BTC-PERPETUAL).
             */
            std::string symbol;
            std::cout << "Enter symbol to subscribe for real-time updates (e.g., BTC-PERPETUAL): ";
            std::getline(std::cin, symbol);

            WebSocketClient::Config wsConfig{"test.deribit.com", "443", "/ws/api/v2"};
            WebSocketClient wsClient(wsConfig);
            wsClient.connect();

            json subscribeMessage = {
                {"jsonrpc", "2.0"},
                {"method", "public/subscribe"},
                {"params", {{"channels", {"book." + symbol + ".100ms"}}}}};

            wsClient.send(subscribeMessage.dump());

            std::thread receiveThread([&wsClient, &keepRunning]()
            {
                while (keepRunning)
                {
                    wsClient.receive([](const std::string &message)
                                    { std::cout << "Real-time Data: " << beautifyJson(message) << "\n"; });
                }
            });

            std::cout << "Press Enter to stop WebSocket stream...\n";
            std::cin.get();
            keepRunning = false;
            receiveThread.join();
            wsClient.disconnect();
            break;
        }

        default:
            std::cerr << "Invalid choice. Please try again.\n";
        }

        waitForKey();
    }

    return 0;
}
