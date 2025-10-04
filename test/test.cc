#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <tuple>
#include <sqlitelib.h>

using namespace std;
using namespace sqlitelib;

int main() {
    try {
        Sqlite db("./test.db");
        if (!db.is_open()) {
            cerr << "数据库打开失败！" << endl;
            return 1;
        }

        // 可以设置密码
        db.setPassword("123555");

        // 建表
        db.prepare(R"(
            CREATE TABLE IF NOT EXISTS people (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT,
                age INTEGER,
                data BLOB
            )
        )").execute();

        // 插入数据
        auto stmt = db.prepare("INSERT INTO people (name, age, data) VALUES (?, ?, ?)");
        stmt.execute("john", 10, vector<char>{'A', 'B', 'C', 'D'});
        stmt.execute("paul", 20, vector<char>{'E', 'B', 'G', 'H'});
        stmt.execute("mark", 15, vector<char>{'I', 'J', 'K', 'L'});
        stmt.execute("luke", 25, vector<char>{'M', 'N', 'O', 'P'});

        cout << "数据插入完成\n";

        // 查询单值
        auto age_john = db.prepare<int>("SELECT age FROM people WHERE name='john'")
                            .execute_value();
        cout << "john 的 age = " << age_john << endl;

        auto name_john = db.prepare<string>("SELECT name FROM people WHERE name='john'")
                             .execute_value();
        cout << "john 的 name = " << name_john << endl;

        // 查询 BLOB
        auto data_john = db.prepare<vector<char>>("SELECT data FROM people WHERE name='john'")
                             .execute_value();
        cout << "john 的数据长度 = " << data_john.size()
             << ", 首字母 = " << data_john[0]
             << ", 末字母 = " << data_john.back() << endl;

        // 多列查询
        auto rows = db.prepare<int, string>("SELECT age, name FROM people").execute();
        cout << "people 表共有 " << rows.size() << " 行\n";
        for (auto &[age, name] : rows) {
            cout << "  " << name << " - " << age << endl;
        }

        // 条件查询
        auto stmt_cond = db.prepare<string>("SELECT name FROM people WHERE age > ?");
        auto rows_cond = stmt_cond.execute(10);
        cout << "年龄 >10 的人有 " << rows_cond.size() << " 个，第一个是 " << rows_cond[0] << endl;

        // 使用 execute_cursor 遍历
        cout << "\n使用游标遍历:\n";
        auto cursor = db.execute_cursor<string, int>("SELECT name, age FROM people");
        for (const auto& row : cursor) {
            auto [name, age] = row;
            cout << "  " << name << " - " << age << endl;
        }

        // 统计行数
        auto count = db.prepare<int>("SELECT COUNT(*) FROM people").execute_value();
        cout << "总共有 " << count << " 条记录" << endl;

        cout << "程序运行结束\n";
    }
    catch (const std::exception &ex) {
        cerr << "发生异常: " << ex.what() << endl;
        return 1;
    }
}
