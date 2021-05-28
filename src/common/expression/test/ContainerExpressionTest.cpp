/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */
#include "common/expression/test/TestBase.h"

namespace nebula {

class ConstantExpressionTest : public ExpressionTest {};

TEST_F(ExpressionTest, ListToString) {
    auto elist = ExpressionList::make(&pool);
    (*elist)
        .add(new ConstantExpression(12345))
        .add(new ConstantExpression("Hello"))
        .add(new ConstantExpression(true));
    auto expr = ListExpression::make(&pool, elist);
    ASSERT_EQ("[12345,\"Hello\",true]", expr->toString());
}

TEST_F(ExpressionTest, SetToString) {
    auto elist = ExpressionList::make(&pool);
    (*elist)
        .add(new ConstantExpression(12345))
        .add(new ConstantExpression(12345))
        .add(new ConstantExpression("Hello"))
        .add(new ConstantExpression(true));
    auto expr = SetExpression::make(&pool, elist);
    ASSERT_EQ("{12345,12345,\"Hello\",true}", expr->toString());
}

TEST_F(ExpressionTest, MapTostring) {
    auto *items = MapItemList::make(&pool);
    (*items)
        .add("key1", new ConstantExpression(12345))
        .add("key2", new ConstantExpression(12345))
        .add("key3", new ConstantExpression("Hello"))
        .add("key4", new ConstantExpression(true));
    auto expr = MapExpression::make(&pool, items);
    auto expected = "{"
                    "key1:12345,"
                    "key2:12345,"
                    "key3:\"Hello\","
                    "key4:true"
                    "}";
    ASSERT_EQ(expected, expr->toString());
}

TEST_F(ExpressionTest, ListEvaluate) {
    auto elist = ExpressionList::make(&pool);
    (*elist)
        .add(new ConstantExpression(12345))
        .add(new ConstantExpression("Hello"))
        .add(new ConstantExpression(true));
    auto expr = ListExpression::make(&pool, elist);
    auto expected = Value(List({12345, "Hello", true}));
    auto value = Expression::eval(expr, gExpCtxt);
    ASSERT_EQ(expected, value);
}

TEST_F(ExpressionTest, SetEvaluate) {
    auto elist = ExpressionList::make(&pool);
    (*elist)
        .add(new ConstantExpression(12345))
        .add(new ConstantExpression(12345))
        .add(new ConstantExpression("Hello"))
        .add(new ConstantExpression(true));
    auto expr = SetExpression::make(&pool, elist);
    auto expected = Value(Set({12345, "Hello", true}));
    auto value = Expression::eval(expr, gExpCtxt);
    ASSERT_EQ(expected, value);
}

TEST_F(ExpressionTest, MapEvaluate) {
    {
        auto *items = MapItemList::make(&pool);
        (*items)
            .add("key1", new ConstantExpression(12345))
            .add("key2", new ConstantExpression(12345))
            .add("key3", new ConstantExpression("Hello"))
            .add("key4", new ConstantExpression(true));
        auto expr = MapExpression::make(&pool, items);
        auto expected =
            Value(Map({{"key1", 12345}, {"key2", 12345}, {"key3", "Hello"}, {"key4", true}}));
        auto value = Expression::eval(expr, gExpCtxt);
        ASSERT_EQ(expected, value);
    }
    {
        auto *items = MapItemList::make(&pool);
        (*items)
            .add("key1", new ConstantExpression(12345))
            .add("key2", new ConstantExpression(12345))
            .add("key3", new ConstantExpression("Hello"))
            .add("key4", new ConstantExpression(false))
            .add("key4", new ConstantExpression(true));
        auto expr = MapExpression::make(&pool, items);
        auto expected =
            Value(Map({{"key1", 12345}, {"key2", 12345}, {"key3", "Hello"}, {"key4", false}}));
        auto value = Expression::eval(expr, gExpCtxt);
        ASSERT_EQ(expected, value);
    }
}
}   // namespace nebula

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);

    return RUN_ALL_TESTS();
}
