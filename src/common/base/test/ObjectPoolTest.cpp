/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/base/ObjectPool.h"

#include <atomic>
#include <thread>

#include <gtest/gtest.h>

namespace nebula {

static std::atomic<int> instances = 0;

class MyClass {
public:
    MyClass() {
        instances++;
    }
    ~MyClass() {
        instances--;
    }
};

TEST(ObjectPoolTest, TestPooling) {
    ASSERT_EQ(instances, 0);

    ObjectPool pool;
    ASSERT_NE(pool.add(new MyClass), nullptr);
    ASSERT_NE(pool.add(new MyClass), nullptr);
    ASSERT_EQ(instances, 2);

    pool.clear();
    ASSERT_EQ(instances, 0);
}

TEST(ObjectPoolTest, TestTransfer) {
    ObjectPool p1, p2;
    auto ptr = new MyClass;
    ASSERT_EQ(p1.add(ptr), ptr);
    auto status = p1.release(ptr);
    ASSERT(status.ok());
    ASSERT_EQ(p2.add(ptr, std::move(status).value()), ptr);

    p1.clear();
    p2.clear();
    ASSERT_EQ(instances, 0);
}

TEST(ObjectPoolTest, TestThread) {
    ObjectPool pool;
    std::thread t1([&pool]() {
        for (int i = 0; i < 10000; i++) {
            pool.add(new MyClass);
        }
    });
    std::thread t2([&pool]() {
        for (int i = 10000; i < 20000; i++) {
            pool.add(new MyClass);
        }
    });
    t1.join();
    t2.join();

    pool.clear();
    ASSERT_EQ(instances, 0);
}

}   // namespace nebula
