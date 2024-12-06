#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include <string>

class AuthManager {
public:
    AuthManager(const std::string& clientId, const std::string& clientSecret);
    std::string authenticate();

private:
    std::string clientId;
    std::string clientSecret;
    std::string accessToken;
};

#endif // AUTH_MANAGER_H
