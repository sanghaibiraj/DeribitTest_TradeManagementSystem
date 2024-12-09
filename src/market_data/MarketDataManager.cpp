#include "MarketDataManager.h"
#include <curl/curl.h>
#include <iostream>
#include <nlohmann/json.hpp> // Include for JSON parsing

using json = nlohmann::json; // Alias for JSON library

/**
 * @file MarketDataManager.cpp
 *
 * @brief Implements the MarketDataManager class with added error handling for HTTP requests and JSON parsing.
 *
 * This implementation includes mechanisms to handle network issues, malformed responses, and invalid input.
 */

// Callback function for libcurl to handle HTTP response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userdata) {
    ((std::string*)userdata)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

/**
 * @brief Fetches the order book data for a specific trading instrument, with error handling.
 *
 * @param instrument The name of the trading instrument (e.g., "BTC-PERPETUAL").
 * @return A JSON-formatted string containing the order book data or an error message in case of failure.
 *
 * ### Error Handling:
 * - Validates the instrument name.
 * - Handles network errors, such as timeout or connection issues.
 * - Handles malformed or invalid JSON responses.
 */
std::string MarketDataManager::getOrderBook(const std::string& instrument) {
    if (instrument.empty()) {
        // Return an error message if the instrument name is empty
        return R"({"error": "Instrument name is required"})";
    }

    CURL* curl = curl_easy_init(); // Initialize a CURL handle
    std::string response; // Buffer to store the API response
    std::string errorMessage; // Buffer for storing error messages

    if (!curl) {
        // Return an error message if CURL initialization fails
        return R"({"error": "Failed to initialize CURL"})";
    }

    try {
        // Construct the API request URL
        std::string url = "https://test.deribit.com/api/v2/public/get_order_book?instrument_name=" + instrument;

        // Set CURL options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // Set the API endpoint URL
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // Set the callback for handling response data
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response); // Provide the response buffer to the callback

        // Perform the HTTP request
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            // Handle CURL errors and return an error message
            errorMessage = curl_easy_strerror(res);
            curl_easy_cleanup(curl);
            return R"({"error": "CURL error: )" + errorMessage + R"("})";
        }

        // Clean up the CURL handle
        curl_easy_cleanup(curl);

        // Validate the response content
        if (response.empty()) {
            return R"({"error": "Empty response received from the server"})";
        }

        // Parse the JSON response to validate its structure
        try {
            json parsedResponse = json::parse(response);

            // Check if the response contains an error field
            if (parsedResponse.contains("error")) {
                return R"({"error": ")" + parsedResponse["error"]["message"].get<std::string>() + R"("})";
            }

            // Return the formatted JSON response if no errors are present
            return parsedResponse.dump(4); // Beautify the JSON output with 4-space indentation
        } catch (const json::parse_error& e) {
            // Handle JSON parsing errors
            return R"({"error": "Failed to parse JSON response: )" + std::string(e.what()) + R"("})";
        }

    } catch (const std::exception& e) {
        // Handle general exceptions
        return R"({"error": "An exception occurred: )" + std::string(e.what()) + R"("})";
    }
}
