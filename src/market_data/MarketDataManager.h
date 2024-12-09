#ifndef MARKET_DATA_MANAGER_H
#define MARKET_DATA_MANAGER_H

#include <string>

/**
 * @file MarketDataManager.h
 *
 * @brief Provides an interface for fetching real-time market data from a trading platform.
 *
 * The `MarketDataManager` class is designed to interact with the Deribit API or a similar trading platform
 * to fetch and manage market data such as order books. The functionality is focused on retrieving real-time
 * snapshots of the market for specific instruments.
 *
 * ### Key Responsibilities:
 * - Fetch market data for specified trading instruments.
 * - Provide methods to interact with APIs for order book retrieval.
 *
 * This class acts as an abstraction layer, simplifying the process of fetching market data by encapsulating
 * the API request and response handling logic.
 */

/**
 * @class MarketDataManager
 *
 * @brief A manager class to fetch market data, such as order books, from trading platforms.
 *
 * ### Responsibilities:
 * - Handles API calls to retrieve order book data for a specific trading instrument.
 * - Provides a simple interface for market data retrieval.
 *
 * ### Usage:
 * This class is designed to be used as part of a larger trading system where market data
 * is required for decision-making or analytics.
 */
class MarketDataManager {
public:
    /**
     * @brief Fetches the order book data for a specific trading instrument.
     *
     * @param instrument The name of the trading instrument (e.g., "BTC-PERPETUAL").
     * @return A JSON-formatted string containing the order book data, including
     *         information about bids, asks, and other market details.
     *
     * ### Example:
     * ```
     * MarketDataManager manager;
     * std::string orderBook = manager.getOrderBook("BTC-PERPETUAL");
     * std::cout << orderBook << std::endl;
     * ```
     *
     * ### Responsibilities:
     * - Constructs a request to fetch market data for the specified instrument.
     * - Handles the parsing of the response into a readable JSON format.
     *
     * ### Possible Enhancements:
     * - Support for real-time updates using WebSocket instead of HTTP requests.
     * - Adding methods for more granular data, such as trade history or price trends.
     */
    std::string getOrderBook(const std::string& instrument);
};

#endif // MARKET_DATA_MANAGER_H
