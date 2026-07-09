#include "../src/common.h"
#include "../src/parse.h"
#include "../src/config.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>

// 简单的宏断言
static int test_passed = 0;
static int test_failed = 0;

#define CHECK(cond, msg) do { \
    if (!(cond)) { \
        std::printf("  FAIL: " msg " (%s:%d)\n", __FILE__, __LINE__); \
        test_failed++; \
    } else { \
        test_passed++; \
    } \
} while(0)

// CHECK_EQ — 比较两个值是否相等（int/long long/timeval 等可 == 比较的类型）
#define CHECK_EQ(a, b, msg) do { \
    if ((a) != (b)) { \
        std::printf("  FAIL: " msg " — expected %lld, got %lld (%s:%d)\n", \
                    (long long)(b), (long long)(a), __FILE__, __LINE__); \
        test_failed++; \
    } else { \
        test_passed++; \
    } \
} while(0)

// 辅助：检查表达式是否抛出 XCException
#define CHECK_THROW(expr, msg) do { \
    bool threw = false; \
    try { expr; } \
    catch (const XCException&) { threw = true; } \
    if (!threw) { \
        std::printf("  FAIL: " msg " — expected XCException, none thrown (%s:%d)\n", \
                    __FILE__, __LINE__); \
        test_failed++; \
    } else { \
        test_passed++; \
    } \
} while(0)

// 辅助：创建临时文件内容
static bool write_file(const std::string& path, const std::string& content) {
    std::ofstream f(path);
    if (!f) return false;
    f << content;
    return true;
}

// ========== Parse::is_number 测试 ==========
void test_is_number() {
    std::printf("[Parse::is_number]\n");
    CHECK(Parse::is_number("0"),         "\"0\" should be number");
    CHECK(Parse::is_number("123"),       "\"123\" should be number");
    CHECK(Parse::is_number("-1"),        "\"-1\" should be number");
    CHECK(Parse::is_number("2147483647"),"\"2147483647\" should be number");
    CHECK(Parse::is_number("-2147483648"),"\"-2147483648\" should be number");
    CHECK(!Parse::is_number(""),         "empty string should NOT be number");
    CHECK(!Parse::is_number("abc"),      "\"abc\" should NOT be number");
    CHECK(!Parse::is_number("-"),        "\"-\" should NOT be number");
    CHECK(!Parse::is_number("12a"),      "\"12a\" should NOT be number");
    CHECK(!Parse::is_number(" 1"),       "\" 1\" should NOT be number (leading space)");
    CHECK(!Parse::is_number("1 "),       "\"1 \" should NOT be number (trailing space)");
    CHECK(Parse::is_number("-0"),        "\"-0\" should be number");
}

// ========== Parse::number 测试 ==========
void test_number() {
    std::printf("[Parse::number]\n");
    CHECK_EQ(Parse::number({"cmd", "42"}, 1), 42, "number({\"cmd\",\"42\"},1)");
    CHECK_EQ(Parse::number({"cmd", "-5"}, 1), -5, "number({\"cmd\",\"-5\"},1)");
    CHECK_THROW(Parse::number({"cmd", "abc"}, 1), "number({\"cmd\",\"abc\"},1) should throw");
    CHECK_THROW(Parse::number({"cmd"}, 1), "number({\"cmd\"},1) should throw (missing arg)");
    CHECK_EQ(Parse::number({"cmd", "0"}, 1), 0, "number({\"cmd\",\"0\"},1)");
    CHECK_EQ(Parse::number({"cmd", "100"}, 1), 100, "number({\"cmd\",\"100\"},1)");
}

// ========== Parse::range 测试 ==========
void test_range() {
    std::printf("[Parse::range]\n");
    CHECK_EQ(Parse::range({"cmd", "5"}, 1, 1, 10), 5, "range 5 in [1,10]");
    CHECK_THROW(Parse::range({"cmd", "0"}, 1, 1, 10), "range 0 in [1,10] should throw");
    CHECK_THROW(Parse::range({"cmd", "11"}, 1, 1, 10), "range 11 in [1,10] should throw");
    CHECK_EQ(Parse::range({"cmd", "1"}, 1, 1, 10), 1, "range 1 in [1,10] (lower bound)");
    CHECK_EQ(Parse::range({"cmd", "10"}, 1, 1, 10), 10, "range 10 in [1,10] (upper bound)");
    CHECK_THROW(Parse::range({"cmd", "abc"}, 1, 1, 10), "range abc should throw");
}

// ========== Parse::least 测试 ==========
void test_least() {
    std::printf("[Parse::least]\n");
    // 不抛异常
    try { Parse::least({"cmd", "a"}, 1); test_passed++; }
    catch (...) { std::printf("  FAIL: least with 1 arg should not throw (%s:%d)\n", __FILE__, __LINE__); test_failed++; }
    // 抛异常
    CHECK_THROW(Parse::least({"cmd"}, 1), "least with 0 args, id=1 should throw");
    // id=0 时，cmd size=1 够用
    try { Parse::least({"cmd"}, 0); test_passed++; }
    catch (...) { std::printf("  FAIL: least with 1 arg id=0 should not throw (%s:%d)\n", __FILE__, __LINE__); test_failed++; }
}

