/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <gtest/gtest.h>
#include "function/FunctionManager.h"
#include "datatypes/DataSet.h"
#include "datatypes/List.h"

namespace nebula {
TEST(FunctionTest, collect) {
    auto result = FunctionManager::get("collect", 1);
    ASSERT_TRUE(result.ok());
    auto func = result.value();

    DataSet ds;
    ds.colNames = {"p1"};
    ds.rows.reserve(10);
    std::vector<Value> expected;
    expected.reserve(10);
    for (auto i = 0; i < 10; ++i) {
        Row row;
        row.columns.emplace_back(i);
        auto cols = row.columns;
        List tmp(std::move(cols));
        expected.emplace_back(std::move(tmp));
        ds.rows.emplace_back(std::move(row));
    }
    Value arg(std::move(ds));
    std::vector<const Value*> args{&arg};
    auto ret = func(args);
    ASSERT_EQ(ret.type(), Value::Type::LIST);
    ASSERT_EQ(ret.getList().values, expected);
}
}  // namespace nebula
