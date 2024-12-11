### **Trading Management System Documentation**

---

## **Overview**

This project is a **Trading Management System** designed to interact with the Deribit API for cryptocurrency trading. It provides a robust and user-friendly interface for performing essential trading functions, such as placing orders, modifying or canceling trades, retrieving account information, and streaming real-time market data using WebSockets.

---

## **Features**

### **1. User Authentication**
   - **What It Does**:
     - Authenticates the user with the Deribit API using client credentials.
     - Retrieves an access token required for making API requests.
   - **How It Works**:
     - Uses the `AuthManager` class to send a POST request to the Deribit authentication endpoint.
     - Stores the access token for use in other system components.

### **2. Order Management**
   - **What It Does**:
     - Allows the user to place, modify, and cancel orders.
     - Retrieves all open orders for a specific instrument.
   - **How It Works**:
     - Uses the `OrderManager` class.
     - Sends JSON-RPC API requests to the relevant Deribit endpoints:
       - Place an order: `/private/buy` or `/private/sell`.
       - Modify an order: `/private/edit`.
       - Cancel an order: `/private/cancel`.
       - Retrieve all open orders: `/private/get_open_orders_by_instrument`.

### **3. Account Management**
   - **What It Does**:
     - Fetches account summaries, including balances and available funds.
     - Retrieves current open trading positions.
   - **How It Works**:
     - Uses the `AccountManager` class.
     - Interacts with these Deribit endpoints:
       - `/private/get_account_summary` for account details.
       - `/private/get_positions` for open position data.

### **4. Market Data Retrieval**
   - **What It Does**:
     - Fetches the order book for a given instrument.
   - **How It Works**:
     - Uses the `MarketDataManager` class.
     - Sends a request to the `/public/get_order_book` endpoint.

### **5. Real-Time Market Data Streaming**
   - **What It Does**:
     - Streams real-time market data for a subscribed instrument via WebSocket.
     - Enables live updates on order book changes.
   - **How It Works**:
     - Uses the `WebSocketClient` class built with Boost.Beast.
     - Connects to the WebSocket API at `wss://test.deribit.com/ws/api/v2`.
     - Subscribes to `book.<instrument>.100ms` channels for order book updates.

---

## **Project Structure**

### **Main Components**

| Module/Class          | Purpose                                                                 |
|------------------------|-------------------------------------------------------------------------|
| `AuthManager`         | Handles user authentication and access token management.               |
| `OrderManager`        | Manages order-related operations, such as placing and modifying orders. |
| `AccountManager`      | Retrieves account summaries and open trading positions.                 |
| `MarketDataManager`   | Fetches market data, such as the order book for instruments.            |
| `WebSocketClient`     | Manages real-time data streaming using WebSocket connections.           |

---

## **How It Works**

### **1. Authentication**
   - The system starts by asking the user for the `client_id` and `client_secret`.
   - These credentials are passed to the `AuthManager`, which sends a POST request to authenticate the user.
   - Upon successful authentication, an access token is retrieved and used for subsequent API requests.

### **2. Order Operations**
   - **Place Order**:
     - User specifies the instrument, side (buy/sell), quantity, and price.
     - `OrderManager` sends the order details to the Deribit API.
   - **Modify Order**:
     - User provides the order ID, new quantity, and new price.
     - The system modifies the order using the API.
   - **Cancel Order**:
     - User specifies the order ID to cancel.
     - The system cancels the order through the API.

### **3. Account Operations**
   - Fetches the account summary to display balances, available funds, and margin usage.
   - Retrieves open trading positions, including entry prices and profits.

### **4. Market Data**
   - Retrieves the order book for a given instrument to show bid/ask prices and quantities.

### **5. Real-Time Data Streaming**
   - **Subscription**:
     - User specifies an instrument to subscribe to.
     - `WebSocketClient` connects to the WebSocket server and subscribes to order book updates.
   - **Live Updates**:
     - Receives real-time updates and processes them for display.

---

## **Latency Metrics**

The following metrics include latency benchmarking results:

- **Order Placement Latency**: 602 ms
- **Modify Order Latency**: 593 ms
- **Order Fetch Latency**: 548 ms
- **Cancel Order Latency**: 540 ms
- **Account Summary Latency**: 557 ms
- **Position Fetch Latency**: 610 ms
- **Order Book Fetch Latency**: 582 ms

---

## **Dependencies**

### **Libraries Used**
1. **libcurl**:
   - For making HTTP requests to the Deribit REST API.
   - Handles secure communication with HTTPS endpoints.

2. **nlohmann/json**:
   - For constructing and parsing JSON requests and responses.

3. **Boost.Asio and Boost.Beast**:
   - Provides asynchronous networking and WebSocket functionality.
   - Enables secure WebSocket connections using SSL.


---

## **How to Run**

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/sanghaibiraj/DeribitTest_TradeManagementSystem.git
   cd DeribitTest_TradeManagementSystem
   ```

2. **Build the Project**:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

3. **Run the Program**:
   ```bash
   ./GoQuant
   ```

---

## **Sample Menu**

### **1. Place Order**
- Input instrument name (e.g., BTC-PERPETUAL), side (buy/sell), quantity, and price.

### **2. Modify Order**
- Input order ID, new quantity, and new price.

### **3. Cancel Order**
- Input the order ID to cancel.

### **4. Get All Orders**
- Input instrument name to fetch all open orders.

### **5. Get Account Summary**
- Displays current balance and available funds.

### **6. Get Current Positions**
- Shows open positions with entry prices and profits.

### **7. Get Order Book**
- Input instrument name to retrieve the order book.

### **8. Real-Time Market Data**
- Input instrument name for live order book updates via WebSocket.

---

## **Future Enhancements**

- **Trading Strategies**:
  - Implement automated trading strategies based on real-time data.
- **Historical Data Analysis**:
  - Fetch and analyze historical trading data for performance evaluation.
- **Error Resilience**:
  - Enhance error handling for API failures and connection interruptions.

---

## **Conclusion**

This Trading Management System integrates Deribit's API for seamless cryptocurrency trading. It combines REST and WebSocket functionalities to provide a comprehensive toolkit for traders. The modular design ensures scalability and ease of future development.