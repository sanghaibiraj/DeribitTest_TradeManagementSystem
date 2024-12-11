#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <limits>

#include "auth/AuthManager.h"                  // Handles authentication
#include "order_management/OrderManager.h"     // Manages orders
#include "account_management/AccountManager.h" // Retrieves account data
#include "market_data/MarketDataManager.h"     // Fetches market data
#include "WebSocketClient.h"                   // Implements WebSocket communication
#include <nlohmann/json.hpp>                   // JSON parsing and serialization

#include <fmt/color.h>

// Define color constants for clarity
const auto ERROR_COLOR = fmt::fg(fmt::color::red);
const auto SUCCESS_COLOR = fmt::fg(fmt::color::cyan);
const auto INFO_COLOR = fmt::fg(fmt::color::blue);
const auto HIGHLIGHT_COLOR = fmt::fg(fmt::color::yellow);

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
        // Parse the input JSON string
        json parsedJson = json::parse(jsonString);
        // Format the beautified JSON with the highlight color
        return fmt::format(SUCCESS_COLOR, "{}\n", parsedJson.dump(4));
    }
    catch (const std::exception &e)
    {
        // Return an error message in error color
        return fmt::format(ERROR_COLOR, "Error while beautifying JSON: {}\n", e.what());
    }
}

/**
 * Displays the main menu for the Deribit Trading Management System.
 * Lists all available operations for user interaction.
 */
void displayMenu()
{
    fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::cyan), "--- Deribit Trading System Menu ---\n");
    fmt::print(fmt::fg(fmt::color::yellow), "1. Place Order\n");
    fmt::print(fmt::fg(fmt::color::yellow), "2. Modify Order\n");
    fmt::print(fmt::fg(fmt::color::yellow), "3. Cancel Order\n");
    fmt::print(fmt::fg(fmt::color::yellow), "4. Get All Orders\n");
    fmt::print(fmt::fg(fmt::color::yellow), "5. Get Account Summary\n");
    fmt::print(fmt::fg(fmt::color::yellow), "6. Get Current Positions\n");
    fmt::print(fmt::fg(fmt::color::yellow), "7. Get Order Book\n");
    fmt::print(fmt::fg(fmt::color::yellow), "8. Start WebSocket for Real-Time Data\n");
    fmt::print(fmt::fg(fmt::color::yellow), "9. Exit\n");
    fmt::print(fmt::fg(fmt::color::green), "Enter your choice: ");
}

