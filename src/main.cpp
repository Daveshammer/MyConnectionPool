#include <iostream>
#include <memory>
#include <thread>
// #include <chrono>
#include "MysqlConn.h"
#include "ConnectionPool.h"
using namespace std;

// 非连接池
void op1(int begin, int end)
{
    for (int i = begin; i < end; i ++)
    {
        MysqlConn conn;
        conn.connect("root", "123456", "test", "127.0.0.1", 3306);
        char sql[1024] = { 0 };
        snprintf(sql, sizeof(sql), "insert into user values(%d, 'zhang san', '221B')", i);
        conn.update(sql);
    }
}

void op2(int begin, int end)
{
    for (int i = begin; i < end; i ++)
    {
        shared_ptr<MysqlConn> conn = ConnectionPool::getConnectionPool()->getConnection();
        char sql[1024] = { 0 };
        snprintf(sql, sizeof(sql), "insert into user values(%d, 'zhang san', '221B')", i);
        conn->update(sql);
    }
}

// 非连接池单线程
void test1()
{
    steady_clock::time_point begin = steady_clock::now();
    // op1(0, 5000);
    op1(0, 50);
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "非连接池, 单线程, 用时: " << length.count() << " 纳秒, "
        << length.count() / 1000000 << " 毫秒" << endl;
}

// 连接池单线程
void test2()
{
    steady_clock::time_point begin = steady_clock::now();
    // op1(0, 5000);
    op2(0, 50);
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "连接池, 单线程, 用时: " << length.count() << " 纳秒, "
        << length.count() / 1000000 << " 毫秒" << endl;
}

// 非连接池，多线程
void test3()
{
    steady_clock::time_point begin = steady_clock::now();
    thread t1(op1, 0, 1000);
    thread t2(op1, 1000, 2000);
    thread t3(op1, 2000, 3000);
    thread t4(op1, 3000, 4000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "非连接池, 多线程, 用时: " << length.count() << " 纳秒, "
        << length.count() / 1000000 << " 毫秒" << endl;
}

void test4()
{
    steady_clock::time_point begin = steady_clock::now();
    thread t1(op2, 0, 1000);
    thread t2(op2, 1000, 2000);
    thread t3(op2, 2000, 3000);
    thread t4(op2, 3000, 4000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    steady_clock::time_point end = steady_clock::now();
    auto length = end - begin;
    cout << "连接池, 多线程, 用时: " << length.count() << " 纳秒, "
        << length.count() / 1000000 << " 毫秒" << endl;
}

// 查询测试
int query()
{
    MysqlConn conn;
    conn.connect("root", "123456", "test", "127.0.0.1");
    string sql = "insert into user values(1, 'zhang san', '221B')";
    bool flag = conn.update(sql);
    cout << "flag value:  " << flag << endl;

    sql = "select * from user";
    conn.query(sql);
    // 从结果集中取出一行
    while (conn.next())
    {
        // 打印每行字段值
        cout << conn.value(0) << ", "
            << conn.value(1) << ", "
            << conn.value(2) << ", "
            << conn.value(3) << endl;
    }
    return 0;
}

int main()
{
    // 非连接池，单线程
    test1();
    // 连接池，单线程
    test2();
    // 非连接池，多线程
    // test3();
    // 连接池，多线程
    // test4();

    return 0;
}