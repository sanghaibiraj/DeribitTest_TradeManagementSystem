---

# **Deribit Test Trading Management System**

A robust trading management system that interacts with the **Deribit API** to manage user authentication, order placement, modification, cancellation, and real-time market data streaming. Built with **C++**, it uses **Boost.Beast**, **libcurl**, **fmt**, and **nlohmann/json** for WebSocket communication, HTTP requests, JSON handling, and colorful formatted output, respectively.

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
6. **Beautified Output**:
   - Use the **fmt** library to enhance console output with formatted and colorful data presentation.
7. **Robust Logging**:
   - Print requests and responses for debugging.
   - Graceful error handling and cleanup.
8. **Latency Measurement**:
   - Measure and display latency metrics for order placement, market data processing, WebSocket message propagation, and the trading loop.

---

## **Technologies Used**

- **C++17**: Core programming language.
- **Boost.Beast**: WebSocket client implementation.
- **libcurl**: HTTP client for interacting with the Deribit API.
- **nlohmann/json**: JSON handling.
- **fmt**: Colorful formatted output.
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
   - **None Pre-installed!** All required libraries will be fetched automatically using `CMake FetchContent`.

---

### **Build the Project**

1. Clone the repository:
   ```bash
   git clone https://github.com/sanghaibiraj/DeribitTest_TradeManagementSystem.git
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
   4. Get All Orders
   5. Get Account Summary
   6. Get Current Positions
   7. Get Order Book
   8. Start WebSocket Server for Real-Time Data
   9. Exit
   ```

### **Menu Features**

- **Place Order**: Provide instrument name, side (`buy`/`sell`), amount, and price.
- **Modify Order**: Modify an existing order by providing the `order_id`, new amount, and price.
- **Cancel Order**: Cancel an order by providing its `order_id`.
- **Get All Orders**: Fetch and display all open orders for a specific instrument, beautified using **fmt** for readability.
- **Get Account Summary**: Fetch and display account balance and equity details, with colorful output.
- **Get Current Positions**: List all open positions.
- **Get Order Book**: View the top 10 levels of the order book for an instrument.
- **Start WebSocket Server**:
   - Connect to Deribit WebSocket API and subscribe to real-time updates.
   - Broadcast updates to WebSocket clients connected to the server.

---

## **Real-Time WebSocket Streaming**

1. The WebSocket client connects to `wss://test.deribit.com/ws/api/v2`.
2. Subscribes to channels like `book.BTC-PERPETUAL.100ms`.

---

## **Debugging Tips**

1. **Inspect Logs**:
   - Requests and responses are printed to the console for debugging.
2. **CURL Errors**:
   - Check internet connectivity or API credentials if CURL fails.
3. **JSON Parsing Errors**:
   - Ensure the API response structure matches the parsing logic.
4. **Latency Monitoring**:
   - Use logged timestamps to identify performance bottlenecks.

---

## **Future Improvements**

1. **Error Handling**:
   - Implement retries for failed API calls.
2. **Enhanced WebSocket Server**:
   - Add more subscription management options.
3. **Support for Historical Data**:
   - Fetch past trades and orders for analytics.
4. **Testing Framework**:
   - Add unit tests for individual components to improve reliability.
5. **API Rate Limiting**:
   - Handle API rate limits more gracefully with retries and exponential backoff.
6. **Automated Deployment**:
   - Create Docker images for easy deployment across platforms.

---

