#pragma once
#include <memory>
#include <string>
#include <ctime>
#include "IUserStore.h"

// SignInSignUp is the business logic layer for user authentication.
// It is intentionally decoupled from any specific database by depending
// on the IUserStore interface.  Concrete persistence implementations
// (e.g. MySqlUserStore) are provided at runtime via dependency injection.
// This allows the same class to work with MySQL, SQLite, cloud services, etc.


class SignInSignUp {
public:
    enum class AccountStatus { Active, Inactive, Blocked };

    explicit SignInSignUp(std::unique_ptr<IUserStore> store);
    ~SignInSignUp();

    bool signup(const std::string& username, const std::string& password);
    bool signin(const std::string& username, const std::string& password);
    AccountStatus getAccountStatus(const std::string& username);

private:
    std::unique_ptr<IUserStore> m_store;
    std::string hashPassword(const std::string& password);
};