#include "AuthManager.h"
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

AuthManager::AuthManager(const std::string& clientId, const std::string& clientSecret)
    : clientId(clientId), clientSecret(clientSecret) {}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userdata) {
    ((std::string*)userdata)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string AuthManager::authenticate() {
    CURL* curl = nullptr;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return "";
    }

    try {
        // Prepare the URL
        std::string url = "https://test.deribit.com/api/v2/public/auth";
        
        // Create JSON-RPC request body
        json requestBody = {
            {"jsonrpc", "2.0"},
            {"method", "public/auth"},
            {"id", 1},
            {"params", {
                {"grant_type", "client_credentials"},
                {"client_id", clientId},
                {"client_secret", clientSecret},
                {"scope", "trade:read_write"} // Ensure this matches exactly
            }}
        };

        std::string data = requestBody.dump();
        // std::cout << "Request Data: " << data << std::endl;

        // Set CURL options
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the request
        res = curl_easy_perform(curl);
        
        // Free headers
        curl_slist_free_all(headers);

        // Always cleanup CURL handle
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "CURL Error: " << curl_easy_strerror(res) << std::endl;
            return "";
        }

        // std::cout << "API Response: " << response << std::endl;

        // Parse JSON response
        json jsonResponse = json::parse(response);
        
        if (jsonResponse.contains("error")) {
            std::cerr << "API Error: " << jsonResponse["error"]["message"] << std::endl;
            return "";
        }

        if (jsonResponse.contains("result") && jsonResponse["result"].contains("access_token")) {
            accessToken = jsonResponse["result"]["access_token"];
            // std::cout << "Access Token: " << accessToken << std::endl;
            return accessToken;
        } else {
            std::cerr << "Unexpected API Response Format" << std::endl;
            return "";
        }
    } 
    catch (const std::exception& e) {
        // Ensure CURL is cleaned up in case of exception
        if (curl) {
            curl_easy_cleanup(curl);
        }
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }
}
