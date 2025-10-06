# cpp-sqlitelib

一个单文件的 C++ SQLite 包装库，头文件库。
本仓库基于 [cpp-sqlitelib](https://github.com/yhirose/cpp-sqlitelib)，是其一个发行版本。与原库不同的是，本版本包装了加密数据库 [sqlite3mc](https://github.com/utelle/SQLite3MultipleCiphers)，并提供了 `setPassword` 函数来操作加密数据库。

## 打开数据库

```cpp
#include <sqlitelib.h>
using namespace sqlitelib;
auto db = Sqlite("./test.db");
```

## 创建表

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

## 删除表

```cpp
db.execute("DROP TABLE IF EXISTS people");
```

## 插入记录

```cpp
auto stmt = db.prepare("INSERT INTO people (name, age, data) VALUES (?, ?, ?)");
stmt.execute("john", 10, vector<char>({ 'A', 'B', 'C', 'D' }));
stmt.execute("paul", 20, vector<char>({ 'E', 'B', 'G', 'H' }));
stmt.execute("mark", 15, vector<char>({ 'I', 'J', 'K', 'L' }));
stmt.execute("luke", 25, vector<char>({ 'M', 'N', 'O', 'P' }));
```

## 查询单列记录

```cpp
auto val = db.execute_value<int>("SELECT age FROM people WHERE name='john'");
val; // 10
```

## 查询多列记录

```cpp
auto rows = db.execute<int, std::string>("SELECT age, name FROM people");
rows.size(); // 4

auto [age, name] = rows[3]; // age: 25, name: luke
```

## 绑定参数 #1

```cpp
auto stmt = db.prepare<std::string>("SELECT name FROM people WHERE age > ?");

auto rows = stmt.execute(10);
rows.size(); // 3
rows[0];     // paul

auto rows = stmt.bind(10).execute();
rows.size(); // 3
rows[0];     // paul
```

## 绑定参数 #2

```cpp
auto val = db.execute_value<int>("SELECT id FROM people WHERE name=? AND age=?", "john", 10);
val; // 1
```

## 游标（多列）

```cpp
auto stmt = db.prepare<std::string, int>("SELECT name, age FROM people");

auto cursor = stmt.execute_cursor();
for (auto it = cursor.begin(); it != cursor.end(); ++it) {
  get<0>(*it);
  get<1>(*it);
  ++it;
}

// 使用 C++17 结构绑定
for (const auto& [name, age] : stmt.execute_cursor()) {
  ;
}
```

## 游标（单列）

```cpp
auto stmt = db.prepare<std::string>("SELECT name FROM people");

for (const auto& x: stmt.execute_cursor()) {
  ;
}
```

## 计数

```cpp
auto val = db.execute_value<int>("SELECT COUNT(*) FROM people");
val; // 4
```

## 简化 API

```cpp
for (const auto& [name, age] :
      db.execute_cursor<string, int>("SELECT name, age FROM people")) {
  ;
}

for (const auto& x: db.execute_cursor<std::string>("SELECT name FROM people")) {
  ;
}
```

## 加密数据库

本库支持使用 [sqlite3mc](https://github.com/utelle/SQLite3MultipleCiphers) 的加密数据库，支持多种加密算法：

- `aes128cbc`: AES 128 位 CBC - 无 HMAC (wxSQLite3)
- `aes256cbc`: AES 256 位 CBC - 无 HMAC (wxSQLite3)
- `chacha20`: ChaCha20 - Poly1305 HMAC (sqleet，默认)
- `sqlcipher`: AES 256 位 CBC - SHA1/SHA256/SHA512 HMAC (SQLCipher)
- `rc4`: RC4 (System.Data.SQLite)
- `ascon128`: Ascon-128 v1.2
- `aegis128`: AEGIS-128
- `aegis256`: AEGIS-256

### 设置密码

```cpp
db.setPassword("your_password");
```

### 设置加密算法

```cpp
db.setCipher("aes256cbc");  // 设置为 AES 256 CBC
db.setPassword("your_password");
```

### 设置 KDF 迭代次数

```cpp
db.setKdfIter(10000);  // 设置 KDF 迭代次数
db.setPassword("your_password");
```

**重要提示**：重新打开加密数据库时，必须在调用 `setPassword()` 之前设置相同的加密算法和 KDF 参数。

### 设置遗留模式

```cpp
db.setLegacy(1);  // 启用遗留模式以保持兼容性
db.setPassword("your_password");
```

## 编译

本库需要安装 [sqlite3mc](https://github.com/utelle/SQLite3MultipleCiphers)。

### 安装 sqlite3mc

从 [GitHub](https://github.com/utelle/SQLite3MultipleCiphers) 下载并编译 sqlite3mc。

对于 Ubuntu/Debian：
```bash
# 克隆并编译 sqlite3mc
git clone https://github.com/utelle/SQLite3MultipleCiphers.git
cd SQLite3MultipleCiphers
make
sudo make install
```

### 编译库

```bash
mkdir build
cd build
cmake ..
make
```

## 测试

```bash
cd build
make test
```

## 许可证

MIT 许可证 (© 2025 Mq-b)
