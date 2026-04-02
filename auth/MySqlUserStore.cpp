#include "MySqlUserStore.h"
#include <mysql/mysql.h>
#include <cstdlib>

MySqlUserStore::MySqlUserStore(const std::string& host,
                               const std::string& user,
                               const std::string& pass,
                               const std::string& db) {
    conn = mysql_init(nullptr);
    if (!mysql_real_connect(conn, host.c_str(), user.c_str(), pass.c_str(), db.c_str(), 0, nullptr, 0)) {
        // handle connection error appropriately in real code
    }
}

MySqlUserStore::~MySqlUserStore() {
    if (conn) mysql_close(conn);
}

bool MySqlUserStore::userExists(const std::string& username) {
    std::string query = "SELECT COUNT(*) FROM users WHERE username='" + username + "'";
    if (mysql_query(conn, query.c_str())) return false;
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return false;
    MYSQL_ROW row = mysql_fetch_row(res);
    bool exists = (row && std::stoi(row[0]) > 0);
    mysql_free_result(res);
    return exists;
}

bool MySqlUserStore::insertUser(const UserRecord& rec) {
    std::string query = "INSERT INTO users (username, password, last_password, password_changed_time, account_created_time, account_status) VALUES ('" +
                        rec.username + "', '" + rec.passwordHash + "', '" + rec.lastPasswordHash + "', " +
                        std::to_string(rec.passwordChangedTime) + ", " + std::to_string(rec.accountCreatedTime) + ", '" +
                        rec.accountStatus + "')";
    return mysql_query(conn, query.c_str()) == 0;
}

std::optional<UserRecord> MySqlUserStore::fetchUser(const std::string& username) {
    std::string query = "SELECT username, password, last_password, password_changed_time, account_created_time, account_status FROM users WHERE username='" + username + "'";
    if (mysql_query(conn, query.c_str())) return std::nullopt;
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) return std::nullopt;
    MYSQL_ROW row = mysql_fetch_row(res);
    std::optional<UserRecord> out;
    if (row) {
        out.emplace();
        out->username = row[0];
        out->passwordHash = row[1];
        out->lastPasswordHash = row[2];
        out->passwordChangedTime = std::stoll(row[3]);
        out->accountCreatedTime = std::stoll(row[4]);
        out->accountStatus = row[5];
    }
    mysql_free_result(res);
    return out;
}

bool MySqlUserStore::updatePassword(const std::string& username,
                                    const std::string& newHash,
                                    long long changedTime) {
    std::string query = "UPDATE users SET password='" + newHash + "', password_changed_time=" +
                        std::to_string(changedTime) + " WHERE username='" + username + "'";
    return mysql_query(conn, query.c_str()) == 0;
}
