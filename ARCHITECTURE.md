# ARCHITECTURE.md — WaveStory Engine

> **Scope:** Internal structure, data flow, and design decisions for contributors modifying the engine.
> For build commands, dependencies, and coding conventions see **[CLAUDE.md](CLAUDE.md)**.

---

## 1. Overview

WaveStory Engine is a single-process C++ HTTP server. It exposes a REST API on port 8080 for the frontend SPA. Its responsibilities are:

- **Document comparison** — delegates to the `dmp` (diff-match-patch) library
- **Authentication** — sign-in / sign-up with SHA256-hashed passwords persisted in MySQL
- **Document I/O** — save and load files from the local filesystem

The entire server runs in a single executable (`./wstory`), built from a handful of `.cpp` files with no runtime plugin loading.

---

## 2. Module Map

```
wavestoryengine/
├── wstory.cpp                # main() — constructs and starts receivermain
├── receivermain.h/.cpp       # HTTP route registration and request handlers
├── auth/
│   ├── IUserStore.h          # Abstract interface: findUser(), createUser()
│   ├── SignInSignUp.h/.cpp   # Business logic layer — orchestrates auth flow
│   ├── MySqlUserStore.h/.cpp # Concrete IUserStore backed by MySQL
│   └── users_table_schema.sql
├── common/
│   ├── Logger.h/.cpp         # Singleton async logger
│   └── SharedVector.h        # Thread-safe vector (used by Logger internally)
└── httplib/
    └── httplib.h             # cpp-httplib v0.18.5 — single-header HTTP server
```

---

## 3. Startup Sequence

```
main()  [wstory.cpp]
  └─ constructs receivermain
       ├─ constructs MySqlUserStore("dbuser", "dbpass", "dbname")
       ├─ constructs SignInSignUp(std::move(userStore))
       └─ registers HTTP routes on svr (httplib::Server)
            ├─ GET  /version
            ├─ POST /singin_signup
            ├─ GET  /fetch_search
            ├─ GET  /save_doc
            └─ GET  /load_doc
       └─ svr.listen("0.0.0.0", 8080)   [blocks forever]
```

The server is single-process; `cpp-httplib` handles per-request threading internally.

---

## 4. Request Handling Layer (`receivermain`)

`receivermain` owns the `httplib::Server` instance and all route handlers. Each handler is a lambda or member function registered via `svr.Get()` / `svr.Post()`.

For the full list of routes and their handler names see the **HTTP API Endpoints** table in [CLAUDE.md](CLAUDE.md).

All handlers receive `const httplib::Request&` and `httplib::Response&`. Responses are set directly on the response object.

---

## 5. Authentication Subsystem

### Layered Design

```
receivermain
    │  owns (unique_ptr)
    ▼
SignInSignUp          ← business logic, password hashing
    │  owns (unique_ptr<IUserStore>)
    ▼
IUserStore            ← abstract persistence interface
    ▲
MySqlUserStore        ← concrete MySQL implementation
```

**`IUserStore`** defines two operations:
- `findUser(username)` → returns user record or null
- `createUser(username, hashedPassword)` → inserts row, returns success

**`SignInSignUp`** contains all business logic:
1. Hash the incoming password with SHA256 (via OpenSSL `SHA256()`)
2. Call `m_store->findUser()` for sign-in; compare hashes
3. Call `m_store->createUser()` for sign-up

**`MySqlUserStore`** translates `IUserStore` calls to raw MySQL C API queries. All queries are currently built via string concatenation (SQL injection risk — see Known Issues in CLAUDE.md).

### Dependency Injection

`SignInSignUp` takes `std::unique_ptr<IUserStore>` in its constructor. To swap out MySQL for a different backend (e.g., SQLite, in-memory mock), implement `IUserStore` and pass it at construction time in `receivermain`.

---

## 6. Logging Subsystem

```
Logger::getInstance()          ← Meyer's singleton
    │
    ├─ Log(Level, message)     ← enqueues into SharedVector<LogEntry>
    │
    └─ background thread       ← drains SharedVector, writes to file
```

**`Logger`** is a thread-safe singleton. Log calls are non-blocking: entries are pushed onto `SharedVector<LogEntry>` (a mutex-guarded `std::vector`) and flushed to disk by a dedicated background thread.

**`SharedVector<T>`** is a generic thread-safe wrapper: `push()` and `pop_all()` operations are guarded by `std::mutex`. Used only by `Logger`.

For the usage API see the **Key Patterns** section in [CLAUDE.md](CLAUDE.md).

---

## 7. Document I/O

`save_doc` and `load_doc` operate on files in `/app/`:

- **save_doc**: reads the filename from the query string, writes body to `/app/{filename}`
- **load_doc**: reads the filename from the query string, returns file contents

No path traversal validation is performed. Filenames are used directly (see Known Issues in CLAUDE.md).

---

## 8. Threading Model

- `cpp-httplib` creates one thread per request internally.
- `receivermain` route handlers are called concurrently.
- `Logger` is the only shared mutable state — it is thread-safe via `SharedVector`.
- `MySqlUserStore` is **not** thread-safe — each request constructs its own MySQL connection (or reuses one with a potential race; review before adding concurrency).

---

## 9. Key Design Invariants

1. **`IUserStore` is the only persistence abstraction.** All database access must go through this interface. Do not call MySQL APIs directly from `receivermain` or `SignInSignUp`.

2. **`SignInSignUp` owns `IUserStore` via `unique_ptr`.** Lifetime is tied to the `SignInSignUp` object. Do not store raw pointers to the store.

3. **`Logger` is a singleton accessed only via `getInstance()`.** Do not construct `Logger` directly or pass it as a parameter.

4. **All HTTP route handlers are registered in `receivermain`.** Do not register routes from other classes or from `main()`.

5. **`httplib.h` is the only HTTP library.** Do not introduce Boost.Beast, libuv, or other networking libraries. The single-header approach keeps the build simple.

---

## 10. Extension Points

| To add... | Where |
|-----------|-------|
| New API endpoint | Register in `receivermain` constructor; add handler method |
| New auth backend | Implement `IUserStore`; pass to `SignInSignUp` in `receivermain` |
| New log destination | Extend `Logger` background thread flush logic |
| Document comparison logic | Add a handler that calls into `dmp` static library; link `libdiff_match_patch.a` in `Makefile` |
