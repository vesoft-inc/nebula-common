/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */
#include "common/expression/test/TestBase.h"

namespace nebula {

class RelationalExpressionTest : public ExpressionTest {};

// TODO: Add tests for all kinds of relExpr
TEST_F(RelationalExpressionTest, RelationEQ) {
    {
        // e1.list == NULL
        auto expr = RelationalExpression::makeRelEQ(
            &pool,
            new EdgePropertyExpression(new std::string("e1"), new std::string("list")),
            ConstantExpression::make(&pool, Value(NullType::NaN)));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        // e1.list_of_list == NULL
        auto expr = RelationalExpression::makeRelEQ(
            &pool,
            new EdgePropertyExpression(new std::string("e1"), new std::string("list_of_list")),
            ConstantExpression::make(&pool, Value(NullType::NaN)));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        // e1.list == e1.list
        auto expr = RelationalExpression::makeRelEQ(
            &pool,
            new EdgePropertyExpression(new std::string("e1"), new std::string("list")),
            new EdgePropertyExpression(new std::string("e1"), new std::string("list")));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // e1.list_of_list == e1.list_of_list
        auto expr = RelationalExpression::makeRelEQ(
            &pool,
            new EdgePropertyExpression(new std::string("e1"), new std::string("list_of_list")),
            new EdgePropertyExpression(new std::string("e1"), new std::string("list_of_list")));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // 1 == NULL
        auto expr =
            RelationalExpression::makeRelEQ(&pool,
                                            ConstantExpression::make(&pool, Value(1)),
                                            ConstantExpression::make(&pool, Value(NullType::NaN)));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        // NULL == NULL
        auto expr =
            RelationalExpression::makeRelEQ(&pool,
                                            ConstantExpression::make(&pool, Value(NullType::NaN)),
                                            ConstantExpression::make(&pool, Value(NullType::NaN)));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
}

TEST_F(RelationalExpressionTest, RelationNE) {
    {
        // 1 != NULL
        auto expr =
            RelationalExpression::makeRelNE(&pool,
                                            ConstantExpression::make(&pool, Value(1)),
                                            ConstantExpression::make(&pool, Value(NullType::NaN)));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        // NULL != NULL
        auto expr =
            RelationalExpression::makeRelNE(&pool,
                                            ConstantExpression::make(&pool, Value(NullType::NaN)),
                                            ConstantExpression::make(&pool, Value(NullType::NaN)));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
}

TEST_F(RelationalExpressionTest, RelationLT) {
    {
        // 1 < NULL
        auto expr =
            RelationalExpression::makeRelLT(&pool,
                                            ConstantExpression::make(&pool, Value(1)),
                                            ConstantExpression::make(&pool, Value(NullType::NaN)));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullValue);
    }
    {
        // NULL < NULL
        auto expr =
            RelationalExpression::makeRelLT(&pool,
                                            ConstantExpression::make(&pool, Value(NullType::NaN)),
                                            ConstantExpression::make(&pool, Value(NullType::NaN)));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullValue);
    }
}

TEST_F(RelationalExpressionTest, RelationIn) {
    {
        auto expr = RelationalExpression::makeRelIn(&pool,
                                                    ConstantExpression::make(&pool, 1),
                                                    ConstantExpression::make(&pool, List({1, 2})));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr = RelationalExpression::makeRelIn(&pool,
                                                    ConstantExpression::make(&pool, 3),
                                                    ConstantExpression::make(&pool, List({1, 2})));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        auto expr =
            RelationalExpression::makeRelIn(&pool,
                                            ConstantExpression::make(&pool, Value::kNullValue),
                                            ConstantExpression::make(&pool, List({1, 2})));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        auto list = List({1, 2});
        list.emplace_back(Value::kNullValue);
        auto expr = RelationalExpression::makeRelIn(
            &pool, ConstantExpression::make(&pool, 1), ConstantExpression::make(&pool, list));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto list = List({3, 2});
        list.emplace_back(Value::kNullValue);
        auto expr = RelationalExpression::makeRelIn(
            &pool, ConstantExpression::make(&pool, 1), ConstantExpression::make(&pool, list));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        auto list = List({3, 2});
        list.emplace_back(Value::kNullValue);
        auto expr = RelationalExpression::makeRelIn(
            &pool, ConstantExpression::make(&pool, 1), ConstantExpression::make(&pool, list));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        auto list = List({3, 2});
        list.emplace_back(Value::kNullValue);
        auto expr = RelationalExpression::makeRelIn(
            &pool, ConstantExpression::make(&pool, list), ConstantExpression::make(&pool, list));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        auto expr =
            RelationalExpression::makeRelIn(&pool,
                                            ConstantExpression::make(&pool, Value::kNullValue),
                                            ConstantExpression::make(&pool, Value::kNullValue));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
    {
        auto expr =
            RelationalExpression::makeRelIn(&pool,
                                            ConstantExpression::make(&pool, 2.3),
                                            ConstantExpression::make(&pool, Value::kNullValue));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
    {
        auto list1 = List({3, 2});
        auto list2 = list1;
        list1.emplace_back(Value::kNullValue);
        auto expr = RelationalExpression::makeRelIn(
            &pool, ConstantExpression::make(&pool, list1), ConstantExpression::make(&pool, list2));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        auto list1 = List({3, 2});
        auto list2 = List({1});
        list2.emplace_back(list1);
        list2.emplace_back(Value::kNullValue);
        auto expr = RelationalExpression::makeRelIn(
            &pool, ConstantExpression::make(&pool, list1), ConstantExpression::make(&pool, list2));
        auto eval = Expression::eval(expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
}

TEST_F(RelationalExpressionTest, RelationNotIn) {
    {
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  ConstantExpression::make(&pool, 1),
                                  ConstantExpression::make(&pool, List({1, 2})));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  ConstantExpression::make(&pool, 3),
                                  ConstantExpression::make(&pool, List({1, 2})));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  ConstantExpression::make(&pool, Value::kNullValue),
                                  ConstantExpression::make(&pool, List({1, 2})));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        auto list = List({1, 2});
        list.emplace_back(Value::kNullValue);
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  ConstantExpression::make(&pool, 1),
                                  ConstantExpression::make(&pool, list));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        auto list = List({3, 2});
        list.emplace_back(Value::kNullValue);
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  ConstantExpression::make(&pool, 1),
                                  ConstantExpression::make(&pool, list));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        auto list = List({3, 2});
        list.emplace_back(Value::kNullValue);
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  ConstantExpression::make(&pool, 1),
                                  ConstantExpression::make(&pool, list));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        auto list = List({3, 2});
        list.emplace_back(Value::kNullValue);
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  ConstantExpression::make(&pool, list),
                                  ConstantExpression::make(&pool, list));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  ConstantExpression::make(&pool, Value::kNullValue),
                                  ConstantExpression::make(&pool, Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
    {
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  ConstantExpression::make(&pool, 2.3),
                                  ConstantExpression::make(&pool, Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
    {
        auto list1 = List({3, 2});
        auto list2 = list1;
        list1.emplace_back(Value::kNullValue);
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  ConstantExpression::make(&pool, list1),
                                  ConstantExpression::make(&pool, list2));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto list1 = List({3, 2});
        auto list2 = List({1});
        list2.emplace_back(list1);
        list2.emplace_back(Value::kNullValue);
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  ConstantExpression::make(&pool, list1),
                                  ConstantExpression::make(&pool, list2));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
}
}   // namespace nebula

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);

    return RUN_ALL_TESTS();
}
