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

namespace boost::beast::websocket {
    // Custom teardown function for SSL stream
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

class WebSocketClient {
public:
    // Enum for connection state
    enum class State {
        Disconnected,
        Connecting,
        Connected
    };

    // Configuration struct for connection parameters
    struct Config {
        std::string host;
        std::string port;
        std::string path{"/ws"};
        bool verify_ssl{true};
        std::chrono::seconds connect_timeout{10};
        std::chrono::seconds read_timeout{30};
    };

    // Constructor with configuration
    explicit WebSocketClient(Config config);

    // Destructor
    ~WebSocketClient();

    // Prevent copy, allow move
    WebSocketClient(const WebSocketClient&) = delete;
    WebSocketClient& operator=(const WebSocketClient&) = delete;
    WebSocketClient(WebSocketClient&&) noexcept;
    WebSocketClient& operator=(WebSocketClient&&) noexcept;

    // Connection methods
    void connect();
    void reconnect();
    void disconnect();

    // Communication methods
    void send(const std::string& message);
    void async_send(const std::string& message);
    
    // Receive methods with different strategies
    void receive(std::function<void(const std::string&)> callback);
    void async_receive(std::function<void(const std::string&)> callback);

    // State and status queries
    State get_state() const;
    bool is_connected() const;

    // Error handling
    std::optional<std::string> get_last_error() const;

private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

#endif // WEBSOCKET_CLIENT_H