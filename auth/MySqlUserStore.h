#pragma once
#include "IUserStore.h"
#include <mysql/mysql.h>
#include <optional>

class MySqlUserStore : public IUserStore {
public:
    MySqlUserStore(const std::string& host,
                   const std::string& user,
                   const std::string& pass,
                   const std::string& db);
    ~MySqlUserStore() override;

    bool userExists(const std::string& username) override;
    bool insertUser(const UserRecord& rec) override;
    std::optional<UserRecord> fetchUser(const std::string& username) override;
    bool updatePassword(const std::string& username,
                        const std::string& newHash,
                        long long changedTime) override;

private:
    MYSQL* conn;
};