#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include <string>

class OrderManager {
public:
    OrderManager(const std::string& accessToken);
    std::string placeOrder(const std::string& instrument, const std::string& side, double quantity, double price);
    std::string modifyOrder(const std::string& orderId, double newQuantity, double newPrice);
    std::string cancelOrder(const std::string& orderId);

private:
    std::string accessToken;
};

#endif // ORDER_MANAGER_H
