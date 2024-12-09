#include "AuthManager.h"
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <sstream>
#include <fmt/color.h>

// Define color constants for clarity
const auto ERROR_COLOR = fmt::fg(fmt::color::red);
const auto SUCCESS_COLOR = fmt::fg(fmt::color::cyan);
const auto INFO_COLOR = fmt::fg(fmt::color::blue);
const auto HIGHLIGHT_COLOR = fmt::fg(fmt::color::yellow);

using json = nlohmann::json;

/**
 * @file AuthManager.cpp
 *
 * @brief Implements the `AuthManager` class, handling user authentication with the trading platform API.
 *
 * This file provides the implementation of the `AuthManager` class, focusing on the `authenticate` method.
 * It handles the process of authenticating the client using their credentials and retrieving an access token.
 */

/**
 * @brief Constructs an `AuthManager` instance with the provided client ID and client secret.
 *
 * @param clientId The client ID provided by the trading platform.
 * @param clientSecret The client secret associated with the client ID.
 *
 * ### Workflow:
 * - Initializes the `AuthManager` object with the necessary credentials for authentication.
 * - These credentials are used during the `authenticate` method to exchange for an access token.
 */
AuthManager::AuthManager(const std::string& clientId, const std::string& clientSecret)
    : clientId(clientId), clientSecret(clientSecret) {}

/**
 * @brief Callback function for libcurl to handle HTTP response data.
 *
 * This function appends the data received from the server to the provided buffer.
 *
 * @param contents Pointer to the data received from the server.
 * @param size Size of each data element (usually 1).
 * @param nmemb Number of data elements received.
 * @param userdata Pointer to the buffer where the response should be stored.
 * @return The total size of the data processed (size * nmemb).
 */
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userdata) {
    ((std::string*)userdata)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

/**
 * @brief Authenticates with the trading platform and retrieves an access token.
 *
 * @return The access token as a `std::string`, or an empty string if authentication fails.
 *
 * ### Workflow:
 * 1. Initialize a CURL handle for HTTP requests.
 * 2. Construct the authentication request using the client ID and client secret.
 * 3. Send the request to the API's `/auth` endpoint using libcurl.
 * 4. Parse the JSON response to extract the access token.
 * 5. Store the access token for future use and return it.
 *
 * ### Error Handling:
 * - Handles libcurl initialization errors.
 * - Catches and reports network errors during the HTTP request.
 * - Validates the API response and checks for errors in the returned JSON.
 *
 * ### Example Usage:
 * ```
 * AuthManager authManager("client_id", "client_secret");
 * std::string token = authManager.authenticate();
 *
 * if (!token.empty()) {
 *     std::cout << "Authentication successful! Token: " << token << std::endl;
 * } else {
 *     std::cerr << "Authentication failed." << std::endl;
 * }
 * ```
 */
std::string AuthManager::authenticate() {
    CURL* curl = nullptr; // CURL handle for making HTTP requests
    CURLcode res;         // CURL result code
    std::string response; // Buffer to store the API response

    // Initialize CURL
    curl = curl_easy_init();
    if (!curl) {
        std::cerr << fmt::format(ERROR_COLOR, "Failed to initialize CURL\n");
        return "";
    }

    try {
        // Construct the API endpoint URL
        std::string url = "https://test.deribit.com/api/v2/public/auth";

        // Prepare the JSON-RPC request body
        json requestBody = {
            {"jsonrpc", "2.0"},              // JSON-RPC version
            {"method", "public/auth"},       // API method name
            {"id", 1},                       // Request ID for tracking
            {"params", {
                {"grant_type", "client_credentials"}, // Authentication grant type
                {"client_id", clientId},             // Client ID
                {"client_secret", clientSecret},     // Client secret
                {"scope", "trade:read_write"}        // Access scope for the token
            }}
        };

        // Serialize the JSON request body to a string
        std::string data = requestBody.dump();

        // Configure CURL options
        struct curl_slist* headers = nullptr; // Headers for the request
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());         // Set the API endpoint URL
        curl_easy_setopt(curl, CURLOPT_POST, 1L);                // Set the request method to POST
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());// Set the POST data
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);     // Add headers
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // Set the response callback
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);    // Set the response buffer

        // Perform the HTTP request
        res = curl_easy_perform(curl);

        // Clean up headers
        curl_slist_free_all(headers);

        // Handle CURL errors
        if (res != CURLE_OK) {
            std::cerr << fmt::format(ERROR_COLOR, "CURL Error: {}\n", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            return "";
        }

        // Clean up the CURL handle
        curl_easy_cleanup(curl);

        // Parse the API response
        json jsonResponse = json::parse(response);

        // Check for errors in the API response
        if (jsonResponse.contains("error")) {
            std::cerr << fmt::format(ERROR_COLOR, "API Error: {}\n", jsonResponse["error"]["message"]);
            return "";
        }

        // Extract and return the access token
        if (jsonResponse.contains("result") && jsonResponse["result"].contains("access_token")) {
            accessToken = jsonResponse["result"]["access_token"];
            return accessToken;
        } else {
            std::cerr << fmt::format(ERROR_COLOR, "Unexpected API Response Format\n");
            return "";
        }
    } catch (const std::exception& e) {
        // Handle exceptions during the process
        if (curl) {
            curl_easy_cleanup(curl);
        }
        std::cerr << fmt::format(ERROR_COLOR, "Error: {}\n", e.what());
        return "";
    }
}
