#include "AccountManager.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userdata) {
    ((std::string*)userdata)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

AccountManager::AccountManager(const std::string& token) : accessToken(token) {}

// Get Account Summary
std::string AccountManager::getAccountSummary() {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        std::string url = "https://test.deribit.com/api/v2/private/get_account_summary";
        json requestBody = {
            {"jsonrpc", "2.0"},
            {"method", "private/get_account_summary"},
            {"id", 1},
            {"params", {{"currency", "BTC"}}}
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

        curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return response;
}

// Get Positions
std::string AccountManager::getPositions() {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        std::string url = "https://test.deribit.com/api/v2/private/get_positions";
        json requestBody = {
            {"jsonrpc", "2.0"},
            {"method", "private/get_positions"},
            {"id", 1},
            {"params", {{"currency", "BTC"}, {"kind", "future"}}}
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

        curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return response;
}
