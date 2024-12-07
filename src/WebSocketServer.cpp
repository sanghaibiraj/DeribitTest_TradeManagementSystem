#include "WebSocketServer.h"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

WebSocketServer::WebSocketServer(int port) {
    server.init_asio();
    server.set_message_handler([this](websocketpp::connection_hdl hdl, WebSocketServerType::message_ptr msg) {
        onMessage(hdl, msg);
    });
    server.set_open_handler([this](websocketpp::connection_hdl hdl) {
        onOpen(hdl);
    });
    server.set_close_handler([this](websocketpp::connection_hdl hdl) {
        onClose(hdl);
    });
    server.listen(port);
    server.start_accept();
}

void WebSocketServer::run() {
    std::cout << "WebSocket server running..." << std::endl;
    server.run();
}

void WebSocketServer::broadcast(const std::string& symbol, const std::string& message) {
    if (subscriptions.count(symbol)) {
        for (const auto& hdl : subscriptions[symbol]) {
            server.send(hdl, message, websocketpp::frame::opcode::text);
        }
    }
}

void WebSocketServer::onMessage(websocketpp::connection_hdl hdl, WebSocketServerType::message_ptr msg) {
    auto msgJson = json::parse(msg->get_payload());

    if (msgJson.contains("subscribe")) {
        std::string symbol = msgJson["subscribe"];
        subscriptions[symbol].insert(hdl);
        std::cout << "Client subscribed to: " << symbol << std::endl;
    }
}

void WebSocketServer::onOpen(websocketpp::connection_hdl hdl) {
    std::cout << "Client connected" << std::endl;
}

void WebSocketServer::onClose(websocketpp::connection_hdl hdl) {
    std::cout << "Client disconnected" << std::endl;

    for (auto& [symbol, clients] : subscriptions) {
        clients.erase(hdl);
    }
}
