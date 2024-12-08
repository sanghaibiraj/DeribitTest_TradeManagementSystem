
---

# **Deribit Test Trading Management System**

A robust trading management system that interacts with the **Deribit API** to manage user authentication, order placement, modification, cancellation, and real-time market data streaming. Built with **C++**, it uses **Boost.Beast**, **libcurl**, and **nlohmann/json** for WebSocket communication, HTTP requests, and JSON handling, respectively.

---

## **Features**

1. **User Authentication**: Authenticate with the Deribit API using `client_id` and `client_secret`.
2. **Order Management**:
   - Place orders (limit orders).
   - Modify existing orders.
   - Cancel active orders.
   - Fetch all open orders by instrument.
3. **Account Management**:
   - Retrieve account summaries.
   - View open positions.
4. **Market Data**:
   - Fetch the order book for any trading instrument.
5. **Real-Time Market Streaming**:
   - Subscribe to real-time market data updates using WebSocket.
   - Broadcast updates to WebSocket clients.
6. **Robust Logging**:
   - Print requests and responses for debugging.
   - Graceful error handling and cleanup.

---

## **Technologies Used**

- **C++17**: Core programming language.
- **Boost.Beast**: WebSocket client implementation.
- **libcurl**: HTTP client for interacting with the Deribit API.
- **nlohmann/json**: JSON handling.
- **CMake**: Build system.
- **OpenSSL**: Secure SSL/TLS communication.
- **pthread**: Threading for the WebSocket server.

---

## **Folder Structure**

```
DeribitTest_TradeManagementSystem/
│
├── src/
│   ├── main.cpp                      # Main entry point
│   ├── auth/
│   │   ├── AuthManager.h             # Authentication manager (header)
│   │   └── AuthManager.cpp           # Authentication manager (implementation)
│   ├── order_management/
│   │   ├── OrderManager.h            # Order manager (header)
│   │   └── OrderManager.cpp          # Order manager (implementation)
│   ├── account_management/
│   │   ├── AccountManager.h          # Account manager (header)
│   │   └── AccountManager.cpp        # Account manager (implementation)
│   ├── market_data/
│   │   ├── MarketDataManager.h       # Market data manager (header)
│   │   └── MarketDataManager.cpp     # Market data manager (implementation)
│   ├── WebSocketClient.h             # WebSocket client (header)
│   ├── WebSocketClient.cpp           # WebSocket client (implementation)
│
├── CMakeLists.txt                    # Build configuration
└── README.md                         # Documentation (this file)
```

---

## **Setup Instructions**

### **Prerequisites**

1. **Operating System**: Linux (Ubuntu recommended), macOS, or Windows with WSL.
2. **Dependencies**:
   - C++17 compiler (e.g., GCC 9+ or Clang 9+).
   - CMake (version 3.14 or higher).
   - Boost Libraries (including Boost.Beast and Boost.Asio).
   - libcurl.
   - OpenSSL.
   - pthread.

### **Install Dependencies**

For Ubuntu:
```bash
sudo apt update
sudo apt install -y build-essential cmake libboost-all-dev libssl-dev libcurl4-openssl-dev
```

For Fedora:
```bash
sudo dnf install gcc-c++ cmake boost-devel openssl-devel libcurl-devel
```

For macOS (using Homebrew):
```bash
brew install cmake boost openssl curl
```

### **Build the Project**

1. Clone the repository:
   ```bash
   git clone https://github.com/username/DeribitTest_TradeManagementSystem.git
   cd DeribitTest_TradeManagementSystem
   ```

2. Create a build directory:
   ```bash
   mkdir build && cd build
   ```

3. Configure the project with CMake:
   ```bash
   cmake ..
   ```

4. Build the project:
   ```bash
   make
   ```

5. Run the application:
   ```bash
   ./GoQuant
   ```

---

## **Usage**

### **Program Workflow**

1. **Start the Program**:
   Run the application and provide your Deribit API credentials (`client_id` and `client_secret`).

2. **Main Menu**:
   The program presents a menu with the following options:
   ```
   === Deribit Trading System Menu ===
   1. Place Order
   2. Modify Order
   3. Cancel Order
   4. Get Account Summary
   5. Get Current Positions
   6. Get Order Book
   7. Start WebSocket Server for Real-Time Data
   8. Get All Orders
   9. Exit
   ```

### **Menu Features**

- **Place Order**: Provide instrument name, side (`buy`/`sell`), amount, and price.
- **Modify Order**: Modify an existing order by providing the `order_id`, new amount, and price.
- **Cancel Order**: Cancel an order by providing its `order_id`.
- **Get Account Summary**: Fetch and display account balance and equity details.
- **Get Current Positions**: List all open positions.
- **Get Order Book**: View the top 10 levels of the order book for an instrument.
- **Start WebSocket Server**:
   - Connect to Deribit WebSocket API and subscribe to real-time updates.
   - Broadcast updates to WebSocket clients connected to the server.
- **Get All Orders**: List all open orders for a specific instrument.

---

## **Real-Time WebSocket Streaming**

1. The WebSocket client connects to `wss://test.deribit.com/ws/api/v2`.
2. Subscribes to channels like `book.BTC-PERPETUAL.100ms`.
3. Streams data to a local WebSocket server running on `localhost:9000`.
4. External clients can connect to the server for live updates.

---

## **Debugging Tips**

1. **Inspect Logs**:
   - Requests and responses are printed to the console for debugging.
2. **CURL Errors**:
   - Check internet connectivity or API credentials if CURL fails.
3. **JSON Parsing Errors**:
   - Ensure the API response structure matches the parsing logic.

---

## **Future Improvements**

1. **Error Handling**:
   - Implement retries for failed API calls.
2. **Enhanced WebSocket Server**:
   - Add more subscription management options.
3. **Support for Historical Data**:
   - Fetch past trades and orders for analytics.

---