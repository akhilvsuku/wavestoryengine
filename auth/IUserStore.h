#pragma once
#include <string>
#include <optional>

// Interface for user persistence.  Any underlying database or storage
// system (MySQL, PostgreSQL, cloud DB, in‑memory, etc.) can provide an
// implementation of this interface.  Higher‑level code talks only to
// IUserStore, making it easy to swap or mock the storage layer.

struct UserRecord {
    std::string username;
    std::string passwordHash;
    std::string lastPasswordHash;
    long long passwordChangedTime;
    long long accountCreatedTime;
    std::string accountStatus; // "Active", "Inactive", "Blocked"
};

class IUserStore {
public:
    virtual ~IUserStore() = default;

    virtual bool userExists(const std::string& username) = 0;
    virtual bool insertUser(const UserRecord& rec) = 0;
    virtual std::optional<UserRecord> fetchUser(const std::string& username) = 0;
    virtual bool updatePassword(const std::string& username,
                                const std::string& newHash,
                                long long changedTime) = 0;
};
