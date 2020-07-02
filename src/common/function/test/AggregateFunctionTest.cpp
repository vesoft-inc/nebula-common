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
        vals1_ = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        vals2_ = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        vals2_.emplace_back(Value(NullType::__NULL__));
        vals2_.emplace_back(Value());
        vals3_ = std::vector<Value>(10, Value::kNullValue);
    }
protected:
    static std::vector<Value>  vals1_;
    static std::vector<Value>  vals2_;
    static std::vector<Value>  vals3_;
};

std::vector<Value>  AggregateFunctionTest::vals1_;
std::vector<Value>  AggregateFunctionTest::vals2_;
std::vector<Value>  AggregateFunctionTest::vals3_;

TEST_F(AggregateFunctionTest, Group) {
    auto group = AggFun::aggFunMap_[AggFun::Function::kNone]();
    group->apply(1);
    EXPECT_EQ(group->getResult(), 1);
}

TEST_F(AggregateFunctionTest, Count) {
    {
        auto cnt = AggFun::aggFunMap_[AggFun::Function::kCount]();
        for (auto& val : vals1_) {
            cnt->apply(val);
        }
        EXPECT_EQ(cnt->getResult(), 10);
    }
    {
        auto cnt = AggFun::aggFunMap_[AggFun::Function::kCount]();
        for (auto& val : vals2_) {
            cnt->apply(val);
        }
        EXPECT_EQ(cnt->getResult(), 10);
    }
    {
        auto cnt = AggFun::aggFunMap_[AggFun::Function::kCount]();
        for (auto& val : vals3_) {
            cnt->apply(val);
        }
        EXPECT_EQ(cnt->getResult(), 0);
    }
}

TEST_F(AggregateFunctionTest, Sum) {
    {
        auto sum = AggFun::aggFunMap_[AggFun::Function::kSum]();
        for (auto& val : vals1_) {
            sum->apply(val);
        }
        EXPECT_EQ(sum->getResult(), 45);
    }
    {
        auto sum = AggFun::aggFunMap_[AggFun::Function::kSum]();
        for (auto& val : vals2_) {
            sum->apply(val);
        }
        EXPECT_EQ(sum->getResult(), 45);
    }
    {
        auto sum = AggFun::aggFunMap_[AggFun::Function::kSum]();
        for (auto& val : vals3_) {
            sum->apply(val);
        }
        EXPECT_EQ(sum->getResult(), Value::kNullValue);
    }
}

TEST_F(AggregateFunctionTest, Avg) {
    {
        auto avg = AggFun::aggFunMap_[AggFun::Function::kAvg]();
        for (auto& val : vals1_) {
            avg->apply(val);
        }
        EXPECT_EQ(avg->getResult(), 4.5);
    }
    {
        auto avg = AggFun::aggFunMap_[AggFun::Function::kAvg]();
        for (auto& val : vals2_) {
            avg->apply(val);
        }
        EXPECT_EQ(avg->getResult(), 4.5);
    }
    {
        auto avg = AggFun::aggFunMap_[AggFun::Function::kAvg]();
        for (auto& val : vals3_) {
            avg->apply(val);
        }
        EXPECT_EQ(avg->getResult(), Value::kNullValue);
    }
}

TEST_F(AggregateFunctionTest, CountDistinct) {
    {
        auto ct = AggFun::aggFunMap_[AggFun::Function::kCountDist]();
        for (auto& val : vals1_) {
            ct->apply(val);
        }
        EXPECT_EQ(ct->getResult(), 10);
    }
    {
        auto ct = AggFun::aggFunMap_[AggFun::Function::kCountDist]();
        for (auto& val : vals2_) {
            ct->apply(val);
        }
        EXPECT_EQ(ct->getResult(), 10);
    }
    {
        auto ct = AggFun::aggFunMap_[AggFun::Function::kCountDist]();
        for (auto& val : vals3_) {
            ct->apply(val);
        }
        EXPECT_EQ(ct->getResult(), 0);
    }
    {
        auto ct = AggFun::aggFunMap_[AggFun::Function::kCountDist]();
        for (auto& val : {0, 0, 1, 1, 2, 2}) {
            ct->apply(val);
        }
        EXPECT_EQ(ct->getResult(), 3);
    }
}

