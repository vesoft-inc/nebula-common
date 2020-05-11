/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */


#include <gtest/gtest.h>
#include "datatypes/List.h"
#include "expression/test/ExpressionContextMock.h"
#include "expression/ArithmeticExpression.h"
#include "expression/ConstantExpression.h"
#include "expression/SymbolPropertyExpression.h"
#include "expression/RelationalExpression.h"

namespace nebula {

class ExpressionTest : public ::testing::Test {
public:
    void SetUp() override {
        expCtxt_ = std::make_unique<ExpressionContextMock>();
    }
    void TearDown() override {}

protected:
    std::unique_ptr<ExpressionContext>  expCtxt_;
};

TEST_F(ExpressionTest, Constant) {
    {
        ConstantExpression integer(1);
        auto eval = integer.eval();
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        ConstantExpression doubl(1.0);
        auto eval = doubl.eval();
        EXPECT_EQ(eval.type(), Value::Type::FLOAT);
        EXPECT_EQ(eval, 1.0);
    }
    {
        ConstantExpression boolean(true);
        auto eval = boolean.eval();
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        ConstantExpression boolean(false);
        auto eval = boolean.eval();
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        ConstantExpression str("abcd");
        auto eval = str.eval();
        EXPECT_EQ(eval.type(), Value::Type::STRING);
        EXPECT_EQ(eval, "abcd");
    }
    // TODO: EMPTY, NULL, DATE, DATETIME, VERTEX, EDGE, PATH, LIST, MAP, SET, DATASET
}

TEST_F(ExpressionTest, Arithmetics) {
    {
        ArithmeticExpression add(
                Expression::Type::EXP_ADD, new ConstantExpression(1), new ConstantExpression(2));
        EXPECT_EQ(add.eval(), 3);
    }
    {
        ArithmeticExpression add(
                Expression::Type::EXP_ADD,
                new ArithmeticExpression(Expression::Type::EXP_ADD,
                        new ConstantExpression(1), new ConstantExpression(2)),
                new ConstantExpression(3));
        EXPECT_EQ(add.eval(), 6);
    }
}

TEST_F(ExpressionTest, RelationIN) {
    {
        RelationalExpression expr(
                Expression::Type::EXP_REL_IN,
                new ConstantExpression(1),
                new ConstantExpression(Value(List(std::vector<Value>{1, 2, 3}))));
        expr.setExpCtxt(expCtxt_.get());
        auto eval = expr.eval();
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        RelationalExpression expr(
                Expression::Type::EXP_REL_IN,
                new ConstantExpression(5),
                new ConstantExpression(Value(List(std::vector<Value>{1, 2, 3}))));
        expr.setExpCtxt(expCtxt_.get());
        auto eval = expr.eval();
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
}

// TODO(cpw): more test cases.
}  // namespace nebula
