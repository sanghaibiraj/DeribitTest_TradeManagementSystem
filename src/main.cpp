#include <iostream>
#include "auth/AuthManager.h"
#include "order_management/OrderManager.h"
#include "account_management/AccountManager.h"
#include "market_data/MarketDataManager.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

int main() {
    // Replace these with your Deribit testnet credentials
    const std::string clientId = "Oo2qw6_G";
    const std::string clientSecret = "ubP2t06RGlV1X4onUoao7q1gdF-aSsZfLycwFfz0-YQ";

    // Step 1: Authenticate
    AuthManager authManager(clientId, clientSecret);
    std::string token = authManager.authenticate();

    if (token.empty()) {
        std::cerr << "Authentication Failed!" << std::endl;
        return 1;
    }

    std::cout << "Authentication Successful! Token: " << token << std::endl;

    // Step 2: Initialize OrderManager with access token
    OrderManager orderManager(token);

    // Step 3: Place an order
    std::string placeOrderResponse = orderManager.placeOrder("BTC-PERPETUAL", "buy", 10, 30000.0);
    std::cout << "Order Placement Response: " << placeOrderResponse << std::endl;

    // Step 4: Extract order_id from the response
    std::string orderId;
    try {
        auto responseJson = json::parse(placeOrderResponse);
        if (responseJson.contains("result") && responseJson["result"].contains("order") && responseJson["result"]["order"].contains("order_id")) {
            orderId = responseJson["result"]["order"]["order_id"];
            std::cout << "Order ID: " << orderId << std::endl;
        } else {
            std::cerr << "Failed to extract order_id from the response." << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "JSON Parsing Error: " << e.what() << std::endl;
        return 1;
    }

    // Step 5: Modify the order
    std::string modifyOrderResponse = orderManager.modifyOrder(orderId, 20, 31000.0);
    std::cout << "Modify Order Response: " << modifyOrderResponse << std::endl;

    // Step 6: Cancel the order
    std::string cancelOrderResponse = orderManager.cancelOrder(orderId);
    std::cout << "Cancel Order Response: " << cancelOrderResponse << std::endl;

    // Use AccountManager
    AccountManager accountManager(token);
    std::cout << "Account Summary: " << accountManager.getAccountSummary() << std::endl;
    std::cout << "Positions: " << accountManager.getPositions() << std::endl;

    // Use MarketDataManager
    MarketDataManager marketDataManager;
    std::cout << "Order Book: " << marketDataManager.getOrderBook("BTC-PERPETUAL") << std::endl;

    return 0;
}
