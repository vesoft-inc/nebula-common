/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */
#include "common/expression/test/TestBase.h"

namespace nebula {

class ColumnExpressionTest : public ExpressionTest {};

TEST_F(ExpressionTest, ColumnExpression) {
    {
        auto expr = ColumnExpression::make(&pool, 2);
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr = ColumnExpression::make(&pool, 0);
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::__EMPTY__);
        EXPECT_EQ(eval, Value::kEmpty);
    }
    {
        auto expr = ColumnExpression::make(&pool, -1);
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::EDGE);
        EXPECT_EQ(eval, Edge("3", "4", 1, "edge", 0, {}));
    }
    {
        auto expr = ColumnExpression::make(&pool, -3);
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::EDGE);
        EXPECT_EQ(eval, Edge("2", "3", 1, "edge", 0, {}));
    }
    {
        auto expr = ColumnExpression::make(&pool, 8);
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    }
    {
        auto expr = ColumnExpression::make(&pool, -8);
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval, 13);
    }
    {
        auto expr = ColumnExpression::make(&pool, 10);
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval, std::string(256, 'a'));
    }
    {
        auto expr = ColumnExpression::make(&pool, -10);
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval, 1);
    }
}

}   // namespace nebula

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);

    return RUN_ALL_TESTS();
}
