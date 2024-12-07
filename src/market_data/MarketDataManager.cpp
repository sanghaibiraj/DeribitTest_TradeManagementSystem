#include "MarketDataManager.h"
#include <curl/curl.h>
#include <iostream>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userdata) {
    ((std::string*)userdata)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Get Order Book
std::string MarketDataManager::getOrderBook(const std::string& instrument) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        std::string url = "https://test.deribit.com/api/v2/public/get_order_book?instrument_name=" + instrument;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    return response;
}
