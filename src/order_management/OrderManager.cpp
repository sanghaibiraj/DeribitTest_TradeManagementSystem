#include "OrderManager.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>

using json = nlohmann::json;

OrderManager::OrderManager(const std::string& token) : accessToken(token) {}

// Helper function for CURL response handling
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userdata) {
    ((std::string*)userdata)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Place Order
std::string OrderManager::placeOrder(const std::string& instrument, const std::string& side, double quantity, double price) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        try {
            // Prepare the URL
            std::string url = "https://test.deribit.com/api/v2/private/buy";
            if (side == "sell") {
                url = "https://test.deribit.com/api/v2/private/sell";
            }

            // JSON-RPC style request body
            json requestBody = {
                {"jsonrpc", "2.0"},
                {"method", side == "buy" ? "private/buy" : "private/sell"},
                {"id", 1},
                {"params", {
                    {"instrument_name", instrument},
                    {"amount", quantity},
                    {"type", "limit"},
                    {"price", price}
                }}
            };

            std::string data = requestBody.dump();
            // std::cout << "Place Order Request Data: " << data << std::endl;

            // Set CURL options
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
            headers = curl_slist_append(headers, "Content-Type: application/json");

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            // Perform the request
            CURLcode res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                std::cerr << "CURL Error: " << curl_easy_strerror(res) << std::endl;
            } 
            // else {
                // std::cout << "Place Order Response: " << response << std::endl;
            // }

            // Cleanup
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        } catch (const std::exception& e) {
            std::cerr << "Error while placing order: " << e.what() << std::endl;
        }
    }

    return response;
}

// modify order
std::string OrderManager::modifyOrder(const std::string& orderId, double newQuantity, double newPrice) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        try {
            // Prepare the URL
            std::string url = "https://test.deribit.com/api/v2/private/edit";

            // JSON-RPC style request body
            json requestBody = {
                {"jsonrpc", "2.0"},
                {"method", "private/edit"},
                {"id", 1},
                {"params", {
                    {"order_id", orderId},
                    {"amount", newQuantity},
                    {"price", newPrice}
                }}
            };

            std::string data = requestBody.dump();
            // std::cout << "Modify Order Request Data: " << data << std::endl;

            // Set CURL options
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
            headers = curl_slist_append(headers, "Content-Type: application/json");

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            // Perform the request
            CURLcode res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                std::cerr << "CURL Error: " << curl_easy_strerror(res) << std::endl;
            } 
            // else {
            //     std::cout << "Modify Order Response: " << response << std::endl;
            // }

            // Cleanup
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        } catch (const std::exception& e) {
            std::cerr << "Error while modifying order: " << e.what() << std::endl;
        }
    }

    return response;
}


// Cancel Order
std::string OrderManager::cancelOrder(const std::string& orderId) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        try {
            // Prepare the URL
            std::string url = "https://test.deribit.com/api/v2/private/cancel";

            // JSON-RPC style request body
            json requestBody = {
                {"jsonrpc", "2.0"},
                {"method", "private/cancel"},
                {"id", 1},
                {"params", {
                    {"order_id", orderId}
                }}
            };

            std::string data = requestBody.dump();
            // std::cout << "Cancel Order Request Data: " << data << std::endl;

            // Set CURL options
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
            headers = curl_slist_append(headers, "Content-Type: application/json");

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            // Perform the request
            CURLcode res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                std::cerr << "CURL Error: " << curl_easy_strerror(res) << std::endl;
            } 
            // else {
            //     std::cout << "Cancel Order Response: " << response << std::endl;
            // }

            // Cleanup
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        } catch (const std::exception& e) {
            std::cerr << "Error while canceling order: " << e.what() << std::endl;
        }
    }

    return response;
}

std::string OrderManager::getAllOrders(const std::string& instrument) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (!curl) {
        std::cerr << "Failed to initialize CURL!" << std::endl;
        return "";
    }

    try {
        // Deribit API endpoint
        std::string url = "https://test.deribit.com/api/v2/private/get_open_orders_by_instrument";

        // Validate access token
        if (accessToken.empty()) {
            std::cerr << "Access token is empty! Please authenticate first." << std::endl;
            curl_easy_cleanup(curl);
            return "";
        }

        // JSON-RPC request body
        json requestBody = {
            {"jsonrpc", "2.0"},
            {"method", "private/get_open_orders_by_instrument"},
            // {"id", 1},
            {"params", {
                {"instrument_name", instrument}
            }}
        };

        std::string data = requestBody.dump();

        // Print the request for debugging
        // std::cout << "Request Sent: " << data << std::endl;

        // Set CURL options
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "CURL Error: " << curl_easy_strerror(res) << std::endl;
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            return "";
        }

        // Print the raw API response
        // std::cout << "Raw API Response: " << response << std::endl;

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

    } catch (const std::exception& e) {
        std::cerr << "Error during fetching orders: " << e.what() << std::endl;
    }

    return response;
}
