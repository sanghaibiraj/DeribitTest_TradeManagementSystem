#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include <string>

/**
 * @class OrderManager
 * 
 * The OrderManager class provides an interface for managing trading orders 
 * on the Deribit platform. It encapsulates functionalities for placing, 
 * modifying, canceling, and retrieving orders, ensuring clean and modular 
 * interaction with the trading API.
 * 
 * This class uses the Deribit API endpoints to perform order-related 
 * operations. Each method constructs and sends a specific API request 
 * to interact with the trading system and returns the API response as 
 * a JSON string.
 * 
 * @note Requires a valid access token for API authentication.
 */
class OrderManager {
public:
    /**
     * @brief Constructor for the OrderManager class.
     * 
     * @param accessToken A valid access token obtained from the Deribit API.
     * 
     * The access token is required for authentication with the API. 
     * It must be provided when instantiating the OrderManager.
     */
    OrderManager(const std::string& accessToken);

    /**
     * @brief Places a new limit order.
     * 
     * @param instrument The trading instrument name (e.g., "BTC-PERPETUAL").
     * @param side The side of the order ("buy" or "sell").
     * @param quantity The quantity of the instrument to trade.
     * @param price The limit price for the order.
     * @return A JSON string containing the API response.
     * 
     * This method sends a request to the API to create a new limit order.
     * It requires the user to specify the trading instrument, order side, 
     * quantity, and price. The response contains the order details or an 
     * error message if the request fails.
     */
    std::string placeOrder(const std::string& instrument, const std::string& side, double quantity, double price);

    /**
     * @brief Modifies an existing order.
     * 
     * @param orderId The unique identifier of the order to modify.
     * @param newQuantity The new quantity for the order.
     * @param newPrice The new price for the order.
     * @return A JSON string containing the API response.
     * 
     * This method allows the user to update the quantity and price of an 
     * existing limit order. The order ID must be provided to identify 
     * the order to be modified.
     */
    std::string modifyOrder(const std::string& orderId, double newQuantity, double newPrice);

    /**
     * @brief Cancels an active order.
     * 
     * @param orderId The unique identifier of the order to cancel.
     * @return A JSON string containing the API response.
     * 
     * This method sends a request to cancel an active order identified 
     * by its order ID. The response includes the status of the cancellation 
     * or an error message if the cancellation fails.
     */
    std::string cancelOrder(const std::string& orderId);

    /**
     * @brief Retrieves all open orders for a specific instrument.
     * 
     * @param instrument The trading instrument name (e.g., "BTC-PERPETUAL").
     * @return A JSON string containing the API response.
     * 
     * This method fetches a list of all open orders for the specified 
     * trading instrument. It is useful for monitoring the current order 
     * status and identifying active trades.
     */
    std::string getAllOrders(const std::string& instrument);

private:
    /**
     * @brief The access token used for API authentication.
     * 
     * This token is required for all API requests and is passed as a 
     * header in each HTTP request. It ensures secure communication 
     * with the Deribit platform.
     */
    std::string accessToken;
};

#endif // ORDER_MANAGER_H
