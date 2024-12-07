#ifndef MARKET_DATA_MANAGER_H
#define MARKET_DATA_MANAGER_H

#include <string>

class MarketDataManager {
public:
    std::string getOrderBook(const std::string& instrument);
};

#endif // MARKET_DATA_MANAGER_H
