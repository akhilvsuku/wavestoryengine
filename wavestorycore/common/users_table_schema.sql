-- SQL schema for users table used by SignInSignUp
CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(64) NOT NULL UNIQUE,
    password VARCHAR(128) NOT NULL,
    last_password VARCHAR(128) NOT NULL,
    password_changed_time BIGINT NOT NULL,
    account_created_time BIGINT NOT NULL,
    account_status ENUM('Active', 'Inactive', 'Blocked') NOT NULL DEFAULT 'Active'
);
-- Index for fast username lookup
CREATE INDEX idx_username ON users(username);
