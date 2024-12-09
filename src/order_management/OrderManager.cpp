#include "OrderManager.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>
#include <fmt/color.h>

// Define color constants for clarity
const auto ERROR_COLOR = fmt::fg(fmt::color::red);
const auto SUCCESS_COLOR = fmt::fg(fmt::color::cyan);
const auto INFO_COLOR = fmt::fg(fmt::color::blue);
const auto HIGHLIGHT_COLOR = fmt::fg(fmt::color::yellow);

using json = nlohmann::json;

/**
 * @file OrderManager.cpp
 * 
 * This file implements the `OrderManager` class, which provides functionalities 
 * to manage trading orders on the Deribit platform. The class supports placing 
 * new orders, modifying existing ones, canceling orders, and fetching open orders 
 * for a specific instrument. Each method communicates with Deribit's API endpoints 
 * using HTTP POST requests via the `libcurl` library.
 */

/**
 * @brief Constructor for the `OrderManager` class.
 * 
 * @param token Access token for authenticating API requests.
 * 
 * The constructor initializes the `OrderManager` instance with a valid access token.
 * This token is used in the `Authorization` header of all API requests.
 */
OrderManager::OrderManager(const std::string& token) : accessToken(token) {}

/**
 * @brief Helper function for handling CURL responses.
 * 
 * @param contents Pointer to the data received in the response.
 * @param size Size of each data chunk.
 * @param nmemb Number of data chunks.
 * @param userdata Pointer to the string buffer where the response will be stored.
 * @return The total size of data processed.
 * 
 * This function appends the received response data to a string buffer for further processing.
 */
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userdata) {
    ((std::string*)userdata)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

/**
 * @brief Places a new limit order.
 * 
 * @param instrument The trading instrument name (e.g., "BTC-PERPETUAL").
 * @param side The side of the order ("buy" or "sell").
 * @param quantity The quantity to trade.
 * @param price The limit price of the order.
 * @return A JSON string containing the API response.
 * 
 * This method sends a POST request to the appropriate endpoint (`buy` or `sell`) based 
 * on the `side` parameter. It constructs a JSON-RPC request body and sends it via CURL. 
 * The API response contains details of the placed order or an error message if the request fails.
 */
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

            // JSON-RPC request body
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
                std::cerr << fmt::format(ERROR_COLOR, "CURL error: {}\n", curl_easy_strerror(res));
            }

            // Cleanup
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        } catch (const std::exception& e) {
            std::cerr << fmt::format(ERROR_COLOR, "Error while placing order: {}\n", e.what());
        }
    }

    return response;
}

/**
 * @brief Modifies an existing order.
 * 
 * @param orderId The unique identifier of the order to modify.
 * @param newQuantity The new quantity for the order.
 * @param newPrice The new price for the order.
 * @return A JSON string containing the API response.
 * 
 * This method sends a POST request to the `edit` endpoint to update the details 
 * of an existing limit order. The API response confirms the modification or 
 * returns an error message.
 */
std::string OrderManager::modifyOrder(const std::string& orderId, double newQuantity, double newPrice) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        try {
            // Prepare the URL
            std::string url = "https://test.deribit.com/api/v2/private/edit";

            // JSON-RPC request body
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
                std::cerr << fmt::format(ERROR_COLOR, "CURL Error: {}\n", curl_easy_strerror(res));
            }

            // Cleanup
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        } catch (const std::exception& e) {
            std::cerr << fmt::format(ERROR_COLOR, "Error while modifying order: {}\n", e.what());
        }
    }

    return response;
}

/**
 * @brief Cancels an active order.
 * 
 * @param orderId The unique identifier of the order to cancel.
 * @return A JSON string containing the API response.
 * 
 * Sends a POST request to the `cancel` endpoint to remove an active order from the order book.
 */
std::string OrderManager::cancelOrder(const std::string& orderId) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        try {
            std::string url = "https://test.deribit.com/api/v2/private/cancel";

            json requestBody = {
                {"jsonrpc", "2.0"},
                {"method", "private/cancel"},
                {"id", 1},
                {"params", {
                    {"order_id", orderId}
                }}
            };

            std::string data = requestBody.dump();

            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
            headers = curl_slist_append(headers, "Content-Type: application/json");

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            CURLcode res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                std::cerr << fmt::format(ERROR_COLOR, "CURL Error: {}\n", curl_easy_strerror(res));
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        } catch (const std::exception& e) {
            std::cerr << fmt::format(ERROR_COLOR, "Error while canceling order: {}\n", e.what());
        }
    }

    return response;
}

/**
 * @brief Fetches all open orders for a specific instrument.
 * 
 * @param instrument The trading instrument name (e.g., "BTC-PERPETUAL").
 * @return A JSON string containing the API response.
 * 
 * This method retrieves a list of all open orders for the specified instrument.
 */
std::string OrderManager::getAllOrders(const std::string& instrument) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (!curl) {
        std::cerr << fmt::format(ERROR_COLOR, "Failed to initialize CURL!\n");
        return "";
    }

    try {
        std::string url = "https://test.deribit.com/api/v2/private/get_open_orders_by_instrument";

        json requestBody = {
            {"jsonrpc", "2.0"},
            {"method", "private/get_open_orders_by_instrument"},
            {"params", {
                {"instrument_name", instrument}
            }}
        };

        std::string data = requestBody.dump();

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << fmt::format(ERROR_COLOR, "CURL Error: {}\n", curl_easy_strerror(res));
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    } catch (const std::exception& e) {
        std::cerr << fmt::format(ERROR_COLOR, "Error while fetching order: {}\n", e.what());
    }

    return response;
}
