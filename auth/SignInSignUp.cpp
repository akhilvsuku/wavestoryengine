#include "SignInSignUp.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <ctime>

SignInSignUp::SignInSignUp(std::unique_ptr<IUserStore> store)
    : m_store(std::move(store)) {}

SignInSignUp::~SignInSignUp() = default;

std::string SignInSignUp::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)password.c_str(), password.size(), hash);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return ss.str();
}

bool SignInSignUp::signup(const std::string& username, const std::string& password) {
    if (m_store->userExists(username)) return false;
    std::string hash = hashPassword(password);
    std::time_t now = std::time(nullptr);
    UserRecord rec;
    rec.username = username;
    rec.passwordHash = hash;
    rec.lastPasswordHash = hash;
    rec.passwordChangedTime = now;
    rec.accountCreatedTime = now;
    rec.accountStatus = "Active";
    return m_store->insertUser(rec);
}

bool SignInSignUp::signin(const std::string& username, const std::string& password) {
    auto row = m_store->fetchUser(username);
    if (!row) return false;
    std::string hash = hashPassword(password);
    return hash == row->passwordHash && row->accountStatus == "Active";
}

SignInSignUp::AccountStatus SignInSignUp::getAccountStatus(const std::string& username) {
    auto row = m_store->fetchUser(username);
    if (!row) return AccountStatus::Inactive;
    if (row->accountStatus == "Active") return AccountStatus::Active;
    if (row->accountStatus == "Blocked") return AccountStatus::Blocked;
    return AccountStatus::Inactive;
}
