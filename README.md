# wavestory

## Authentication

The project now includes an `auth` subsystem providing sign-in/sign-up
functionality.  The high-level class `SignInSignUp` depends on the
`IUserStore` interface; a concrete `MySqlUserStore` currently implements
this interface.  This separation lets you change or mock the database
without touching business logic.

See `auth/users_table_schema.sql` for the required `users` table.

### Building

The Makefile now compiles the new authentication subsystem.  A standard
`make` invocation will build the executable; make sure your system has the
MySQL client library and OpenSSL headers installed, e.g. `libmysqlclient-dev`
and `libssl-dev` on Debian/Ubuntu.  If the headers are in a custom location,
adjust `INC_DIR` in the Makefile accordingly.

