#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <unordered_map>
#include <set>
#include <string>

typedef websocketpp::server<websocketpp::config::asio> WebSocketServerType;

class WebSocketServer {
public:
    WebSocketServer(int port);
    void run();
    void broadcast(const std::string& symbol, const std::string& message);

private:
    WebSocketServerType server;
    std::unordered_map<std::string, std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>>> subscriptions;

    void onMessage(websocketpp::connection_hdl hdl, WebSocketServerType::message_ptr msg);
    void onOpen(websocketpp::connection_hdl hdl);
    void onClose(websocketpp::connection_hdl hdl);
};

#endif // WEBSOCKET_SERVER_H
