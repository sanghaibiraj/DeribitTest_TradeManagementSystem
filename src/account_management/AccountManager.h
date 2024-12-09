#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include <string>

/**
 * @file AccountManager.h
 *
 * @brief Defines the `AccountManager` class, which is responsible for managing account-related data
 *        and operations in the trading platform API.
 *
 * The `AccountManager` class provides methods to retrieve account summaries and active positions.
 * It acts as a bridge between the trading system and the API's account-related endpoints, ensuring
 * authenticated access to account data using the provided access token.
 *
 * ### Key Responsibilities:
 * - Fetch the account summary, including balance and currency details.
 * - Retrieve the current open positions in the user's trading account.
 *
 * ### Integration with the Trading System:
 * The `AccountManager` class is a crucial component of the trading system, enabling users to view
 * their account status and analyze their trading activity. It works in conjunction with other
 * components, such as the `AuthManager` for authentication and `OrderManager` for trade execution.
 */

/**
 * @class AccountManager
 *
 * @brief Manages account-related data and operations for the trading system.
 *
 * ### Key Features:
 * - Securely stores the access token for authenticated API requests.
 * - Provides methods to fetch account summaries and active trading positions.
 *
 * ### Workflow:
 * 1. Create an instance of the `AccountManager` class, providing the access token retrieved during authentication.
 * 2. Use `getAccountSummary` to fetch overall account details, such as balances and available funds.
 * 3. Use `getPositions` to fetch details of currently open positions in the trading account.
 *
 * ### Example:
 * ```
 * AccountManager accountManager("access_token");
 * std::string accountSummary = accountManager.getAccountSummary();
 * std::cout << "Account Summary: " << accountSummary << std::endl;
 *
 * std::string positions = accountManager.getPositions();
 * std::cout << "Current Positions: " << positions << std::endl;
 * ```
 */
class AccountManager {
public:
    /**
     * @brief Constructor for the `AccountManager` class.
     *
     * @param token The access token retrieved during authentication.
     *
     * ### Purpose:
     * - Initializes the `AccountManager` with the access token, which is required for making
     *   authenticated API requests to retrieve account data.
     */
    AccountManager(const std::string& token);

    /**
     * @brief Fetches the account summary from the trading platform API.
     *
     * @return A JSON-formatted string containing the account summary, including balance and currency details.
     *
     * ### Workflow:
     * - Sends an HTTP request to the trading platform's account summary endpoint.
     * - Uses the access token for authentication.
     * - Returns the API response as a raw JSON string for further processing.
     *
     * ### Example:
     * ```
     * std::string summary = accountManager.getAccountSummary();
     * std::cout << "Account Summary: " << summary << std::endl;
     * ```
     *
     * ### API Data:
     * - **Endpoint**: `/private/get_account_summary`
     * - **Response**: Includes balance, available funds, currency details, and more.
     */
    std::string getAccountSummary();

    /**
     * @brief Fetches the current open positions from the trading platform API.
     *
     * @return A JSON-formatted string containing details of active trading positions.
     *
     * ### Workflow:
     * - Sends an HTTP request to the trading platform's positions endpoint.
     * - Uses the access token for authentication.
     * - Returns the API response as a raw JSON string for further processing.
     *
     * ### Example:
     * ```
     * std::string positions = accountManager.getPositions();
     * std::cout << "Current Positions: " << positions << std::endl;
     * ```
     *
     * ### API Data:
     * - **Endpoint**: `/private/get_positions`
     * - **Response**: Includes details such as instrument names, sizes, entry prices, and profits.
     */
    std::string getPositions();

private:
    /**
     * @brief The access token retrieved during authentication.
     *
     * This token is required for all authenticated API requests. It is securely stored within
     * the `AccountManager` class and is not exposed directly to external components.
     */
    std::string accessToken;
};

#endif // ACCOUNT_MANAGER_H
