/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <gtest/gtest.h>
#include "common/function/AggregateFunction.h"

namespace nebula {
class AggregateFunctionTest : public ::testing::Test {
public:
    static void SetUpTestCase() {
        vals_ = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    }
protected:
    static std::vector<Value>  vals_;
};

std::vector<Value>  AggregateFunctionTest::vals_;

TEST_F(AggregateFunctionTest, Group) {
    auto group = AggFun::aggFunMap_[""]();
    group->apply(1);
    EXPECT_EQ(group->getResult(), 1);
}

TEST_F(AggregateFunctionTest, Count) {
    auto cnt = AggFun::aggFunMap_[kCount]();
    for (auto& val : vals_) {
        cnt->apply(val);
    }
    EXPECT_EQ(cnt->getResult(), 10);
}

TEST_F(AggregateFunctionTest, Sum) {
    auto sum = AggFun::aggFunMap_[kSum]();
    for (auto& val : vals_) {
        sum->apply(val);
    }
    EXPECT_EQ(sum->getResult(), 45);
}

TEST_F(AggregateFunctionTest, Avg) {
    auto avg = AggFun::aggFunMap_[kAvg]();
    for (auto& val : vals_) {
        avg->apply(val);
    }
    EXPECT_EQ(avg->getResult(), 4.5);
}

TEST_F(AggregateFunctionTest, CountDistinct) {
    {
        auto ct = AggFun::aggFunMap_[kCountDist]();
        for (auto& val : vals_) {
            ct->apply(val);
        }
        EXPECT_EQ(ct->getResult(), 10);
    }
    {
        auto ct = AggFun::aggFunMap_[kCountDist]();
        for (auto& val : {0, 0, 1, 1, 2, 2}) {
            ct->apply(val);
        }
        EXPECT_EQ(ct->getResult(), 3);
    }
}

TEST_F(AggregateFunctionTest, Max) {
    auto max = AggFun::aggFunMap_[kMax]();
    for (auto& val : vals_) {
        max->apply(val);
    }
    EXPECT_EQ(max->getResult(), 9);
}

TEST_F(AggregateFunctionTest, Min) {
    auto min = AggFun::aggFunMap_[kMin]();
    for (auto& val : vals_) {
        min->apply(val);
    }
    EXPECT_EQ(min->getResult(), 0);
}

TEST_F(AggregateFunctionTest, Stdev) {
    auto stdev = AggFun::aggFunMap_[kStd]();
    for (auto& val : vals_) {
        stdev->apply(val);
    }
    EXPECT_EQ(stdev->getResult(), 2.87228132327);
}
}  // namespace nebula
