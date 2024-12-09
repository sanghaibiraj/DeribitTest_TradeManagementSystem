#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <string>
#include <functional>
#include <memory>
#include <optional>

/**
 * @file WebSocketClient.h
 *
 * @brief A header file defining the WebSocketClient class, a robust WebSocket 
 * client implementation using Boost.Beast and Boost.Asio libraries.
 *
 * The WebSocketClient class provides an interface for establishing secure 
 * WebSocket connections, sending and receiving messages, handling connection 
 * states, and managing error conditions. It supports synchronous and asynchronous 
 * communication, making it suitable for real-time data streaming applications like 
 * trading platforms.
 */

namespace boost::beast::websocket {
    /**
     * @brief Custom teardown function for SSL streams.
     *
     * This function ensures proper shutdown of SSL connections by first shutting 
     * down the SSL stream and then closing the underlying TCP socket. It also 
     * handles specific errors like incomplete handshakes gracefully.
     *
     * @tparam Teardown A custom teardown policy, if any.
     * @param role Specifies the WebSocket role (client/server).
     * @param socket The SSL stream wrapping the TCP socket.
     * @param ec Error code to store any shutdown errors.
     */
    template <typename Teardown>
    void teardown(role_type role,
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket,
        boost::system::error_code& ec)
    {
        // Shutdown SSL stream first
        socket.shutdown(ec);

        // If shutdown fails due to a handshake not being completed, ignore the error
        if (ec == boost::asio::error::invalid_argument)
            ec = {};

        // Then close the underlying socket
        if (socket.lowest_layer().is_open())
            socket.lowest_layer().close(ec);
    }
}

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;

/**
 * @class WebSocketClient
 * 
 * @brief A high-level WebSocket client implementation.
 *
 * The WebSocketClient class allows secure WebSocket connections using SSL. It 
 * abstracts the complexities of WebSocket communication and provides a clean 
 * interface for sending, receiving, and managing WebSocket connections.
 *
 * Features:
 * - SSL-enabled secure WebSocket connections.
 * - Configurable connection parameters (host, port, path, timeouts).
 * - Synchronous and asynchronous communication methods.
 * - Error handling and connection state management.
 */
class WebSocketClient {
public:
    /**
     * @enum State
     * @brief Defines the connection state of the WebSocket client.
     */
    enum class State {
        Disconnected, /**< The client is not connected. */
        Connecting,   /**< The client is in the process of connecting. */
        Connected     /**< The client is connected to the server. */
    };

    /**
     * @struct Config
     * 
     * @brief Configuration for WebSocket client connections.
     *
     * This structure holds the connection parameters required to establish 
     * a WebSocket connection, such as the host, port, path, and timeouts. 
     * The default path is `/ws`, and SSL verification is enabled by default.
     */
    struct Config {
        std::string host;                   /**< The WebSocket server hostname. */
        std::string port;                   /**< The port to connect to (e.g., "443" for SSL). */
        std::string path{"/ws"};            /**< The WebSocket path (default is `/ws`). */
        bool verify_ssl{true};              /**< Whether to verify the SSL certificate. */
        std::chrono::seconds connect_timeout{10}; /**< Timeout for connection attempts. */
        std::chrono::seconds read_timeout{30};    /**< Timeout for reading messages. */
    };

    /**
     * @brief Constructs a WebSocketClient with the given configuration.
     * 
     * @param config A Config structure containing connection parameters.
     */
    explicit WebSocketClient(Config config);

    /**
     * @brief Destructor for the WebSocketClient class.
     * 
     * Ensures proper cleanup of resources, including disconnecting the WebSocket 
     * if it is still connected.
     */
    ~WebSocketClient();

    /**
     * @brief Move constructor (disabled copy).
     */
    WebSocketClient(WebSocketClient&&) noexcept;

    /**
     * @brief Move assignment operator (disabled copy).
     */
    WebSocketClient& operator=(WebSocketClient&&) noexcept;

    // Deleted copy constructor and assignment operator
    WebSocketClient(const WebSocketClient&) = delete;
    WebSocketClient& operator=(const WebSocketClient&) = delete;

    // --- Connection Management ---

    /**
     * @brief Connects to the WebSocket server.
     * 
     * Attempts to establish a connection to the WebSocket server using the provided 
     * configuration parameters. Throws exceptions on failure.
     */
    void connect();

    /**
     * @brief Reconnects to the WebSocket server.
     * 
     * Closes the current connection (if any) and attempts to reconnect.
     */
    void reconnect();

    /**
     * @brief Disconnects from the WebSocket server.
     * 
     * Gracefully closes the WebSocket connection if it is currently active.
     */
    void disconnect();

    // --- Communication Methods ---

    /**
     * @brief Sends a message to the WebSocket server.
     * 
     * @param message The message to send.
     * 
     * This method sends the specified message to the connected WebSocket server 
     * synchronously. An exception is thrown if the client is not connected.
     */
    void send(const std::string& message);

    /**
     * @brief Sends a message asynchronously.
     * 
     * @param message The message to send.
     * 
     * This method queues the specified message for sending. The actual sending 
     * occurs asynchronously, allowing the application to continue processing other tasks.
     */
    void async_send(const std::string& message);

    /**
     * @brief Receives a message from the WebSocket server.
     * 
     * @param callback A function to process the received message.
     * 
     * Blocks until a message is received and then invokes the callback function 
     * with the received message. Throws exceptions on connection errors.
     */
    void receive(std::function<void(const std::string&)> callback);

    /**
     * @brief Asynchronously receives messages.
     * 
     * @param callback A function to process the received messages.
     * 
     * Continuously listens for incoming messages and processes them using 
     * the provided callback function. Does not block the main thread.
     */
    void async_receive(std::function<void(const std::string&)> callback);

    // --- State and Status Queries ---

    /**
     * @brief Retrieves the current connection state.
     * 
     * @return The current state of the WebSocket connection.
     */
    State get_state() const;

    /**
     * @brief Checks if the client is currently connected.
     * 
     * @return True if the client is connected; otherwise, false.
     */
    bool is_connected() const;

    // --- Error Handling ---

    /**
     * @brief Retrieves the last error encountered by the client.
     * 
     * @return An optional string describing the last error, or std::nullopt if no error occurred.
     */
    std::optional<std::string> get_last_error() const;

private:
    /**
     * @class Impl
     * 
     * @brief A private implementation class for encapsulating WebSocket functionality.
     * 
     * The `Impl` class contains the actual logic for managing WebSocket connections. 
     * Using a PIMPL (pointer to implementation) pattern ensures that the header file 
     * remains lightweight and reduces compilation dependencies.
     */
    class Impl;

    /**
     * @brief Pointer to the implementation object.
     * 
     * Encapsulates the WebSocket logic and ensures clean separation of interface and implementation.
     */
    std::unique_ptr<Impl> pimpl_;
};

#endif // WEBSOCKET_CLIENT_H
