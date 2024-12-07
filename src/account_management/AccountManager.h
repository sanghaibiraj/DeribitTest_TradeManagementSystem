#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include <string>

class AccountManager {
public:
    AccountManager(const std::string& token);

    std::string getAccountSummary();
    std::string getPositions();

private:
    std::string accessToken;
};

#endif // ACCOUNT_MANAGER_H
