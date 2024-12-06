#include <iostream>
#include "auth/AuthManager.h"

int main() {
    // Replace with your Deribit Test API credentials
    const std::string clientId = "Oo2qw6_G";
    const std::string clientSecret = "ubP2t06RGlV1X4onUoao7q1gdF-aSsZfLycwFfz0-YQ";

    AuthManager authManager(clientId, clientSecret);
    std::string token = authManager.authenticate();

    if (!token.empty()) {
        std::cout << "Authentication Successful! Token: " << token << std::endl;
    } else {
        std::cerr << "Authentication Failed!" << std::endl;
    }

    return 0;
}
