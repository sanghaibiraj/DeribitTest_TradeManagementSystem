#include <iostream>
#include "auth/AuthManager.h"
#include "order_management/OrderManager.h"

int main() {
    // Replace with your Deribit Test API credentials
    const std::string clientId = "Oo2qw6_G";
    const std::string clientSecret = "ubP2t06RGlV1X4onUoao7q1gdF-aSsZfLycwFfz0-YQ";

    AuthManager authManager(clientId, clientSecret);
    std::string token = authManager.authenticate();

    if (!token.empty()) {
        std::cout << "Authentication Successful! Token: " << token << std::endl;

        // Initialize Order Manager
        OrderManager orderManager(token);

        // Example calculation
        double contractSize = 10.0; // USD per contract
        double price = 30000.0;
        double quantity = contractSize / price; // Rounds to the nearest whole contract
        std::string response = orderManager.placeOrder("BTC-PERPETUAL", "buy", 10.0, 30000.0);

        // // Place an order
        // std::string response = orderManager.placeOrder("BTC-PERPETUAL", "buy", 1.0, 30000.0);
        std::cout << "Order Placement Response: " << response << std::endl;

    } else {
        std::cerr << "Authentication Failed!" << std::endl;
    }

    return 0;
}
