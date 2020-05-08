/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */


#include <gtest/gtest.h>
#include "expression/ArithmeticExpression.h"
#include "expression/ConstantExpression.h"

namespace nebula {

class ExpressionTest : public ::testing::Test {
public:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ExpressionTest, Constant) {
    {
        ConstantExpression integer(1);
        auto eval = Expression::eval(&integer);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        ConstantExpression doubl(1.0);
        auto eval = Expression::eval(&doubl);
        EXPECT_EQ(eval.type(), Value::Type::FLOAT);
        EXPECT_EQ(eval, 1.0);
    }
    {
        ConstantExpression boolean(true);
        auto eval = Expression::eval(&boolean);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        ConstantExpression boolean(false);
        auto eval = Expression::eval(&boolean);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        ConstantExpression str("abcd");
        auto eval = Expression::eval(&str);
        EXPECT_EQ(eval.type(), Value::Type::STRING);
        EXPECT_EQ(eval, "abcd");
    }
    // TODO: EMPTY, NULL, DATE, DATETIME, VERTEX, EDGE, PATH, LIST, MAP, SET, DATASET
}

TEST_F(ExpressionTest, Arithmetics) {
    {
        ArithmeticExpression add(
                Expression::Type::EXP_ADD, new ConstantExpression(1), new ConstantExpression(2));
        EXPECT_EQ(Expression::eval(&add), 3);
    }
    {
        ArithmeticExpression add(
                Expression::Type::EXP_ADD,
                new ArithmeticExpression(
                    Expression::Type::EXP_ADD,
                    new ConstantExpression(1), new ConstantExpression(2)),
                new ConstantExpression(3));
        EXPECT_EQ(Expression::eval(&add), 6);
    }
}
}  // namespace nebula