int main()
{
    /*
     * Step 1: Authenticate with Deribit API.
     * The system begins by requesting `client_id` and `client_secret` from the user.
     * These credentials are used to authenticate with the Deribit API and retrieve an access token.
     */
    std::string clientId, clientSecret;
    std::cout << fmt::format(INFO_COLOR, "Enter your Deribit client_id: ");
    std::getline(std::cin, clientId);
    std::cout << fmt::format(INFO_COLOR, "Enter your Deribit client_secret: ");
    std::getline(std::cin, clientSecret);

    AuthManager authManager(clientId, clientSecret);
    std::string token = authManager.authenticate();

    if (token.empty())
    {
        std::cerr << fmt::format(ERROR_COLOR, "Authentication Failed! Please check your credentials.\n");
        return 1; // Exit if authentication fails
    }

    std::cout << fmt::format(SUCCESS_COLOR, "Authentication Successful! Token: {}\n", token);

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
            std::cout << fmt::format(HIGHLIGHT_COLOR, "Exiting the system. Goodbye!\n");
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
            std::cout << fmt::format(HIGHLIGHT_COLOR, "Enter instrument name (e.g., BTC-PERPETUAL): ");
            std::getline(std::cin, instrument);
            std::cout << fmt::format(HIGHLIGHT_COLOR, "Enter side (buy/sell): ");
            std::getline(std::cin, side);
            std::cout << fmt::format(HIGHLIGHT_COLOR, "Enter amount: ");
            std::cin >> amount;
            std::cout << fmt::format(HIGHLIGHT_COLOR, "Enter price: ");
            std::cin >> price;
            std::cin.ignore();

            auto start = std::chrono::high_resolution_clock::now();
            std::string response = orderManager.placeOrder(instrument, side, amount, price);
            auto end = std::chrono::high_resolution_clock::now();

            std::cout << fmt::format(SUCCESS_COLOR, "Order Placement Response:\n")
                      << beautifyJson(response) << "\n";
            fmt::print(INFO_COLOR, "Order Placement Latency: {} ms\n",
                       std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
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
            std::cout << fmt::format(HIGHLIGHT_COLOR, "Enter order ID to modify: ");
            std::getline(std::cin, orderId);
            std::cout << fmt::format(HIGHLIGHT_COLOR, "Enter new amount: ");
            std::cin >> newAmount;
            std::cout << fmt::format(HIGHLIGHT_COLOR, "Enter new price: ");
            std::cin >> newPrice;
            std::cin.ignore();

            auto start = std::chrono::high_resolution_clock::now();
            std::string response = orderManager.modifyOrder(orderId, newAmount, newPrice);
            auto end = std::chrono::high_resolution_clock::now();

            std::cout << fmt::format(SUCCESS_COLOR, "Modify Order Response:\n")
                      << beautifyJson(response) << "\n";
            fmt::print(INFO_COLOR, "Modify Order Latency: {} ms\n",
                       std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
            break;
        }

        case 3: // Cancel Order
        {
            /*
             * Allows the user to cancel an active order.
             * The user provides the order ID of the order they wish to cancel.
             */
            std::string orderId;
            std::cout << fmt::format(HIGHLIGHT_COLOR, "Enter order ID to cancel: ");
            std::getline(std::cin, orderId);

            auto start = std::chrono::high_resolution_clock::now();
            std::string response = orderManager.cancelOrder(orderId);
            auto end = std::chrono::high_resolution_clock::now();

            std::cout << fmt::format(SUCCESS_COLOR, "Cancel Order Response:\n")
                      << beautifyJson(response) << "\n";
            fmt::print(INFO_COLOR, "Cancel Order Latency: {} ms\n",
                       std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
            break;
        }

        case 4: // Get All Orders
        {
            /*
             * Fetches all open orders for a specific instrument.
             * The user provides the instrument name (e.g., BTC-PERPETUAL).
             */
            std::string instrument;
            std::cout << fmt::format(HIGHLIGHT_COLOR, "Enter instrument name (e.g., BTC-PERPETUAL): ");
            std::getline(std::cin, instrument);

            auto start = std::chrono::high_resolution_clock::now();
            std::string response = orderManager.getAllOrders(instrument);
            auto end = std::chrono::high_resolution_clock::now();

            if (response.empty())
            {
                std::cerr << fmt::format(ERROR_COLOR, "Failed to fetch orders. No response received.\n");
                break;
            }

            try
            {
                auto responseJson = json::parse(response);
                if (responseJson.contains("result") && !responseJson["result"].empty())
                {
                    fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::cyan), "\n--- Open Orders ---\n");
                    for (const auto &order : responseJson["result"])
                    {
                        fmt::print(fmt::fg(fmt::color::yellow), "Order ID: {}\n", order.value("order_id", "N/A"));
                        fmt::print(fmt::fg(fmt::color::green), "Instrument: {}\n", order.value("instrument_name", "N/A"));
                        fmt::print(fmt::fg(fmt::color::blue), "Amount: {}\n", order.value("amount", 0.0));
                        fmt::print(fmt::fg(fmt::color::magenta), "Price: {}\n", order.value("price", 0.0));
                        fmt::print(fmt::fg(fmt::color::red), "Direction: {}\n", order.value("direction", "N/A"));
                        fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::white), "-----------------------------\n");
                    }
                }
                else
                {
                    fmt::print(fmt::fg(fmt::color::red), "No open orders found for instrument: {}\n", instrument);
                }
            }
            catch (const std::exception &e)
            {
                fmt::print(fmt::fg(fmt::color::red), "Error parsing JSON response: {}\n", e.what());
            }

            fmt::print(INFO_COLOR, "Order Fetch Latency: {} ms\n",
                       std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
            break;
        }

        case 5: // Get Account Summary
        {
            /*
             * Fetches the account summary, including balance and equity details.
             */
            auto start = std::chrono::high_resolution_clock::now();
            std::string response = accountManager.getAccountSummary();
            auto end = std::chrono::high_resolution_clock::now();

            std::cout << fmt::format(SUCCESS_COLOR, "Account Summary:\n")
                      << beautifyJson(response) << "\n";
            fmt::print(INFO_COLOR, "Account Summary Latency: {} ms\n",
                       std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
            break;
        }

        case 6: // Get Current Positions
        {
            /*
             * Fetches all current open positions for the user.
             */
            auto start = std::chrono::high_resolution_clock::now();
            std::string response = accountManager.getPositions();
            auto end = std::chrono::high_resolution_clock::now();

            std::cout << fmt::format(SUCCESS_COLOR, "Current Positions:\n")
                      << beautifyJson(response) << "\n";
            fmt::print(INFO_COLOR, "Position Fetch Latency: {} ms\n",
                       std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
            break;
        }

        case 7: // Get Order Book
        {
            /*
             * Retrieves the top levels of the order book for a specific instrument.
             * The user provides the instrument name (e.g., BTC-PERPETUAL).
             */
            std::string instrument;
            std::cout << fmt::format(HIGHLIGHT_COLOR, "Enter instrument name for order book (e.g., BTC-PERPETUAL): ");
            std::getline(std::cin, instrument);

            auto start = std::chrono::high_resolution_clock::now();
            std::string response = marketDataManager.getOrderBook(instrument);
            auto end = std::chrono::high_resolution_clock::now();

            std::cout << fmt::format(SUCCESS_COLOR, "Order Book:\n")
                      << beautifyJson(response) << "\n";
            fmt::print(INFO_COLOR, "Order Book Fetch Latency: {} ms\n",
                       std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
            break;
        }

        case 8: // Start WebSocket for Real-Time Data
        {
            /*
             * Starts a WebSocket client to stream real-time market data.
             * The user provides the symbol to subscribe to (e.g., BTC-PERPETUAL).
             */
            std::string symbol;
            std::cout << fmt::format(HIGHLIGHT_COLOR, "Enter symbol to subscribe for real-time updates (e.g., BTC-PERPETUAL): ");
            std::getline(std::cin, symbol);

            WebSocketClient::Config wsConfig{"test.deribit.com", "443", "/ws/api/v2"};
            WebSocketClient wsClient(wsConfig);

            auto start = std::chrono::high_resolution_clock::now();
            wsClient.connect();
            auto end = std::chrono::high_resolution_clock::now();

            fmt::print(SUCCESS_COLOR, "WebSocket Connected!\n");
            fmt::print(INFO_COLOR, "WebSocket Connection Latency: {} ms\n",
                       std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

            json subscribeMessage = {
                {"jsonrpc", "2.0"},
                {"method", "public/subscribe"},
                {"params", {{"channels", {"book." + symbol + ".100ms"}}}}};

            wsClient.send(subscribeMessage.dump());

            std::thread receiveThread([&wsClient, &keepRunning](){
                while (keepRunning)
                {
                    wsClient.receive([](const std::string &message)
                                    { std::cout << fmt::format(SUCCESS_COLOR, "Real-time Data: {}\n", beautifyJson(message)); });
                } });

            std::cout << fmt::format(INFO_COLOR, "Press Enter to stop WebSocket stream...\n");
            std::cin.get();
            keepRunning = false;
            receiveThread.join();
            wsClient.disconnect();
            break;
        }

        default:
            std::cerr << fmt::format(ERROR_COLOR, "Invalid choice. Please try again.\n");
        }

        waitForKey();
    }

    return 0;
}
