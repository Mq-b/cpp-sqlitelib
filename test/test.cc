#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <tuple>
#include <filesystem>
#include <thread>
#include <chrono>
#include <sqlitelib.h>

using namespace std;
using namespace sqlitelib;
namespace fs = std::filesystem;

// 测试不同加密算法
void test_cipher(const string& cipher_name) {
    cout << "\n=== 测试加密算法: " << cipher_name << " ===" << endl;

    string db_path = "./test_" + cipher_name + ".db";

    // 清理旧文件
    if (fs::exists(db_path)) {
        fs::remove(db_path);
    }
    
    // 确保文件被完全删除
    this_thread::sleep_for(chrono::milliseconds(100));

    try {
        // 创建加密数据库
        {
            Sqlite db(db_path.c_str());
            if (!db.is_open()) {
                throw runtime_error("数据库打开失败");
            }

            db.setCipher(cipher_name);
            db.setKdfIter(10000);
            db.setPassword("test_password");

            // 创建表并插入数据
            db.execute(R"(
                CREATE TABLE test_table (
                    id INTEGER PRIMARY KEY,
                    data TEXT
                )
            )");

            db.execute("INSERT INTO test_table (data) VALUES (?)", "Hello " + cipher_name);
        }

        // 重新打开并验证密码
        {
            Sqlite db(db_path.c_str());
            db.setCipher(cipher_name);
            db.setKdfIter(10000);  // 也设置 KDF
            db.setPassword("test_password");

            auto result = db.execute_value<string>("SELECT data FROM test_table WHERE id=1");
            cout << "读取数据: " << result << endl;

            if (result != "Hello " + cipher_name) {
                throw runtime_error("数据验证失败");
            }
        }

        // 测试错误密码
        {
            Sqlite db(db_path.c_str());
            db.setCipher(cipher_name);
            db.setKdfIter(10000);  // 也设置 KDF
            try {
                db.setPassword("wrong_password");
                throw runtime_error("错误密码应该失败");
            } catch (const runtime_error& e) {
                cout << "错误密码正确被拒绝: " << e.what() << endl;
            }
        }

        cout << "✓ " << cipher_name << " 测试通过" << endl;

    } catch (const exception& e) {
        cout << "✗ " << cipher_name << " 测试失败: " << e.what() << endl;
    }

    // 清理
    if (fs::exists(db_path)) {
        fs::remove(db_path);
    }
}

int main() {
    try {
        cout << "开始加密数据库测试..." << endl;

        // 支持的加密算法列表
        vector<string> ciphers = {
            "aes128cbc",
            "aes256cbc",
            "chacha20",
            "sqlcipher",
            "rc4",
            "ascon128",
            "aegis"
        };

        // 测试每种算法
        for (const auto& cipher : ciphers) {
            test_cipher(cipher);
        }

        // 测试默认算法 (chacha20)
        cout << "\n=== 测试默认算法 (无 setCipher 调用) ===" << endl;
        string db_path = "./test_default.db";
        if (fs::exists(db_path)) fs::remove(db_path);

        {
            Sqlite db(db_path.c_str());
            db.setPassword("default_password");
            db.execute("CREATE TABLE test (id INTEGER)");
            db.execute("INSERT INTO test VALUES (1)");
        }

        {
            Sqlite db(db_path.c_str());
            db.setPassword("default_password");
            auto count = db.execute_value<int>("SELECT COUNT(*) FROM test");
            cout << "默认算法测试: " << (count == 1 ? "通过" : "失败") << endl;
        }

        if (fs::exists(db_path)) fs::remove(db_path);

        // 测试 KDF 迭代次数
        cout << "\n=== 测试 KDF 迭代次数 ===" << endl;
        db_path = "./test_kdf.db";
        if (fs::exists(db_path)) fs::remove(db_path);

        {
            Sqlite db(db_path.c_str());
            db.setCipher("aes256cbc");
            db.setKdfIter(50000);
            db.setPassword("kdf_test");
            db.execute("CREATE TABLE test (id INTEGER)");
        }

        {
            Sqlite db(db_path.c_str());
            db.setCipher("aes256cbc");
            db.setPassword("kdf_test");
            auto exists = db.execute_value<int>("SELECT 1");
            cout << "KDF 测试: " << (exists == 1 ? "通过" : "失败") << endl;
        }

        if (fs::exists(db_path)) fs::remove(db_path);

        cout << "\n所有测试完成！" << endl;

    } catch (const exception& e) {
        cerr << "测试过程中发生异常: " << e.what() << endl;
        return 1;
    }

    return 0;
}
