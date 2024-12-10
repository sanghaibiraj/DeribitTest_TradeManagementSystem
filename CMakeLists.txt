cmake_minimum_required(VERSION 3.14)
project(GoQuantAssignment)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)

# Add source files
add_executable(GoQuant
    src/main.cpp
    src/auth/AuthManager.cpp
    src/order_management/OrderManager.cpp
    src/account_management/AccountManager.cpp
    src/market_data/MarketDataManager.cpp
    src/WebSocketClient.cpp
)

# Add include directories
include_directories(
    /usr/include/boost
)

include(FetchContent)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2 # Replace with the desired version
)

FetchContent_MakeAvailable(nlohmann_json)

target_link_libraries(your_target PRIVATE nlohmann_json::nlohmann_json)


# Find and link libcurl
find_package(CURL REQUIRED)
target_include_directories(GoQuant PRIVATE ${CURL_INCLUDE_DIR})
target_link_libraries(GoQuant PRIVATE ${CURL_LIBRARIES})

find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(GoQuant PRIVATE nlohmann_json::nlohmann_json)

find_package(Boost REQUIRED COMPONENTS system)

include_directories(${Boost_INCLUDE_DIRS})

target_link_libraries(GoQuant PRIVATE
    Boost::boost
    Boost::system
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread
)

find_package(OpenSSL REQUIRED)

find_package(fmt REQUIRED)
target_link_libraries(GoQuant PRIVATE fmt::fmt)

