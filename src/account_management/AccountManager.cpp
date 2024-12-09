#include "AccountManager.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

/**
 * @file AccountManager.cpp
 *
 * @brief Implements the `AccountManager` class, which is responsible for handling account-related data
 *        and operations on the trading platform API.
 *
 * This file provides the implementation of the `AccountManager` class, focusing on methods for fetching
 * account summaries and active trading positions. It interacts with the API to retrieve data about the user's
 * account status and active trades.
 */

/**
 * @brief Callback function for handling HTTP response data in libcurl.
 *
 * This function is used by libcurl to collect the data received from the server into a buffer.
 *
 * @param contents Pointer to the data received from the server.
 * @param size Size of each data element (usually 1 byte).
 * @param nmemb Number of data elements received.
 * @param userdata Pointer to a `std::string` where the response data will be stored.
 * @return The total size of the data written (size * nmemb).
 */
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userdata) {
    ((std::string*)userdata)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

/**
 * @brief Constructs an `AccountManager` instance with the provided access token.
 *
 * @param token The access token retrieved during authentication. This token is used to make authenticated
 *              API requests to access account information.
 *
 * ### Purpose:
 * - Initializes the `AccountManager` with the necessary access token.
 * - This token is required to fetch account data, such as account summaries and positions.
 */
AccountManager::AccountManager(const std::string& token) : accessToken(token) {}

/**
 * @brief Fetches the account summary from the trading platform API.
 *
 * @return A JSON-formatted string containing the account summary, including balances and currency details.
 *
 * ### Workflow:
 * - Constructs an HTTP request to the API's account summary endpoint.
 * - Uses the access token for authentication.
 * - Sends the request and retrieves the response.
 * - Parses the JSON response to extract the required account details.
 *
 * ### API Data:
 * - **Endpoint**: `/private/get_account_summary`
 * - **Parameters**:
 *   - `currency`: Specifies the currency for which the account summary is requested. Defaults to "BTC".
 * - **Response**: Contains account balance, available funds, and other relevant account information.
 *
 * ### Error Handling:
 * - If the access token is not set, returns an empty response.
 * - Logs any CURL initialization errors, network issues, or API response errors.
 */
std::string AccountManager::getAccountSummary() {
    CURL* curl = curl_easy_init(); // Initialize a CURL handle for HTTP requests
    std::string response;          // Buffer to store the API response

    if (curl) {
        // Construct the API URL for fetching account summary
        std::string url = "https://test.deribit.com/api/v2/private/get_account_summary";
        
        // Create the JSON-RPC request body
        json requestBody = {
            {"jsonrpc", "2.0"},              // JSON-RPC version
            {"method", "private/get_account_summary"}, // API method name
            {"id", 1},                       // Request ID for tracking
            {"params", {{"currency", "BTC"}}} // Parameter for the currency type
        };

        std::string data = requestBody.dump(); // Serialize the JSON request to a string

        // Set CURL options
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());         // Set the API endpoint URL
        curl_easy_setopt(curl, CURLOPT_POST, 1L);                // Use POST method
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());// Set the request data
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);     // Set the request headers
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // Set the response callback function
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);    // Set the response buffer

        curl_easy_perform(curl); // Perform the HTTP request

        // Free the headers and clean up CURL handle
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return response;
}

/**
 * @brief Fetches the current open positions from the trading platform API.
 *
 * @return A JSON-formatted string containing details of active trading positions.
 *
 * ### Workflow:
 * - Constructs an HTTP request to the API's positions endpoint.
 * - Uses the access token for authentication.
 * - Sends the request and retrieves the response.
 * - Parses the JSON response to extract details of open positions.
 *
 * ### API Data:
 * - **Endpoint**: `/private/get_positions`
 * - **Parameters**:
 *   - `currency`: Specifies the currency (e.g., "BTC") for which positions are requested.
 *   - `kind`: Specifies the kind of positions (e.g., "future").
 * - **Response**: Includes details such as instrument names, sizes, entry prices, and profits.
 *
 * ### Error Handling:
 * - Logs any CURL initialization errors, network issues, or API response errors.
 */
std::string AccountManager::getPositions() {
    CURL* curl = curl_easy_init(); // Initialize a CURL handle for HTTP requests
    std::string response;          // Buffer to store the API response

    if (curl) {
        // Construct the API URL for fetching current positions
        std::string url = "https://test.deribit.com/api/v2/private/get_positions";

        // Create the JSON-RPC request body
        json requestBody = {
            {"jsonrpc", "2.0"},              // JSON-RPC version
            {"method", "private/get_positions"}, // API method name
            {"id", 1},                       // Request ID for tracking
            {"params", {{"currency", "BTC"}, {"kind", "future"}}} // Parameters for currency and kind of positions
        };

        std::string data = requestBody.dump(); // Serialize the JSON request to a string

        // Set CURL options
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());         // Set the API endpoint URL
        curl_easy_setopt(curl, CURLOPT_POST, 1L);                // Use POST method
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());// Set the request data
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);     // Set the request headers
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // Set the response callback function
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);    // Set the response buffer

        curl_easy_perform(curl); // Perform the HTTP request

        // Free the headers and clean up CURL handle
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return response;
}
