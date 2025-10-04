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

## 许可证

MIT 许可证 (© 2025 Mq-b)
