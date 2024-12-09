#include "WebSocketClient.h"
#include <iostream>
#include <mutex>
#include <thread>

#include <fmt/color.h>

// Define color constants for clarity
const auto ERROR_COLOR = fmt::fg(fmt::color::red);
const auto SUCCESS_COLOR = fmt::fg(fmt::color::cyan);
const auto INFO_COLOR = fmt::fg(fmt::color::blue);
const auto HIGHLIGHT_COLOR = fmt::fg(fmt::color::yellow);

/**
 * @brief Implements a thread-safe, SSL-enabled WebSocket client using Boost.Beast and Boost.Asio
 * 
 * This implementation follows the Pimpl (Pointer to Implementation) idiom to:
 * - Provide clean separation of interface and implementation
 * - Hide complex Boost library details from the header
 * - Enable easier future modifications without breaking ABI compatibility
 */
class WebSocketClient::Impl {
public:
    /**
     * @brief Constructor initializes WebSocket client configuration
     * 
     * Sets up SSL context with configurable verification:
     * - If verify_ssl is true, uses system's default certificate verification
     * - If verify_ssl is false, disables SSL certificate verification
     * 
     * @param config Configuration settings for WebSocket connection
     */
    Impl(const Config& config) 
        : config_(config), 
          ioc_(std::make_shared<asio::io_context>()),
          ssl_ctx_(asio::ssl::context::tlsv12_client),
          ws_stream_(*ioc_, ssl_ctx_),
          state_(State::Disconnected) 
    {
        // Configure SSL context
        if (config_.verify_ssl) {
            ssl_ctx_.set_default_verify_paths();
            ssl_ctx_.set_verify_mode(asio::ssl::verify_peer);
        } else {
            ssl_ctx_.set_verify_mode(asio::ssl::verify_none);
        }
    }

    /**
     * @brief Establishes a secure WebSocket connection
     * 
     * Connection process:
     * 1. Resolve host and port
     * 2. Set up a connection timeout mechanism
     * 3. Perform SSL handshake
     * 4. Complete WebSocket handshake
     * 
     * Thread-safety is ensured via mutex locking
     * Throws exceptions on connection failures
     */
    void connect() {
        std::lock_guard<std::mutex> lock(mutex_);
    
        if (state_ == State::Connected) {
            return;
        }

        state_ = State::Connecting;
        last_error_.reset();

        try {
            // Resolve host
            asio::ip::tcp::resolver resolver(*ioc_);
            auto const results = resolver.resolve(config_.host, config_.port);

            // Connect with timeout
            asio::steady_timer timeout_timer(*ioc_, 
                std::chrono::steady_clock::now() + config_.connect_timeout);
            
            timeout_timer.async_wait([this](const boost::system::error_code& ec) {
                if (ec != asio::error::operation_aborted) {
                    // Use next_layer().lowest_layer() instead of lowest_layer()
                    ws_stream_.next_layer().lowest_layer().cancel();
                }
            });

            // Connect to server
            asio::connect(ws_stream_.next_layer().lowest_layer(), results);

            // Perform SSL handshake
            ws_stream_.next_layer().handshake(asio::ssl::stream_base::client);

            // WebSocket handshake
            ws_stream_.handshake(config_.host, config_.path);

            state_ = State::Connected;
            std::cout << fmt::format(SUCCESS_COLOR, "WebSocket connected to: {}\n", config_.host);
        }
        catch (const std::exception& e) {
            state_ = State::Disconnected;
            last_error_ = e.what();
            std::cerr << fmt::format(ERROR_COLOR, "Connection error: {}\n", e.what());
            throw;
        }
    }

    /**
     * @brief Gracefully closes the WebSocket connection
     * 
     * Ensures thread-safe disconnection with proper error handling
     * Logs any errors encountered during disconnection
     */
    void disconnect() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (state_ == State::Connected) {
            try {
                // Gracefully close WebSocket
                ws_stream_.close(websocket::close_code::normal);
            }
            catch (const std::exception& e) {
                std::cerr << fmt::format(ERROR_COLOR, "Disconnection error: {}\n", e.what());
            }
        }
        
        state_ = State::Disconnected;
    }

    /**
     * @brief Sends a message over the WebSocket connection
     * 
     * Ensures thread-safety and connection state validation
     * Throws an exception if not connected or send fails
     * 
     * @param message String message to be sent
     */
    void send(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (state_ != State::Connected) {
            throw std::runtime_error("Not connected");
        }

        try {
            ws_stream_.write(asio::buffer(message));
        }
        catch (const std::exception& e) {
            last_error_ = e.what();
            std::cerr << fmt::format(ERROR_COLOR, "Send error: {}\n", e.what());
            throw;
        }
    }

    /**
     * @brief Receives a message from the WebSocket connection
     * 
     * Provides a synchronous message reception mechanism
     * Invokes a callback function with the received message
     * 
     * @param callback Function to be called with the received message
     */
    void receive(std::function<void(const std::string&)> callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (state_ != State::Connected) {
            throw std::runtime_error("Not connected");
        }

        try {
            beast::flat_buffer buffer;
            ws_stream_.read(buffer);
            
            std::string message = beast::buffers_to_string(buffer.data());
            callback(message);
        }
        catch (const std::exception& e) {
            last_error_ = e.what();
            std::cerr << fmt::format(ERROR_COLOR, "Receive error: {}\n", e.what());
            throw;
        }
    }

    /**
     * @brief Retrieves the current connection state
     * @return Current connection state
     */
    State get_state() const {
        return state_;
    }

    /**
     * @brief Retrieves the last error that occurred
     * @return Optional string containing the last error message
     */
    std::optional<std::string> get_last_error() const {
        return last_error_;
    }

private:
    Config config_;
    std::shared_ptr<asio::io_context> ioc_;
    asio::ssl::context ssl_ctx_;
    websocket::stream<asio::ssl::stream<asio::ip::tcp::socket>> ws_stream_;
    
    std::mutex mutex_;
    State state_;
    std::optional<std::string> last_error_;
};

// Remaining wrapper methods remain the same as in the original implementation
// (WebSocketClient constructor, destructor, and forwarding methods)

// Implement the WebSocketClient wrapper methods
WebSocketClient::WebSocketClient(Config config) 
    : pimpl_(std::make_unique<Impl>(config)) {
}

WebSocketClient::~WebSocketClient() {
    if (pimpl_ && pimpl_->get_state() == State::Connected) {
        pimpl_->disconnect();
    }
}

WebSocketClient::WebSocketClient(WebSocketClient&&) noexcept = default;
WebSocketClient& WebSocketClient::operator=(WebSocketClient&&) noexcept = default;

void WebSocketClient::connect() {
    pimpl_->connect();
}

void WebSocketClient::disconnect() {
    pimpl_->disconnect();
}

void WebSocketClient::send(const std::string& message) {
    pimpl_->send(message);
}

void WebSocketClient::receive(std::function<void(const std::string&)> callback) {
    pimpl_->receive(callback);
}

WebSocketClient::State WebSocketClient::get_state() const {
    return pimpl_->get_state();
}

std::optional<std::string> WebSocketClient::get_last_error() const {
    return pimpl_->get_last_error();
}