TEST_F(AggregateFunctionTest, Max) {
    {
        auto max = AggFun::aggFunMap_[AggFun::Function::kMax]();
        for (auto& val : vals1_) {
            max->apply(val);
        }
        EXPECT_EQ(max->getResult(), 9);
    }
    {
        auto max = AggFun::aggFunMap_[AggFun::Function::kMax]();
        for (auto& val : vals2_) {
            max->apply(val);
        }
        EXPECT_EQ(max->getResult(), 9);
    }
    {
        auto max = AggFun::aggFunMap_[AggFun::Function::kMax]();
        for (auto& val : vals3_) {
            max->apply(val);
        }
        EXPECT_EQ(max->getResult(), Value::kNullValue);
    }
}

TEST_F(AggregateFunctionTest, Min) {
    {
        auto min = AggFun::aggFunMap_[AggFun::Function::kMin]();
        for (auto& val : vals1_) {
            min->apply(val);
        }
        EXPECT_EQ(min->getResult(), 0);
    }
    {
        auto min = AggFun::aggFunMap_[AggFun::Function::kMin]();
        for (auto& val : vals2_) {
            min->apply(val);
        }
        EXPECT_EQ(min->getResult(), 0);
    }
    {
        auto min = AggFun::aggFunMap_[AggFun::Function::kMin]();
        for (auto& val : vals3_) {
            min->apply(val);
        }
        EXPECT_EQ(min->getResult(), Value::kNullValue);
    }
}

TEST_F(AggregateFunctionTest, Stdev) {
    {
        auto stdev = AggFun::aggFunMap_[AggFun::Function::kStdev]();
        for (auto& val : vals1_) {
            stdev->apply(val);
        }
        EXPECT_EQ(stdev->getResult(), 2.87228132327);
    }
    {
        auto stdev = AggFun::aggFunMap_[AggFun::Function::kStdev]();
        for (auto& val : vals2_) {
            stdev->apply(val);
        }
        EXPECT_EQ(stdev->getResult(), 2.87228132327);
    }
    {
        auto stdev = AggFun::aggFunMap_[AggFun::Function::kStdev]();
        for (auto& val : vals3_) {
            stdev->apply(val);
        }
        EXPECT_EQ(stdev->getResult(), Value::kNullValue);
    }
}

TEST_F(AggregateFunctionTest, BitAnd) {
    {
        auto bitAnd = AggFun::aggFunMap_[AggFun::Function::kBitAnd]();
        for (auto& val : vals1_) {
            bitAnd->apply(val);
        }
        EXPECT_EQ(bitAnd->getResult(), 0);
    }
    {
        auto bitAnd = AggFun::aggFunMap_[AggFun::Function::kBitAnd]();
        for (auto& val : vals2_) {
            bitAnd->apply(val);
        }
        EXPECT_EQ(bitAnd->getResult(), 0);
    }
    {
        auto bitAnd = AggFun::aggFunMap_[AggFun::Function::kBitAnd]();
        for (auto& val : vals3_) {
            bitAnd->apply(val);
        }
        EXPECT_EQ(bitAnd->getResult(), Value::kNullValue);
    }
}

TEST_F(AggregateFunctionTest, BitOr) {
    {
        auto bitOr = AggFun::aggFunMap_[AggFun::Function::kBitOr]();
        for (auto& val : vals1_) {
            bitOr->apply(val);
        }
        EXPECT_EQ(bitOr->getResult(), 15);
    }
    {
        auto bitOr = AggFun::aggFunMap_[AggFun::Function::kBitOr]();
        for (auto& val : vals2_) {
            bitOr->apply(val);
        }
        EXPECT_EQ(bitOr->getResult(), 15);
    }
    {
        auto bitOr = AggFun::aggFunMap_[AggFun::Function::kBitOr]();
        for (auto& val : vals3_) {
            bitOr->apply(val);
        }
        EXPECT_EQ(bitOr->getResult(), Value::kNullValue);
    }
}

TEST_F(AggregateFunctionTest, BitXor) {
    {
        auto bitXor = AggFun::aggFunMap_[AggFun::Function::kBitXor]();
        for (auto& val : vals1_) {
            bitXor->apply(val);
        }
        EXPECT_EQ(bitXor->getResult(), 1);
    }
    {
        auto bitXor = AggFun::aggFunMap_[AggFun::Function::kBitXor]();
        for (auto& val : vals2_) {
            bitXor->apply(val);
        }
        EXPECT_EQ(bitXor->getResult(), 1);
    }
    {
        auto bitXor = AggFun::aggFunMap_[AggFun::Function::kBitXor]();
        for (auto& val : vals3_) {
            bitXor->apply(val);
        }
        EXPECT_EQ(bitXor->getResult(), Value::kNullValue);
    }
}
}  // namespace nebula
