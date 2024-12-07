#include "WebSocketClient.h"
#include <iostream>
#include <mutex>
#include <thread>

class WebSocketClient::Impl {
public:
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
        std::cout << "WebSocket connected to " << config_.host << std::endl;
    }
    catch (const std::exception& e) {
        state_ = State::Disconnected;
        last_error_ = e.what();
        std::cerr << "Connection error: " << e.what() << std::endl;
        throw;
    }
}

    void disconnect() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (state_ == State::Connected) {
            try {
                // Gracefully close WebSocket
                ws_stream_.close(websocket::close_code::normal);
            }
            catch (const std::exception& e) {
                std::cerr << "Disconnection error: " << e.what() << std::endl;
            }
        }
        
        state_ = State::Disconnected;
    }

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
            std::cerr << "Send error: " << e.what() << std::endl;
            throw;
        }
    }

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
            std::cerr << "Receive error: " << e.what() << std::endl;
            throw;
        }
    }

    State get_state() const {
        return state_;
    }

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