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

# Find and link libcurl
find_package(CURL REQUIRED)
target_include_directories(GoQuant PRIVATE ${CURL_INCLUDE_DIR})
target_link_libraries(GoQuant PRIVATE ${CURL_LIBRARIES})

include(FetchContent)
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.2
    CMAKE_ARGS -DJSON_BuildTests=OFF
)
FetchContent_MakeAvailable(json)
# find_package(nlohmann_json 3.2.0 REQUIRED)
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
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
FetchContent_MakeAvailable(fmt)
target_link_libraries(GoQuant PRIVATE fmt::fmt)


