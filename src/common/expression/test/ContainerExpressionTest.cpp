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
        .add(ConstantExpression::make(&pool, 12345))
        .add(ConstantExpression::make(&pool, "Hello"))
        .add(ConstantExpression::make(&pool, true));
    auto expr = ListExpression::make(&pool, elist);
    ASSERT_EQ("[12345,\"Hello\",true]", expr->toString());
}

TEST_F(ExpressionTest, SetToString) {
    auto elist = ExpressionList::make(&pool);
    (*elist)
        .add(ConstantExpression::make(&pool, 12345))
        .add(ConstantExpression::make(&pool, 12345))
        .add(ConstantExpression::make(&pool, "Hello"))
        .add(ConstantExpression::make(&pool, true));
    auto expr = SetExpression::make(&pool, elist);
    ASSERT_EQ("{12345,12345,\"Hello\",true}", expr->toString());
}

TEST_F(ExpressionTest, MapTostring) {
    auto *items = MapItemList::make(&pool);
    (*items)
        .add("key1", ConstantExpression::make(&pool, 12345))
        .add("key2", ConstantExpression::make(&pool, 12345))
        .add("key3", ConstantExpression::make(&pool, "Hello"))
        .add("key4", ConstantExpression::make(&pool, true));
    auto expr = MapExpression::make(&pool, items);
    auto expected = "{"
                    "key1:12345,"
                    "key2:12345,"
                    "key3:\"Hello\","
                    "key4:true"
                    "}";
    ASSERT_EQ(expected, expr->toString());
}

TEST_F(ExpressionTest, MapProjectionTostring) {
    auto mapVarName = "map_var";
    auto *items = MapItemList::make(&pool);
    (*items)
        .add(".key1", nullptr)
        .add(".key2", nullptr)
        .add("key", ConstantExpression::make(&pool, "val"))
        .add("path_v2", nullptr)
        .add(".*", nullptr);

    auto expr = MapProjectionExpression::make(&pool, mapVarName, items);
    auto expected = "map_var";
    ASSERT_EQ(expected, expr->toString());
}

TEST_F(ExpressionTest, ListEvaluate) {
    auto elist = ExpressionList::make(&pool);
    (*elist)
        .add(ConstantExpression::make(&pool, 12345))
        .add(ConstantExpression::make(&pool, "Hello"))
        .add(ConstantExpression::make(&pool, true));
    auto expr = ListExpression::make(&pool, elist);
    auto expected = Value(List({12345, "Hello", true}));
    auto value = Expression::eval(expr, gExpCtxt);
    ASSERT_EQ(expected, value);
}

TEST_F(ExpressionTest, SetEvaluate) {
    auto elist = ExpressionList::make(&pool);
    (*elist)
        .add(ConstantExpression::make(&pool, 12345))
        .add(ConstantExpression::make(&pool, 12345))
        .add(ConstantExpression::make(&pool, "Hello"))
        .add(ConstantExpression::make(&pool, true));
    auto expr = SetExpression::make(&pool, elist);
    auto expected = Value(Set({12345, "Hello", true}));
    auto value = Expression::eval(expr, gExpCtxt);
    ASSERT_EQ(expected, value);
}

TEST_F(ExpressionTest, MapEvaluate) {
    {
        auto *items = MapItemList::make(&pool);
        (*items)
            .add("key1", ConstantExpression::make(&pool, 12345))
            .add("key2", ConstantExpression::make(&pool, 12345))
            .add("key3", ConstantExpression::make(&pool, "Hello"))
            .add("key4", ConstantExpression::make(&pool, true));
        auto expr = MapExpression::make(&pool, items);
        auto expected =
            Value(Map({{"key1", 12345}, {"key2", 12345}, {"key3", "Hello"}, {"key4", true}}));
        auto value = Expression::eval(expr, gExpCtxt);
        ASSERT_EQ(expected, value);
    }
    {
        auto *items = MapItemList::make(&pool);
        (*items)
            .add("key1", ConstantExpression::make(&pool, 12345))
            .add("key2", ConstantExpression::make(&pool, 12345))
            .add("key3", ConstantExpression::make(&pool, "Hello"))
            .add("key4", ConstantExpression::make(&pool, false))
            .add("key4", ConstantExpression::make(&pool, true));
        auto expr = MapExpression::make(&pool, items);
        auto expected =
            Value(Map({{"key1", 12345}, {"key2", 12345}, {"key3", "Hello"}, {"key4", false}}));
        auto value = Expression::eval(expr, gExpCtxt);
        ASSERT_EQ(expected, value);
    }
}

TEST_F(ExpressionTest, MapProjectionEvaluate) {
    /* Some values this test used:
        "path_v2": Value(Vertex("3", {}))
        "map_var": Value(Map({{"key1", 12345}, {"key2", 12345}, {"key3", "Hello"}, {"key4", true}}))
    */
    // map_var {.key1, .key2, key:"val", path_v2}
    {
        // mock the MapProjectionExpression that has been rewritten
        auto mapVarName = "map_var";
        auto *items = MapItemList::make(&pool);
        (*items)
            .add("key1",
                 AttributeExpression::make(&pool,
                                           VariablePropertyExpression::make(&pool, "", "map_var"),
                                           ConstantExpression::make(&pool, "key1")))
            .add("key2",
                 AttributeExpression::make(&pool,
                                           VariablePropertyExpression::make(&pool, "", "map_var"),
                                           ConstantExpression::make(&pool, "key2")))
            .add("key", ConstantExpression::make(&pool, "val"))
            .add("path_v2", VariablePropertyExpression::make(&pool, "", "path_v2"));

        auto expr = MapProjectionExpression::make(&pool, mapVarName, items);

        auto expected = Value(Map({
             {"key1", 12345},
             {"key2", 12345},
             {"key", "val"},
             {"path_v2", Value(Vertex("3", {}))}
            }));
        auto value = Expression::eval(expr, gExpCtxt);
        ASSERT_EQ(expected, value);
    }
    // map_var {.*}
    {
        // mock the MapProjectionExpression that has been rewritten
        auto mapVarName = "map_var";
        auto *items = MapItemList::make(&pool);
        (*items)
            .add("key1",
                 AttributeExpression::make(&pool,
                                           VariablePropertyExpression::make(&pool, "", "map_var"),
                                           ConstantExpression::make(&pool, "key1")))
            .add("key2",
                 AttributeExpression::make(&pool,
                                           VariablePropertyExpression::make(&pool, "", "map_var"),
                                           ConstantExpression::make(&pool, "key2")))
            .add("key3",
                 AttributeExpression::make(&pool,
                                           VariablePropertyExpression::make(&pool, "", "map_var"),
                                           ConstantExpression::make(&pool, "key3")))
            .add("key4",
                 AttributeExpression::make(&pool,
                                           VariablePropertyExpression::make(&pool, "", "map_var"),
                                           ConstantExpression::make(&pool, "key4")));

        auto expr = MapProjectionExpression::make(&pool, mapVarName, items);

        auto expected = Value(Map({
             {"key1", 12345},
             {"key2", 12345},
             {"key3", "Hello"},
             {"key4", true}
            }));
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