// ========== diff_Z 测试 ==========
void test_diff_Z() {
    std::printf("[diff_Z]\n");
    // 相同文件
    CHECK(write_file("/tmp/duipai_test_a.txt", "hello\nworld\n"), "write test file a");
    CHECK(write_file("/tmp/duipai_test_b.txt", "hello\nworld\n"), "write test file b");
    CHECK_EQ(diff_Z("/tmp/duipai_test_a.txt", "/tmp/duipai_test_b.txt"), 0, "identical files");

    // 不同内容
    CHECK(write_file("/tmp/duipai_test_b.txt", "hello\nuniverse\n"), "write diff file");
    CHECK_EQ(diff_Z("/tmp/duipai_test_a.txt", "/tmp/duipai_test_b.txt"), 1, "different files");

    // 行数不同
    CHECK(write_file("/tmp/duipai_test_b.txt", "hello\n"), "write shorter file");
    CHECK_EQ(diff_Z("/tmp/duipai_test_a.txt", "/tmp/duipai_test_b.txt"), 1, "different line count");

    // 行尾空白差异（忽略）
    CHECK(write_file("/tmp/duipai_test_a.txt", "hello   \nworld\t\n"), "write with trailing spaces");
    CHECK(write_file("/tmp/duipai_test_b.txt", "hello\nworld\n"), "write without trailing spaces");
    CHECK_EQ(diff_Z("/tmp/duipai_test_a.txt", "/tmp/duipai_test_b.txt"), 0, "trailing whitespace ignored");

    // 空文件 vs 空文件
    CHECK(write_file("/tmp/duipai_test_a.txt", ""), "write empty file a");
    CHECK(write_file("/tmp/duipai_test_b.txt", ""), "write empty file b");
    CHECK_EQ(diff_Z("/tmp/duipai_test_a.txt", "/tmp/duipai_test_b.txt"), 0, "both empty files");

    // 空文件 vs 非空
    CHECK(write_file("/tmp/duipai_test_b.txt", "x\n"), "write non-empty file");
    CHECK_EQ(diff_Z("/tmp/duipai_test_a.txt", "/tmp/duipai_test_b.txt"), 1, "empty vs non-empty");

    // 打开失败
    CHECK_EQ(diff_Z("/tmp/duipai_test_nonexistent.txt", "/tmp/duipai_test_b.txt"), 1,
             "non-existent file returns 1");
}

// ========== copy_file 测试 ==========
void test_copy_file() {
    std::printf("[copy_file]\n");
    CHECK(write_file("/tmp/duipai_copy_src.txt", "test content 123\n"), "write source file");
    CHECK(copy_file("/tmp/duipai_copy_src.txt", "/tmp/duipai_copy_dst.txt"), "copy_file should succeed");
    // 验证内容一致（用 ifstream 和 getline 验证）
    {
        std::ifstream dst("/tmp/duipai_copy_dst.txt");
        std::string content;
        std::getline(dst, content);
        CHECK(content == "test content 123", "copied content matches");
    }
    // 源文件不存在
    CHECK(!copy_file("/tmp/duipai_copy_nonexistent.txt", "/tmp/duipai_copy_dst2.txt"), "copy non-existent returns false");
}

// ========== to_us 测试 ==========
void test_to_us() {
    std::printf("[to_us]\n");
    timeval tv;
    tv.tv_sec = 1; tv.tv_usec = 500000;
    CHECK_EQ(to_us(tv), 1500000LL, "1s + 500000us = 1500000us");
    tv.tv_sec = 0; tv.tv_usec = 0;
    CHECK_EQ(to_us(tv), 0LL, "0s + 0us = 0us");
    tv.tv_sec = 0; tv.tv_usec = 999999;
    CHECK_EQ(to_us(tv), 999999LL, "0s + 999999us = 999999us");
    tv.tv_sec = 10; tv.tv_usec = 1;
    CHECK_EQ(to_us(tv), 10000001LL, "10s + 1us = 10000001us");
}

// ========== Config::read/save 测试 ==========
void test_config() {
    std::printf("[Config::read/save]\n");
    // 备份当前 config 文件
    bool has_config = false;
    {
        std::ifstream f("config");
        if (f) has_config = true;
    }
    if (has_config) {
        std::rename("config", "config.testbak");
    }

    // 测试：保存值后读取回来一致
    Config::threads = 8;
    Config::time_limit = 5000;
    Config::mem_limit = 256;
    Config::save();

    // 重置为默认值
    Config::threads = 4;
    Config::time_limit = -1;
    Config::mem_limit = -1;

    Config::read();
    CHECK_EQ(Config::threads, 8, "threads restored to 8");
    CHECK_EQ(Config::time_limit, 5000, "time_limit restored to 5000");
    CHECK_EQ(Config::mem_limit, 256, "mem_limit restored to 256");

    // 测试极端值
    Config::threads = 4096;
    Config::time_limit = 2147483647;
    Config::mem_limit = -2147483648;
    Config::save();
    Config::read();
    CHECK_EQ(Config::threads, 4096, "threads extreme value");
    CHECK_EQ(Config::time_limit, 2147483647, "time_limit INT_MAX");
    CHECK_EQ(Config::mem_limit, -2147483648, "mem_limit INT_MIN");

    // 恢复原始 config 文件
    std::remove("config");
    if (has_config) {
        std::rename("config.testbak", "config");
    } else {
        std::remove("config.testbak");
    }
}

int main() {
    std::printf("=== XC-duipai unit tests ===\n\n");

    test_is_number();
    test_number();
    test_range();
    test_least();
    test_diff_Z();
    test_copy_file();
    test_to_us();
    test_config();

    // 清理临时文件
    std::remove("/tmp/duipai_test_a.txt");
    std::remove("/tmp/duipai_test_b.txt");
    std::remove("/tmp/duipai_copy_src.txt");
    std::remove("/tmp/duipai_copy_dst.txt");

    int total = test_passed + test_failed;
    std::printf("\n=== Results: %d/%d passed, %d failed ===\n",
                test_passed, total, test_failed);
    return test_failed > 0 ? 1 : 0;
}