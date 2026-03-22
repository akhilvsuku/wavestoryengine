# WaveStory Engine — Claude Code Guide

## Project Overview

WaveStory Engine is the C++ based backend web server for an editor application. Its primary purpose is to compare any kind of files using the **diff-match-patch** algorithm and send the diff results back to the UI application. It also handles user authentication and document management. The server exposes a REST API on port 8080 backed by MySQL for user storage.

## Architecture

```
wavestoryengine/
├── wstory.cpp            # Entry point — instantiates receivermain
├── receivermain.h/.cpp   # HTTP route handlers (port 8080)
├── auth/                 # Authentication subsystem
│   ├── IUserStore.h      # Abstract interface for user persistence
│   ├── SignInSignUp.h/.cpp   # Business logic, SHA256 password hashing
│   ├── MySqlUserStore.h/.cpp # MySQL implementation of IUserStore
│   └── users_table_schema.sql
├── common/               # Shared utilities
│   ├── Logger.h/.cpp     # Singleton async logger (thread-safe)
│   └── SharedVector.h    # Thread-safe vector template (used by Logger)
└── httplib/
    └── httplib.h         # cpp-httplib v0.18.5 (single-header, bundled)
```

## Build System

```bash
make          # Build → produces ./wstory executable
make clean    # Remove object files and binary
```

**Compiler**: `g++`, C++17 (`-std=c++17`), `-Wall -Wextra -pthread`

**Link flags**: `-lssl -lcrypto -lmysqlclient -pthread`

**Include paths**: `./httplib`, `./common`, `./auth`

**Source files compiled**: `wstory.cpp`, `receivermain.cpp`, `common/Logger.cpp`, `auth/SignInSignUp.cpp`, `auth/MySqlUserStore.cpp`

## Dependencies

| Library | Purpose | Install |
|---------|---------|---------|
| OpenSSL | SHA256 password hashing (`openssl/sha.h`) | `libssl-dev` |
| libmysqlclient | MySQL database access | `libmysqlclient-dev` |
| cpp-httplib | HTTP server (bundled in `httplib/httplib.h`) | Included |
| C++17 stdlib | `std::filesystem`, `std::thread`, `std::mutex`, `std::chrono` | Compiler |

## HTTP API Endpoints

| Method | Path | Handler | Description |
|--------|------|---------|-------------|
| GET | `/version` | `process_version()` | Returns `"v1.0.02"` |
| POST | `/singin_signup` | `singin_signup()` | User sign-in / sign-up |
| GET | `/fetch_search` | `fetch_search()` | Search (stub) |
| GET | `/save_doc` | `save_doc()` | Upload file to `/app/` |
| GET | `/load_doc` | `load_doc()` | Download file from `/app/` |

## Key Patterns

### Dependency Injection (auth layer)
`SignInSignUp` takes a `std::unique_ptr<IUserStore>` — swap `MySqlUserStore` for any other implementation.

### Singleton Logger
```cpp
Logger::getInstance().Log(Logger::Level::INFO, "message");
Logger::getInstance().SetPrintToConsole(true);
```

### Thread-Safe Queue
`SharedVector<T>` wraps `std::vector` with a mutex; used by Logger for async file I/O.

### Authentication Flow
1. `receivermain` constructs `MySqlUserStore` with DB credentials
2. Passes ownership to `SignInSignUp`
3. Endpoint handlers call `m_auth->signin()` / `m_auth->signup()`
4. Passwords stored as SHA256 hex digests via OpenSSL

## Database Schema

```sql
CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(64) NOT NULL UNIQUE,
    password VARCHAR(128) NOT NULL,
    last_password VARCHAR(128) NOT NULL,
    password_changed_time BIGINT NOT NULL,
    account_created_time BIGINT NOT NULL,
    account_status ENUM('Active', 'Inactive', 'Blocked') NOT NULL DEFAULT 'Active'
);
CREATE INDEX idx_username ON users(username);
```

## Docker

```bash
docker build -t wavestoryengine .
docker run -p 8080:8080 wavestoryengine
```

Base image: `ubuntu:20.04`. Installs `build-essential`, `g++`, `make`. Builds via `RUN make` and runs `./wstory`.

## Known Issues / Security Gaps

- **SQL Injection**: `MySqlUserStore` builds queries via string concatenation — should use prepared statements.
- **Hardcoded DB credentials**: `receivermain.cpp` constructor has literal `"dbuser"/"dbpass"/"dbname"` — move to env vars or config file.
- **Unrestricted file upload**: `save_doc()` writes to `/app/{filename}` without path traversal validation.
- **No input sanitization**: username/password not validated before use.

## Development Guidelines

Agent-specific guides live in `.github/agents/`:
- `coding-features.agent.md` — C++ feature development standards
- `code-review.agent.md` — Code review checklist
- `debugging.agent.md` — Debugging procedures
- `testing.agent.md` — Testing procedures

## Testing

No automated test framework is configured yet. Endpoints can be exercised with `curl`:

```bash
curl http://localhost:8080/version
curl -X POST http://localhost:8080/singin_signup -d '{"username":"u","password":"p"}'
```
