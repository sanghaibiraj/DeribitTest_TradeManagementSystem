#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include <string>

/**
 * @file AuthManager.h
 *
 * @brief Defines the `AuthManager` class, which is responsible for handling user authentication with the trading platform API.
 *
 * The `AuthManager` class provides methods to authenticate users by exchanging their client credentials
 * (client ID and client secret) for an access token. This token is then used to authorize further API requests.
 *
 * ### Key Responsibilities:
 * - Authenticate users by interacting with the API's authentication endpoint.
 * - Securely store and manage the access token after successful authentication.
 *
 * ### Usage in a Trading System:
 * The `AuthManager` class is an integral component of a larger trading system. It ensures that all API requests
 * are authenticated, which is a prerequisite for accessing protected endpoints (e.g., placing orders, retrieving account data).
 */

/**
 * @class AuthManager
 *
 * @brief Handles user authentication with the trading platform API.
 *
 * ### Key Features:
 * - Stores client credentials (client ID and client secret).
 * - Authenticates with the API to retrieve an access token.
 * - Provides access to the token for other components of the trading system.
 *
 * ### Workflow:
 * 1. Create an instance of the `AuthManager` class, providing the client ID and client secret as parameters.
 * 2. Call the `authenticate` method to retrieve an access token.
 * 3. Use the access token for further API requests that require authentication.
 *
 * ### Example:
 * ```
 * AuthManager authManager("your_client_id", "your_client_secret");
 * std::string token = authManager.authenticate();
 *
 * if (!token.empty()) {
 *     std::cout << "Authentication successful! Token: " << token << std::endl;
 * } else {
 *     std::cerr << "Authentication failed." << std::endl;
 * }
 * ```
 */
class AuthManager {
public:
    /**
     * @brief Constructor for the `AuthManager` class.
     *
     * @param clientId The client ID provided by the trading platform.
     * @param clientSecret The client secret associated with the client ID.
     *
     * ### Purpose:
     * - Initializes the `AuthManager` instance with the required client credentials.
     * - These credentials are used to authenticate with the API and obtain an access token.
     */
    AuthManager(const std::string& clientId, const std::string& clientSecret);

    /**
     * @brief Authenticates with the trading platform and retrieves an access token.
     *
     * @return The access token as a `std::string`, or an empty string if authentication fails.
     *
     * ### Workflow:
     * 1. Constructs an authentication request using the client ID and client secret.
     * 2. Sends the request to the trading platform's authentication endpoint.
     * 3. Parses the response to extract the access token.
     * 4. Stores the access token for use in future API requests.
     *
     * ### Error Handling:
     * - If the API returns an error (e.g., invalid credentials), this method logs the error and returns an empty string.
     * - If the request fails due to network issues, an error is logged, and an empty string is returned.
     *
     * ### Example Usage:
     * ```
     * AuthManager authManager("my_client_id", "my_client_secret");
     * std::string token = authManager.authenticate();
     * if (!token.empty()) {
     *     std::cout << "Token: " << token << std::endl;
     * } else {
     *     std::cerr << "Authentication failed." << std::endl;
     * }
     * ```
     */
    std::string authenticate();

private:
    /**
     * @brief The client ID provided by the trading platform for authentication.
     *
     * This is a unique identifier for the user or application that is accessing the API.
     */
    std::string clientId;

    /**
     * @brief The client secret associated with the client ID.
     *
     * This is a secure key used in combination with the client ID to authenticate with the API.
     */
    std::string clientSecret;

    /**
     * @brief The access token retrieved after successful authentication.
     *
     * This token is used to authorize subsequent API requests. It is typically valid for a specific duration
     * and may need to be refreshed after expiration.
     */
    std::string accessToken;
};

#endif // AUTH_MANAGER_H
