# cpp-sqlitelib

A single-file C++ SQLite wrapper library, header-only. This repository is a distribution version based on [cpp-sqlitelib](https://github.com/yhirose/cpp-sqlitelib). Unlike the original, this version wraps the encrypted database [sqlite3mc](https://github.com/utelle/SQLite3MultipleCiphers) and provides a `setPassword` function to interact with the encrypted database.

## Open database

```cpp
#include <sqlitelib.h>
using namespace sqlitelib;
auto db = Sqlite("./test.db");
```

## Create table

```cpp
db.execute(R"(
  CREATE TABLE IF NOT EXISTS people (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT,
    age INTEGER,
    data BLOB
  )
)");
```

## Drop table

```cpp
db.execute("DROP TABLE IF EXISTS people");
```

## Insert records

```cpp
auto stmt = db.prepare("INSERT INTO people (name, age, data) VALUES (?, ?, ?)");
stmt.execute("john", 10, vector<char>({ 'A', 'B', 'C', 'D' }));
stmt.execute("paul", 20, vector<char>({ 'E', 'B', 'G', 'H' }));
stmt.execute("mark", 15, vector<char>({ 'I', 'J', 'K', 'L' }));
stmt.execute("luke", 25, vector<char>({ 'M', 'N', 'O', 'P' }));
```

## Select a record (single colum)

```cpp
auto val = db.execute_value<int>("SELECT age FROM people WHERE name='john'");
val; // 10
```

## Select records (multiple columns)

```cpp
auto rows = db.execute<int, std::string>("SELECT age, name FROM people");
rows.size(); // 4

auto [age, name] = rows[3]; // age: 25, name: luke
```

## Bind #1

```cpp
auto stmt = db.prepare<std::string>("SELECT name FROM people WHERE age > ?");

auto rows = stmt.execute(10);
rows.size(); // 3
rows[0];     // paul

auto rows = stmt.bind(10).execute();
rows.size(); // 3
rows[0];     // paul
```

## Bind #2

```cpp
auto val = db.execute_value<int>("SELECT id FROM people WHERE name=? AND age=?", "john", 10);
val; // 1
```

## Cursor (multiple columns)

```cpp
auto stmt = db.prepare<std::string, int>("SELECT name, age FROM people");

auto cursor = stmt.execute_cursor();
for (auto it = cursor.begin(); it != cursor.end(); ++it) {
  get<0>(*it);
  get<1>(*it);
  ++it;
}

// With C++17 structured binding
for (const auto& [name, age] : stmt.execute_cursor()) {
  ;
}
```

## Cursor (single column)

```cpp
auto stmt = db.prepare<std::string>("SELECT name FROM people");

for (const auto& x: stmt.execute_cursor()) {
  ;
}
```

## Count

```cpp
auto val = db.execute_value<int>("SELECT COUNT(*) FROM people");
val; // 4
```

## Flat API

```cpp
for (const auto& [name, age] :
      db.execute_cursor<string, int>("SELECT name, age FROM people")) {
  ;
}

for (const auto& x: db.execute_cursor<std::string>("SELECT name FROM people")) {
  ;
}
```

## Encrypted Database

This library supports encrypted databases using [sqlite3mc](https://github.com/utelle/SQLite3MultipleCiphers), which supports multiple encryption algorithms:

- `aes128cbc`: AES 128 Bit CBC - No HMAC (wxSQLite3)
- `aes256cbc`: AES 256 Bit CBC - No HMAC (wxSQLite3)
- `chacha20`: ChaCha20 - Poly1305 HMAC (sqleet, default)
- `sqlcipher`: AES 256 Bit CBC - SHA1/SHA256/SHA512 HMAC (SQLCipher)
- `rc4`: RC4 (System.Data.SQLite)
- `ascon128`: Ascon-128 v1.2
- `aegis128`: AEGIS-128
- `aegis256`: AEGIS-256

### Set Password

```cpp
db.setPassword("your_password");
```

### Set Encryption Algorithm

```cpp
db.setCipher("aes256cbc");  // Set to AES 256 CBC
db.setPassword("your_password");
```

### Set KDF Iterations

```cpp
db.setKdfIter(10000);  // Set KDF iterations
db.setPassword("your_password");
```

**Important**: When reopening an encrypted database, you must set the same cipher and KDF parameters before calling `setPassword()`.

### Set Legacy Mode

```cpp
db.setLegacy(1);  // Enable legacy mode for compatibility
db.setPassword("your_password");
```

## Build

This library requires [sqlite3mc](https://github.com/utelle/SQLite3MultipleCiphers) to be installed.

### Install sqlite3mc

Download and compile sqlite3mc from [GitHub](https://github.com/utelle/SQLite3MultipleCiphers).

For Ubuntu/Debian:
```bash
# Clone and build sqlite3mc
git clone https://github.com/utelle/SQLite3MultipleCiphers.git
cd SQLite3MultipleCiphers
make
sudo make install
```

### Build the library

```bash
mkdir build
cd build
cmake ..
make
```

## Test

```bash
cd build
make test
```

## License

MIT license (© 2025 Mq-b)
