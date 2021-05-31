/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/test/TestBase.h"

namespace nebula {

TEST_F(ExpressionTest, GetProp) {
    {
        // e1.int
        EdgePropertyExpression ep("e1", "int");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // t1.int
        TagPropertyExpression ep("t1", "int");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // $-.int
        InputPropertyExpression ep("int");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // $^.source.int
        SourcePropertyExpression ep("source", "int");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // $$.dest.int
        DestPropertyExpression ep("dest", "int");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // $var.float
        VariablePropertyExpression ep("var", "float");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::FLOAT);
        EXPECT_EQ(eval, 1.1);
    }
}

TEST_F(ExpressionTest, EdgeTest) {
    {
        // EdgeName._src
        EdgeSrcIdExpression ep("edge1");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // EdgeName._type
        EdgeTypeExpression ep("edge1");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // EdgeName._rank
        EdgeRankExpression ep("edge1");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // EdgeName._dst
        EdgeDstIdExpression ep("edge1");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
}

TEST_F(ExpressionTest, LogicalCalculation) {
    {
        TEST_EXPR(true, true);
        TEST_EXPR(false, false);
    }
    {
        TEST_EXPR(true XOR true, false);
        TEST_EXPR(true XOR false, true);
        TEST_EXPR(false XOR false, false);
        TEST_EXPR(false XOR true, true);
    }
    {
        TEST_EXPR(true AND true AND true, true);
        TEST_EXPR(true AND true OR false, true);
        TEST_EXPR(true AND true AND false, false);
        TEST_EXPR(true OR false AND true OR false, true);
        TEST_EXPR(true XOR true XOR false, false);
    }
    {
        // AND
        TEST_EXPR(true AND true, true);
        TEST_EXPR(true AND false, false);
        TEST_EXPR(false AND true, false);
        TEST_EXPR(false AND false, false);

        // AND AND  ===  (AND) AND
        TEST_EXPR(true AND true AND true, true);
        TEST_EXPR(true AND true AND false, false);
        TEST_EXPR(true AND false AND true, false);
        TEST_EXPR(true AND false AND false, false);
        TEST_EXPR(false AND true AND true, false);
        TEST_EXPR(false AND true AND false, false);
        TEST_EXPR(false AND false AND true, false);
        TEST_EXPR(false AND false AND false, false);

        // OR
        TEST_EXPR(true OR true, true);
        TEST_EXPR(true OR false, true);
        TEST_EXPR(false OR true, true);
        TEST_EXPR(false OR false, false);

        // OR OR  ===  (OR) OR
        TEST_EXPR(true OR true OR true, true);
        TEST_EXPR(true OR true OR false, true);
        TEST_EXPR(true OR false OR true, true);
        TEST_EXPR(true OR false OR false, true);
        TEST_EXPR(false OR true OR true, true);
        TEST_EXPR(false OR true OR false, true);
        TEST_EXPR(false OR false OR true, true);
        TEST_EXPR(false OR false OR false, false);

        // AND OR  ===  (AND) OR
        TEST_EXPR(true AND true OR true, true);
        TEST_EXPR(true AND true OR false, true);
        TEST_EXPR(true AND false OR true, true);
        TEST_EXPR(true AND false OR false, false);
        TEST_EXPR(false AND true OR true, true);
        TEST_EXPR(false AND true OR false, false);
        TEST_EXPR(false AND false OR true, true);
        TEST_EXPR(false AND false OR false, false);

        // OR AND  === OR (AND)
        TEST_EXPR(true OR true AND true, true);
        TEST_EXPR(true OR true AND false, true);
        TEST_EXPR(true OR false AND true, true);
        TEST_EXPR(true OR false AND false, true);
        TEST_EXPR(false OR true AND true, true);
        TEST_EXPR(false OR true AND false, false);
        TEST_EXPR(false OR false AND true, false);
        TEST_EXPR(false OR false AND false, false);
    }
    {
        TEST_EXPR(2 > 1 AND 3 > 2, true);
        TEST_EXPR(2 <= 1 AND 3 > 2, false);
        TEST_EXPR(2 > 1 AND 3 < 2, false);
        TEST_EXPR(2 < 1 AND 3 < 2, false);
    }
    {
        // test bad null
        TEST_EXPR(2 / 0, Value::kNullDivByZero);
        TEST_EXPR(2 / 0 AND true, Value::kNullDivByZero);
        TEST_EXPR(2 / 0 AND false, Value::Value::kNullDivByZero);
        TEST_EXPR(true AND 2 / 0, Value::kNullDivByZero);
        TEST_EXPR(false AND 2 / 0, false);
        TEST_EXPR(2 / 0 AND 2 / 0, Value::kNullDivByZero);
        TEST_EXPR(empty AND null AND 2 / 0 AND empty, Value::kNullDivByZero);

        TEST_EXPR(2 / 0 OR true, Value::kNullDivByZero);
        TEST_EXPR(2 / 0 OR false, Value::kNullDivByZero);
        TEST_EXPR(true OR 2 / 0, true);
        TEST_EXPR(false OR 2 / 0, Value::kNullDivByZero);
        TEST_EXPR(2 / 0 OR 2 / 0, Value::kNullDivByZero);
        TEST_EXPR(empty OR null OR 2 / 0 OR empty, Value::kNullDivByZero);

        TEST_EXPR(2 / 0 XOR true, Value::kNullDivByZero);
        TEST_EXPR(2 / 0 XOR false, Value::kNullDivByZero);
        TEST_EXPR(true XOR 2 / 0, Value::kNullDivByZero);
        TEST_EXPR(false XOR 2 / 0, Value::kNullDivByZero);
        TEST_EXPR(2 / 0 XOR 2 / 0, Value::kNullDivByZero);
        TEST_EXPR(empty XOR 2 / 0 XOR null XOR empty, Value::kNullDivByZero);

        // test normal null
        TEST_EXPR(null AND true, Value::kNullValue);
        TEST_EXPR(null AND false, false);
        TEST_EXPR(true AND null, Value::kNullValue);
        TEST_EXPR(false AND null, false);
        TEST_EXPR(null AND null, Value::kNullValue);
        TEST_EXPR(empty AND null AND empty, Value::kNullValue);

        TEST_EXPR(null OR true, true);
        TEST_EXPR(null OR false, Value::kNullValue);
        TEST_EXPR(true OR null, true);
        TEST_EXPR(false OR null, Value::kNullValue);
        TEST_EXPR(null OR null, Value::kNullValue);
        TEST_EXPR(empty OR null OR empty, Value::kNullValue);

        TEST_EXPR(null XOR true, Value::kNullValue);
        TEST_EXPR(null XOR false, Value::kNullValue);
        TEST_EXPR(true XOR null, Value::kNullValue);
        TEST_EXPR(false XOR null, Value::kNullValue);
        TEST_EXPR(null XOR null, Value::kNullValue);
        TEST_EXPR(empty XOR null XOR empty, Value::kNullValue);

        // test empty
        TEST_EXPR(empty, Value::kEmpty);
        TEST_EXPR(empty AND true, Value::kEmpty);
        TEST_EXPR(empty AND false, false);
        TEST_EXPR(true AND empty, Value::kEmpty);
        TEST_EXPR(false AND empty, false);
        TEST_EXPR(empty AND empty, Value::kEmpty);
        TEST_EXPR(empty AND null, Value::kNullValue);
        TEST_EXPR(null AND empty, Value::kNullValue);
        TEST_EXPR(empty AND true AND empty, Value::kEmpty);

        TEST_EXPR(empty OR true, true);
        TEST_EXPR(empty OR false, Value::kEmpty);
        TEST_EXPR(true OR empty, true);
        TEST_EXPR(false OR empty, Value::kEmpty);
        TEST_EXPR(empty OR empty, Value::kEmpty);
        TEST_EXPR(empty OR null, Value::kNullValue);
        TEST_EXPR(null OR empty, Value::kNullValue);
        TEST_EXPR(empty OR false OR empty, Value::kEmpty);

        TEST_EXPR(empty XOR true, Value::kEmpty);
        TEST_EXPR(empty XOR false, Value::kEmpty);
        TEST_EXPR(true XOR empty, Value::kEmpty);
        TEST_EXPR(false XOR empty, Value::kEmpty);
        TEST_EXPR(empty XOR empty, Value::kEmpty);
        TEST_EXPR(empty XOR null, Value::kNullValue);
        TEST_EXPR(null XOR empty, Value::kNullValue);
        TEST_EXPR(true XOR empty XOR false, Value::kEmpty);

        TEST_EXPR(empty OR false AND true AND null XOR empty, Value::kEmpty);
        TEST_EXPR(empty OR false AND true XOR empty OR true, true);
        TEST_EXPR((empty OR false) AND true XOR empty XOR null AND 2 / 0, Value::kNullValue);
        // empty OR false AND 2/0
        TEST_EXPR(empty OR false AND true XOR empty XOR null AND 2 / 0, Value::kEmpty);
        TEST_EXPR(empty AND true XOR empty XOR null AND 2 / 0, Value::kNullValue);
        TEST_EXPR(empty OR false AND true XOR empty OR null AND 2 / 0, Value::kNullDivByZero);
        TEST_EXPR(empty OR false AND empty XOR empty OR null, Value::kNullValue);
    }
}

TEST_F(ExpressionTest, LiteralConstantsRelational) {
    {
        TEST_EXPR(true == 1.0, false);
        TEST_EXPR(true == 2.0, false);
        TEST_EXPR(true != 1.0, true);
        TEST_EXPR(true != 2.0, true);
        TEST_EXPR(true > 1.0, Value::kNullBadType);
        TEST_EXPR(true >= 1.0, Value::kNullBadType);
        TEST_EXPR(true < 1.0, Value::kNullBadType);
        TEST_EXPR(true <= 1.0, Value::kNullBadType);
        TEST_EXPR(false == 0.0, false);
        TEST_EXPR(false == 1.0, false);
        TEST_EXPR(false != 0.0, true);
        TEST_EXPR(false != 1.0, true);
        TEST_EXPR(false > 0.0, Value::kNullBadType);
        TEST_EXPR(false >= 0.0, Value::kNullBadType);
        TEST_EXPR(false < 0.0, Value::kNullBadType);
        TEST_EXPR(false <= 0.0, Value::kNullBadType);

        TEST_EXPR(true == 1, false);
        TEST_EXPR(true == 2, false);
        TEST_EXPR(true != 1, true);
        TEST_EXPR(true != 2, true);
        TEST_EXPR(true > 1, Value::kNullBadType);
        TEST_EXPR(true >= 1, Value::kNullBadType);
        TEST_EXPR(true < 1, Value::kNullBadType);
        TEST_EXPR(true <= 1, Value::kNullBadType);
        TEST_EXPR(false == 0, false);
        TEST_EXPR(false == 1, false);
        TEST_EXPR(false != 0, true);
        TEST_EXPR(false != 1, true);
        TEST_EXPR(false > 0, Value::kNullBadType);
        TEST_EXPR(false >= 0, Value::kNullBadType);
        TEST_EXPR(false < 0, Value::kNullBadType);
        TEST_EXPR(false <= 0, Value::kNullBadType);
    }
    {
        TEST_EXPR(-1 == -2, false);
        TEST_EXPR(-2 == -1, false);
        TEST_EXPR(-1 != -2, true);
        TEST_EXPR(-2 != -1, true);
        TEST_EXPR(-1 > -2, true);
        TEST_EXPR(-2 > -1, false);
        TEST_EXPR(-1 >= -2, true);
        TEST_EXPR(-2 >= -1, false);
        TEST_EXPR(-1 < -2, false);
        TEST_EXPR(-2 < -1, true);
        TEST_EXPR(-1 <= -2, false);
        TEST_EXPR(-2 <= -1, true);

        TEST_EXPR(0.5 == 1, false);
        TEST_EXPR(1.0 == 1, true);
        TEST_EXPR(0.5 != 1, true);
        TEST_EXPR(1.0 != 1, false);
        TEST_EXPR(0.5 > 1, false);
        TEST_EXPR(0.5 >= 1, false);
        TEST_EXPR(0.5 < 1, true);
        TEST_EXPR(0.5 <= 1, true);

        TEST_EXPR(-1 == -1, true);
        TEST_EXPR(-1 != -1, false);
        TEST_EXPR(-1 > -1, false);
        TEST_EXPR(-1 >= -1, true);
        TEST_EXPR(-1 < -1, false);
        TEST_EXPR(-1 <= -1, true);

        TEST_EXPR(1 == 2, false);
        TEST_EXPR(2 == 1, false);
        TEST_EXPR(1 != 2, true);
        TEST_EXPR(2 != 1, true);
        TEST_EXPR(1 > 2, false);
        TEST_EXPR(2 > 1, true);
        TEST_EXPR(1 >= 2, false);
        TEST_EXPR(2 >= 1, true);
        TEST_EXPR(1 < 2, true);
        TEST_EXPR(2 < 1, false);
        TEST_EXPR(1 <= 2, true);
        TEST_EXPR(2 <= 1, false);

        TEST_EXPR(1 == 1, true);
        TEST_EXPR(1 != 1, false);
        TEST_EXPR(1 > 1, false);
        TEST_EXPR(1 >= 1, true);
        TEST_EXPR(1 < 1, false);
        TEST_EXPR(1 <= 1, true);
    }
    {
        TEST_EXPR(empty == empty, true);
        TEST_EXPR(empty == null, Value::kNullValue);
        TEST_EXPR(empty != null, Value::kNullValue);
        TEST_EXPR(empty != 1, true);
        TEST_EXPR(empty != true, true);
        TEST_EXPR(empty > "1", Value::kEmpty);
        TEST_EXPR(empty < 1, Value::kEmpty);
        TEST_EXPR(empty >= 1.11, Value::kEmpty);

        TEST_EXPR(null != 1, Value::kNullValue);
        TEST_EXPR(null != true, Value::kNullValue);
        TEST_EXPR(null > "1", Value::kNullValue);
        TEST_EXPR(null < 1, Value::kNullValue);
        TEST_EXPR(null >= 1.11, Value::kNullValue);
    }
    {
        TEST_EXPR(8 % 2 + 1 == 1, true);
        TEST_EXPR(8 % 2 + 1 != 1, false);
        TEST_EXPR(8 % 3 + 1 == 3, true);
        TEST_EXPR(8 % 3 + 1 != 3, false);
        TEST_EXPR(8 % 3 > 1, true);
        TEST_EXPR(8 % 3 >= 2, true);
        TEST_EXPR(8 % 3 <= 2, true);
        TEST_EXPR(8 % 3 < 2, false);

        TEST_EXPR(3.14 * 3 * 3 / 2 > 3.14 * 1.5 * 1.5 / 2, true);
        TEST_EXPR(3.14 * 3 * 3 / 2 < 3.14 * 1.5 * 1.5 / 2, false);
    }
}

// TEST_F(ExpressionTest, UnaryINCR) {
//     {
//         // ++var_int
//         UnaryExpression expr(
//                 Expression::Kind::kUnaryIncr,
//                 new VariableExpression("var_int"));
//         auto eval = Expression::eval(&expr, gExpCtxt);
//         EXPECT_EQ(eval.type(), Value::Type::INT);
//         EXPECT_EQ(eval, 2);
//     }
//     {
//         // ++versioned_var{0}
//         UnaryExpression expr(
//                 Expression::Kind::kUnaryIncr,
//                 new VersionedVariableExpression(
//                     "versioned_var",
//                     new ConstantExpression(0)));
//         auto eval = Expression::eval(&expr, gExpCtxt);
//         EXPECT_EQ(eval.type(), Value::Type::INT);
//         EXPECT_EQ(eval, 2);
//     }
// }

TEST_F(ExpressionTest, IsNull) {
    {
        UnaryExpression expr(
                Expression::Kind::kIsNull,
                new ConstantExpression(Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kIsNull,
                new ConstantExpression(1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kIsNull,
                new ConstantExpression(1.1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kIsNull,
                new ConstantExpression(true));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kIsNull,
                new ConstantExpression(Value::kEmpty));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
}


TEST_F(ExpressionTest, IsNotNull) {
    {
        UnaryExpression expr(
                Expression::Kind::kIsNotNull,
                new ConstantExpression(Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kIsNotNull,
                new ConstantExpression(1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kIsNotNull,
                new ConstantExpression(1.1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kIsNotNull,
                new ConstantExpression(true));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kIsNotNull,
                new ConstantExpression(Value::kEmpty));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
}


TEST_F(ExpressionTest, IsEmpty) {
    {
        UnaryExpression expr(
                Expression::Kind::kIsEmpty,
                new ConstantExpression(Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kIsEmpty,
                new ConstantExpression(1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kIsEmpty,
                new ConstantExpression(1.1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kIsEmpty,
                new ConstantExpression(true));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kIsEmpty,
                new ConstantExpression(Value::kEmpty));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
}

TEST_F(ExpressionTest, IsNotEmpty) {
    {
        UnaryExpression expr(
                Expression::Kind::kIsNotEmpty,
                new ConstantExpression(Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kIsNotEmpty,
                new ConstantExpression(1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kIsNotEmpty,
                new ConstantExpression(1.1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kIsNotEmpty,
                new ConstantExpression(true));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kIsNotEmpty,
                new ConstantExpression(Value::kEmpty));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
}

TEST_F(ExpressionTest, UnaryDECR) {
    {
        // --var_int
        UnaryExpression expr(
                Expression::Kind::kUnaryDecr,
                new VariableExpression("var_int"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 0);
    }
}

TEST_F(ExpressionTest, VersionedVar) {
    {
        // versioned_var{0}
        VersionedVariableExpression expr(
                "versioned_var",
                new ConstantExpression(0));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // versioned_var{0}
        VersionedVariableExpression expr(
                "versioned_var",
                new ConstantExpression(-1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 2);
    }
    {
        // versioned_var{0}
        VersionedVariableExpression expr(
                "versioned_var",
                new ConstantExpression(1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 8);
    }
    {
        // versioned_var{-cnt}
        VersionedVariableExpression expr(
                "versioned_var",
                new UnaryExpression(
                    Expression::Kind::kUnaryNegate,
                    new VariableExpression("cnt")));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 2);
    }
}

TEST_F(ExpressionTest, toStringTest) {
    {
        ConstantExpression ep(1);
        EXPECT_EQ(ep.toString(), "1");
    }
    {
        ConstantExpression ep(1.123);
        EXPECT_EQ(ep.toString(), "1.123");
    }
    // FIXME: double/float to string conversion
    // {
    //     ConstantExpression ep(1.0);
    //     EXPECT_EQ(ep.toString(), "1.0");
    // }
    {
        ConstantExpression ep(true);
        EXPECT_EQ(ep.toString(), "true");
    }
    {
        ConstantExpression ep(List(std::vector<Value>{1, 2, 3, 4, 9, 0, -23}));
        EXPECT_EQ(ep.toString(), "[1,2,3,4,9,0,-23]");
    }
    {
        ConstantExpression ep(Map({{"hello", "world"}, {"name", "zhang"}}));
        EXPECT_EQ(ep.toString(), "{name:\"zhang\",hello:\"world\"}");
    }
    {
        ConstantExpression ep(Set({1, 2.3, "hello", true}));
        EXPECT_EQ(ep.toString(), "{\"hello\",2.3,true,1}");
    }
    {
        ConstantExpression ep(Date(1234));
        EXPECT_EQ(ep.toString(), "-32765-05-19");
    }
    {
        ConstantExpression ep(Edge("100", "102", 2, "like", 3, {{"likeness", 95}}));
        EXPECT_EQ(ep.toString(), "(\"100\")-[like(2)]->(\"102\")@3 likeness:95");
    }
    {
        ConstantExpression ep(Vertex("100", {Tag("player", {{"name", "jame"}})}));
        EXPECT_EQ(ep.toString(), "(\"100\") Tag: player, name:\"jame\"");
    }
    {
        TypeCastingExpression ep(Value::Type::FLOAT, new ConstantExpression(2));
        EXPECT_EQ(ep.toString(), "(FLOAT)2");
    }
    {
        UnaryExpression plus(Expression::Kind::kUnaryPlus, new ConstantExpression(2));
        EXPECT_EQ(plus.toString(), "+(2)");

        UnaryExpression nega(Expression::Kind::kUnaryNegate, new ConstantExpression(2));
        EXPECT_EQ(nega.toString(), "-(2)");

        UnaryExpression incr(Expression::Kind::kUnaryIncr, new ConstantExpression(2));
        EXPECT_EQ(incr.toString(), "++(2)");

        UnaryExpression decr(Expression::Kind::kUnaryDecr, new ConstantExpression(2));
        EXPECT_EQ(decr.toString(), "--(2)");

        UnaryExpression no(Expression::Kind::kUnaryNot, new ConstantExpression(2));
        EXPECT_EQ(no.toString(), "!(2)");

        UnaryExpression isNull(Expression::Kind::kIsNull, new ConstantExpression(2));
        EXPECT_EQ(isNull.toString(), "2 IS NULL");

        UnaryExpression isNotNull(Expression::Kind::kIsNotNull,
                                  new ConstantExpression(Value::kNullValue));
        EXPECT_EQ(isNotNull.toString(), "NULL IS NOT NULL");

        UnaryExpression isEmpty(Expression::Kind::kIsEmpty, new ConstantExpression(2));
        EXPECT_EQ(isEmpty.toString(), "2 IS EMPTY");

        UnaryExpression isNotEmpty(Expression::Kind::kIsNotEmpty,
                                  new ConstantExpression(Value::kEmpty));
        EXPECT_EQ(isNotEmpty.toString(), "__EMPTY__ IS NOT EMPTY");
    }
    {
        VariableExpression var("name");
        EXPECT_EQ(var.toString(), "$name");

        VersionedVariableExpression versionVar("name", new ConstantExpression(1));
        EXPECT_EQ(versionVar.toString(), "$name{1}");
    }
    {
        TEST_TOSTRING(2 + 2 - 3, "((2+2)-3)");
        TEST_TOSTRING(true OR true, "(true OR true)");
        TEST_TOSTRING(true AND false OR false, "((true AND false) OR false)");
        TEST_TOSTRING(true == 2, "(true==2)");
        TEST_TOSTRING(2 > 1 AND 3 > 2, "((2>1) AND (3>2))");
        TEST_TOSTRING((3 + 5) * 3 / (6 - 2), "(((3+5)*3)/(6-2))");
        TEST_TOSTRING(76 - 100 / 20 * 4, "(76-((100/20)*4))");
        TEST_TOSTRING(8 % 2 + 1 == 1, "(((8%2)+1)==1)");
        TEST_TOSTRING(1 == 2, "(1==2)");
    }
}

TEST_F(ExpressionTest, PropertyToStringTest) {
    {
        DestPropertyExpression ep("like",
                                  "likeness");
        EXPECT_EQ(ep.toString(), "$$.like.likeness");
    }
    {
        EdgePropertyExpression ep("like", "likeness");
        EXPECT_EQ(ep.toString(), "like.likeness");
    }
    {
        InputPropertyExpression ep("name");
        EXPECT_EQ(ep.toString(), "$-.name");
    }
    {
        VariablePropertyExpression ep("player", "name");
        EXPECT_EQ(ep.toString(), "$player.name");
    }
    {
        SourcePropertyExpression ep("player", "age");
        EXPECT_EQ(ep.toString(), "$^.player.age");
    }
    {
        DestPropertyExpression ep("player", "age");
        EXPECT_EQ(ep.toString(), "$$.player.age");
    }
    {
        EdgeSrcIdExpression ep("like");
        EXPECT_EQ(ep.toString(), "like._src");
    }
    {
        EdgeTypeExpression ep("like");
        EXPECT_EQ(ep.toString(), "like._type");
    }
    {
        EdgeRankExpression ep("like");
        EXPECT_EQ(ep.toString(), "like._rank");
    }
    {
        EdgeDstIdExpression ep("like");
        EXPECT_EQ(ep.toString(), "like._dst");
    }
}




TEST_F(ExpressionTest, InList) {
    {
        auto *elist = new ExpressionList;
        (*elist).add(new ConstantExpression(12345))
                .add(new ConstantExpression("Hello"))
                .add(new ConstantExpression(true));
        auto listExpr = new ListExpression(elist);
        RelationalExpression expr(Expression::Kind::kRelIn,
                                  new ConstantExpression(12345),
                                  listExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(true, value);
    }
    {
        auto *elist = new ExpressionList;
        (*elist).add(new ConstantExpression(12345))
                .add(new ConstantExpression("Hello"))
                .add(new ConstantExpression(true));
        auto listExpr = new ListExpression(elist);
        RelationalExpression expr(Expression::Kind::kRelIn,
                                  new ConstantExpression(false),
                                  listExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value);
    }
}

TEST_F(ExpressionTest, InSet) {
    {
        auto *elist = new ExpressionList;
        (*elist).add(new ConstantExpression(12345))
                .add(new ConstantExpression("Hello"))
                .add(new ConstantExpression(true));
        auto setExpr = new SetExpression(elist);
        RelationalExpression expr(Expression::Kind::kRelIn,
                                  new ConstantExpression(12345),
                                  setExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(true, value);
    }
    {
        auto *elist = new ExpressionList;
        (*elist).add(new ConstantExpression(12345))
                .add(new ConstantExpression("Hello"))
                .add(new ConstantExpression(true));
        auto setExpr = new ListExpression(elist);
        RelationalExpression expr(Expression::Kind::kRelIn,
                                  new ConstantExpression(false),
                                  setExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value);
    }
}

TEST_F(ExpressionTest, InMap) {
    {
        auto *items = new MapItemList();
        (*items).add("key1", new ConstantExpression(12345))
                .add("key2", new ConstantExpression(12345))
                .add("key3", new ConstantExpression("Hello"))
                .add("key4", new ConstantExpression(true));
        auto mapExpr = new MapExpression(items);
        RelationalExpression expr(Expression::Kind::kRelIn,
                                  new ConstantExpression("key1"),
                                  mapExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(true, value);
    }
    {
        auto *items = new MapItemList();
        (*items).add("key1", new ConstantExpression(12345))
                .add("key2", new ConstantExpression(12345))
                .add("key3", new ConstantExpression("Hello"))
                .add("key4", new ConstantExpression(true));
        auto mapExpr = new MapExpression(items);
        RelationalExpression expr(Expression::Kind::kRelIn,
                                  new ConstantExpression("key5"),
                                  mapExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value);
    }
    {
        auto *items = new MapItemList();
        (*items).add("key1", new ConstantExpression(12345))
                .add("key2", new ConstantExpression(12345))
                .add("key3", new ConstantExpression("Hello"))
                .add("key4", new ConstantExpression(true));
        auto mapExpr = new MapExpression(items);
        RelationalExpression expr(Expression::Kind::kRelIn,
                                  new ConstantExpression(12345),
                                  mapExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value);
    }
}

TEST_F(ExpressionTest, NotInList) {
    {
        auto *elist = new ExpressionList;
        (*elist).add(new ConstantExpression(12345))
                .add(new ConstantExpression("Hello"))
                .add(new ConstantExpression(true));
        auto listExpr = new ListExpression(elist);
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  new ConstantExpression(12345),
                                  listExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value);
    }
    {
        auto *elist = new ExpressionList;
        (*elist).add(new ConstantExpression(12345))
                .add(new ConstantExpression("Hello"))
                .add(new ConstantExpression(true));
        auto listExpr = new ListExpression(elist);
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  new ConstantExpression(false),
                                  listExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(true, value);
    }
}

TEST_F(ExpressionTest, NotInSet) {
    {
        auto *elist = new ExpressionList;
        (*elist).add(new ConstantExpression(12345))
                .add(new ConstantExpression("Hello"))
                .add(new ConstantExpression(true));
        auto setExpr = new SetExpression(elist);
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  new ConstantExpression(12345),
                                  setExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value);
    }
    {
        auto *elist = new ExpressionList;
        (*elist).add(new ConstantExpression(12345))
                .add(new ConstantExpression("Hello"))
                .add(new ConstantExpression(true));
        auto setExpr = new ListExpression(elist);
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  new ConstantExpression(false),
                                  setExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(true, value);
    }
}

TEST_F(ExpressionTest, NotInMap) {
    {
        auto *items = new MapItemList();
        (*items).add("key1", new ConstantExpression(12345))
                .add("key2", new ConstantExpression(12345))
                .add("key3", new ConstantExpression("Hello"))
                .add("key4", new ConstantExpression(true));
        auto mapExpr = new MapExpression(items);
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  new ConstantExpression("key1"),
                                  mapExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value);
    }
    {
        auto *items = new MapItemList();
        (*items).add("key1", new ConstantExpression(12345))
                .add("key2", new ConstantExpression(12345))
                .add("key3", new ConstantExpression("Hello"))
                .add("key4", new ConstantExpression(true));
        auto mapExpr = new MapExpression(items);
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  new ConstantExpression("key5"),
                                  mapExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(true, value);
    }
    {
        auto *items = new MapItemList();
        (*items).add("key1", new ConstantExpression(12345))
                .add("key2", new ConstantExpression(12345))
                .add("key3", new ConstantExpression("Hello"))
                .add("key4", new ConstantExpression(true));
        auto mapExpr = new MapExpression(items);
        RelationalExpression expr(Expression::Kind::kRelNotIn,
                                  new ConstantExpression(12345),
                                  mapExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(true, value);
    }
}

TEST_F(ExpressionTest, DataSetSubscript) {
    {
        // dataset[]
        // [[0,1,2,3,4],[1,2,3,4,5],[2,3,4,5,6]] [0]
        DataSet ds;
        for (int32_t i = 0; i < 3; i++) {
            std::vector<Value> val;
            val.reserve(5);
            for (int32_t j = 0; j < 5; j++) {
                val.emplace_back(i + j);
            }
            ds.rows.emplace_back(List(std::move(val)));
        }
        auto *dataset = new ConstantExpression(ds);
        auto *rowIndex = new ConstantExpression(0);
        SubscriptExpression expr(dataset, rowIndex);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isList());
        ASSERT_EQ(Value(List({0, 1, 2, 3, 4})), value.getList());
    }
    {
        // dataset[][]
        // [[0,1,2,3,4],[1,2,3,4,5],[2,3,4,5,6]] [0][1]
        DataSet ds;
        for (int32_t i = 0; i < 3; i++) {
            std::vector<Value> val;
            val.reserve(5);
            for (int32_t j = 0; j < 5; j++) {
                val.emplace_back(i + j);
            }
            ds.rows.emplace_back(List(std::move(val)));
        }
        auto *dataset = new ConstantExpression(ds);
        auto *rowIndex = new ConstantExpression(0);
        auto *colIndex = new ConstantExpression(1);
        SubscriptExpression expr(new SubscriptExpression(dataset, rowIndex), colIndex);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(1, value.getInt());
    }
    {
        // dataset[]
        // [[0,1,2,3,4],[1,2,3,4,5],[2,3,4,5,6]] [-1]
        DataSet ds;
        for (int32_t i = 0; i < 3; i++) {
            std::vector<Value> val;
            val.reserve(5);
            for (int32_t j = 0; j < 5; j++) {
                val.emplace_back(i + j);
            }
            ds.rows.emplace_back(List(std::move(val)));
        }
        auto *dataset = new ConstantExpression(ds);
        auto *rowIndex = new ConstantExpression(-1);
        SubscriptExpression expr(dataset, rowIndex);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBadNull());
    }
    {
        // dataset[][]
        // [[0,1,2,3,4],[1,2,3,4,5],[2,3,4,5,6]] [0][5]
        DataSet ds;
        for (int32_t i = 0; i < 3; i++) {
            std::vector<Value> val;
            val.reserve(5);
            for (int32_t j = 0; j < 5; j++) {
                val.emplace_back(i + j);
            }
            ds.rows.emplace_back(List(std::move(val)));
        }
        auto *dataset = new ConstantExpression(ds);
        auto *rowIndex = new ConstantExpression(0);
        auto *colIndex = new ConstantExpression(5);
        SubscriptExpression expr(new SubscriptExpression(dataset, rowIndex), colIndex);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBadNull());
    }
}

TEST_F(ExpressionTest, ListSubscript) {
    // [1,2,3,4][0]
    {
        auto *items = new ExpressionList();
        (*items).add(new ConstantExpression(1))
                .add(new ConstantExpression(2))
                .add(new ConstantExpression(3))
                .add(new ConstantExpression(4));
        auto *list = new ListExpression(items);
        auto *index = new ConstantExpression(0);
        SubscriptExpression expr(list, index);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(1, value.getInt());
    }
    // [1,2,3,4][3]
    {
        auto *items = new ExpressionList();
        (*items).add(new ConstantExpression(1))
                .add(new ConstantExpression(2))
                .add(new ConstantExpression(3))
                .add(new ConstantExpression(4));
        auto *list = new ListExpression(items);
        auto *index = new ConstantExpression(3);
        SubscriptExpression expr(list, index);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(4, value.getInt());
    }
    // [1,2,3,4][4]
    {
        auto *items = new ExpressionList();
        (*items).add(new ConstantExpression(1))
                .add(new ConstantExpression(2))
                .add(new ConstantExpression(3))
                .add(new ConstantExpression(4));
        auto *list = new ListExpression(items);
        auto *index = new ConstantExpression(4);
        SubscriptExpression expr(list, index);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBadNull());
    }
    // [1,2,3,4][-1]
    {
        auto *items = new ExpressionList();
        (*items).add(new ConstantExpression(1))
                .add(new ConstantExpression(2))
                .add(new ConstantExpression(3))
                .add(new ConstantExpression(4));
        auto *list = new ListExpression(items);
        auto *index = new ConstantExpression(-1);
        SubscriptExpression expr(list, index);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(4, value.getInt());
    }
    // [1,2,3,4][-4]
    {
        auto *items = new ExpressionList();
        (*items).add(new ConstantExpression(1))
                .add(new ConstantExpression(2))
                .add(new ConstantExpression(3))
                .add(new ConstantExpression(4));
        auto *list = new ListExpression(items);
        auto *index = new ConstantExpression(-4);
        SubscriptExpression expr(list, index);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(1, value.getInt());
    }
    // [1,2,3,4][-5]
    {
        auto *items = new ExpressionList();
        (*items).add(new ConstantExpression(1))
                .add(new ConstantExpression(2))
                .add(new ConstantExpression(3))
                .add(new ConstantExpression(4));
        auto *list = new ListExpression(items);
        auto *index = new ConstantExpression(-5);
        SubscriptExpression expr(list, index);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBadNull());
    }
    // [1,2,3,4]["0"]
    {
        auto *items = new ExpressionList();
        (*items).add(new ConstantExpression(1))
                .add(new ConstantExpression(2))
                .add(new ConstantExpression(3))
                .add(new ConstantExpression(4));
        auto *list = new ListExpression(items);
        auto *index = new ConstantExpression("0");
        SubscriptExpression expr(list, index);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBadNull());
    }
    // 1[0]
    {
        auto *integer = new ConstantExpression(1);
        auto *index = new ConstantExpression(0);
        SubscriptExpression expr(integer, index);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBadNull());
    }
}

TEST_F(ExpressionTest, ListSubscriptRange) {
    auto *items = new ExpressionList();
    (*items).add(new ConstantExpression(0))
            .add(new ConstantExpression(1))
            .add(new ConstantExpression(2))
            .add(new ConstantExpression(3))
            .add(new ConstantExpression(4))
            .add(new ConstantExpression(5));
    auto list = std::make_unique<ListExpression>(items);
    // [0,1,2,3,4,5][0..] => [0,1,2,3,4,5]
    {
        auto *lo = new ConstantExpression(0);
        SubscriptRangeExpression expr(list->clone().release(), lo, nullptr);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List({0, 1, 2, 3, 4, 5}), value.getList());
    }
    // [0,1,2,3,4,5][0..0] => []
    {
        auto *lo = new ConstantExpression(0);
        auto *hi = new ConstantExpression(0);
        SubscriptRangeExpression expr(list->clone().release(), lo, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List(), value.getList());
    }
    // [0,1,2,3,4,5][0..10] => [0,1,2,3,4,5]
    {
        auto *lo = new ConstantExpression(0);
        auto *hi = new ConstantExpression(10);
        SubscriptRangeExpression expr(list->clone().release(), lo, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List({0, 1, 2, 3, 4, 5}), value.getList());
    }
    // [0,1,2,3,4,5][3..2] => []
    {
        auto *lo = new ConstantExpression(3);
        auto *hi = new ConstantExpression(2);
        SubscriptRangeExpression expr(list->clone().release(), lo, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List(), value.getList());
    }
    // [0,1,2,3,4,5][..-1] => [0,1,2,3,4]
    {
        auto *hi = new ConstantExpression(-1);
        SubscriptRangeExpression expr(list->clone().release(), nullptr, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List({0, 1, 2, 3, 4}), value.getList());
    }
    // [0,1,2,3,4,5][-1..-1] => []
    {
        auto *lo = new ConstantExpression(-1);
        auto *hi = new ConstantExpression(-1);
        SubscriptRangeExpression expr(list->clone().release(), lo, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List(), value.getList());
    }
    // [0,1,2,3,4,5][-10..-1] => [0,1,2,3,4]
    {
        auto *lo = new ConstantExpression(-10);
        auto *hi = new ConstantExpression(-1);
        SubscriptRangeExpression expr(list->clone().release(), lo, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List({0, 1, 2, 3, 4}), value.getList());
    }
    // [0,1,2,3,4,5][-2..-3] => []
    {
        auto *lo = new ConstantExpression(-2);
        auto *hi = new ConstantExpression(-3);
        SubscriptRangeExpression expr(list->clone().release(), lo, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List(), value.getList());
    }
    // [0,1,2,3,4,5][2..-3] => [2]
    {
        auto *lo = new ConstantExpression(2);
        auto *hi = new ConstantExpression(-3);
        SubscriptRangeExpression expr(list->clone().release(), lo, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List({2}), value.getList());
    }
    // [0,1,2,3,4,5][-2..3] => []
    {
        auto *lo = new ConstantExpression(-2);
        auto *hi = new ConstantExpression(3);
        SubscriptRangeExpression expr(list->clone().release(), lo, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List(), value.getList());
    }
}

TEST_F(ExpressionTest, MapSubscript) {
    // {"key1":1,"key2":2, "key3":3}["key1"]
    {
        auto *items = new MapItemList();
        (*items).add("key1", new ConstantExpression(1))
                .add("key2", new ConstantExpression(2))
                .add("key3", new ConstantExpression(3));
        auto *map = new MapExpression(items);
        auto *key = new ConstantExpression("key1");
        SubscriptExpression expr(map, key);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(1, value.getInt());
    }
    // {"key1":1,"key2":2, "key3":3}["key4"]
    {
        auto *items = new MapItemList();
        (*items).add("key1", new ConstantExpression(1))
                .add("key2", new ConstantExpression(2))
                .add("key3", new ConstantExpression(3));
        auto *map = new MapExpression(items);
        auto *key = new ConstantExpression("key4");
        SubscriptExpression expr(map, key);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isNull());
        ASSERT_FALSE(value.isBadNull());
    }
    // {"key1":1,"key2":2, "key3":3}[0]
    {
        auto *items = new MapItemList();
        (*items).add("key1", new ConstantExpression(1))
                .add("key2", new ConstantExpression(2))
                .add("key3", new ConstantExpression(3));
        auto *map = new MapExpression(items);
        auto *key = new ConstantExpression(0);
        SubscriptExpression expr(map, key);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isNull());
        ASSERT_FALSE(value.isBadNull());
    }
}

TEST_F(ExpressionTest, VertexSubscript) {
    Vertex vertex;
    vertex.vid = "vid";
    vertex.tags.resize(2);
    vertex.tags[0].props = {
        {"Venus", "Mars"},
        {"Mull", "Kintyre"},
    };
    vertex.tags[1].props = {
        {"Bip", "Bop"},
        {"Tug", "War"},
        {"Venus", "RocksShow"},
    };
    {
        auto *left = new ConstantExpression(Value(vertex));
        auto *right = new ConstantExpression("Mull");
        SubscriptExpression expr(left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("Kintyre", value.getStr());
    }
    {
        auto *left = new ConstantExpression(Value(vertex));
        auto *right = new LabelExpression("Bip");
        SubscriptExpression expr(left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("Bop", value.getStr());
    }
    {
        auto *left = new ConstantExpression(Value(vertex));
        auto *right = new LabelExpression("Venus");
        SubscriptExpression expr(left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("Mars", value.getStr());
    }
    {
        auto *left = new ConstantExpression(Value(vertex));
        auto *right = new LabelExpression("_vid");
        SubscriptExpression expr(left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("vid", value.getStr());
    }
}

TEST_F(ExpressionTest, EdgeSubscript) {
    Edge edge;
    edge.name = "type";
    edge.src = "src";
    edge.dst = "dst";
    edge.ranking = 123;
    edge.props = {
        {"Magill", "Nancy"},
        {"Gideon", "Bible"},
        {"Rocky", "Raccoon"},
    };
    {
        auto *left = new ConstantExpression(Value(edge));
        auto *right = new ConstantExpression("Rocky");
        SubscriptExpression expr(left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("Raccoon", value.getStr());
    }
    {
        auto *left = new ConstantExpression(Value(edge));
        auto *right = new ConstantExpression(kType);
        SubscriptExpression expr(left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("type", value.getStr());
    }
    {
        auto *left = new ConstantExpression(Value(edge));
        auto *right = new ConstantExpression(kSrc);
        SubscriptExpression expr(left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("src", value.getStr());
    }
    {
        auto *left = new ConstantExpression(Value(edge));
        auto *right = new ConstantExpression(kDst);
        SubscriptExpression expr(left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("dst", value.getStr());
    }
    {
        auto *left = new ConstantExpression(Value(edge));
        auto *right = new ConstantExpression(kRank);
        SubscriptExpression expr(left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(123, value.getInt());
    }
}

TEST_F(ExpressionTest, TypeCastTest) {
    {
        TypeCastingExpression typeCast(Value::Type::INT, new ConstantExpression(1));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        TypeCastingExpression typeCast(Value::Type::INT, new ConstantExpression(1.23));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        TypeCastingExpression typeCast(Value::Type::INT, new ConstantExpression("1.23"));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        TypeCastingExpression typeCast(Value::Type::INT, new ConstantExpression("123"));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 123);
    }
    {
        TypeCastingExpression typeCast(Value::Type::INT, new ConstantExpression(".123"));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 0);
    }
    {
        TypeCastingExpression typeCast(Value::Type::INT, new ConstantExpression(".123ab"));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        TypeCastingExpression typeCast(Value::Type::INT, new ConstantExpression("abc123"));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        TypeCastingExpression typeCast(Value::Type::INT, new ConstantExpression("123abc"));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        TypeCastingExpression typeCast(Value::Type::INT, new ConstantExpression(true));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }

    {
        TypeCastingExpression typeCast(Value::Type::FLOAT, new ConstantExpression(1.23));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::FLOAT);
        EXPECT_EQ(eval, 1.23);
    }
    {
        TypeCastingExpression typeCast(Value::Type::FLOAT, new ConstantExpression(2));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::FLOAT);
        EXPECT_EQ(eval, 2.0);
    }
    {
        TypeCastingExpression typeCast(Value::Type::FLOAT, new ConstantExpression("1.23"));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::FLOAT);
        EXPECT_EQ(eval, 1.23);
    }
    {
        TypeCastingExpression typeCast(Value::Type::BOOL, new ConstantExpression(2));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        TypeCastingExpression typeCast(Value::Type::BOOL, new ConstantExpression(0));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        TypeCastingExpression typeCast(Value::Type::STRING, new ConstantExpression(true));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::STRING);
        EXPECT_EQ(eval, "true");
    }
    {
        TypeCastingExpression typeCast(Value::Type::STRING, new ConstantExpression(false));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::STRING);
        EXPECT_EQ(eval, "false");
    }
    {
        TypeCastingExpression typeCast(Value::Type::STRING, new ConstantExpression(12345));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::STRING);
        EXPECT_EQ(eval, "12345");
    }
    {
        TypeCastingExpression typeCast(Value::Type::STRING, new ConstantExpression(34.23));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::STRING);
        EXPECT_EQ(eval, "34.23");
    }
    {
        TypeCastingExpression typeCast(Value::Type::STRING, new ConstantExpression(.23));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::STRING);
        EXPECT_EQ(eval, "0.23");
    }
    {
        TypeCastingExpression typeCast(Value::Type::SET, new ConstantExpression(23));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        TypeCastingExpression typeCast(Value::Type::INT, new ConstantExpression(Set()));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
}

TEST_F(ExpressionTest, RelationRegexMatch) {
    {
        RelationalExpression expr(
                Expression::Kind::kRelREG,
                new ConstantExpression("abcd\xA3g1234efgh\x49ijkl"),
                new ConstantExpression("\\w{4}\xA3g12\\d*e\\w+\x49\\w+"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kRelREG,
                new ConstantExpression("Tony Parker"),
                new ConstantExpression("T.*er"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kRelREG,
                new ConstantExpression("010-12345"),
                new ConstantExpression("\\d{3}\\-\\d{3,8}"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kRelREG,
                new ConstantExpression("test_space_128"),
                new ConstantExpression("[a-zA-Z_][0-9a-zA-Z_]{0,19}"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kRelREG,
                new ConstantExpression("2001-09-01 08:00:00"),
                new ConstantExpression("\\d+\\-0\\d?\\-\\d+\\s\\d+:00:\\d+"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kRelREG,
                new ConstantExpression("jack138tom发."),
                new ConstantExpression("j\\w*\\d+\\w+\u53d1\\."));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kRelREG,
                new ConstantExpression("jack138tom\u53d1.34数数数"),
                new ConstantExpression("j\\w*\\d+\\w+发\\.34[\u4e00-\u9fa5]+"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kRelREG,
                new ConstantExpression("a good person"),
                new ConstantExpression("a\\s\\w+"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kRelREG,
                new ConstantExpression("Tony Parker"),
                new ConstantExpression("T\\w+\\s?\\P\\d+"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kRelREG,
                new ConstantExpression("010-12345"),
                new ConstantExpression("\\d?\\-\\d{3,8}"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kRelREG,
                new ConstantExpression("test_space_128牛"),
                new ConstantExpression("[a-zA-Z_][0-9a-zA-Z_]{0,19}"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kRelREG,
                new ConstantExpression("2001-09-01 08:00:00"),
                new ConstantExpression("\\d+\\s\\d+:00:\\d+"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kRelREG,
                new ConstantExpression(Value::kNullBadData),
                new ConstantExpression(Value::kNullBadType));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kRelREG,
                new ConstantExpression(Value::kNullValue),
                new ConstantExpression(3));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kRelREG,
                new ConstantExpression(3),
                new ConstantExpression(true));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kRelREG,
                new ConstantExpression("abc"),
                new ConstantExpression(Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
}

TEST_F(ExpressionTest, RelationContains) {
    {
        // "abc" contains "a"
        RelationalExpression expr(
                Expression::Kind::kContains,
                new ConstantExpression("abc"),
                new ConstantExpression("a"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" contains "bc"
        RelationalExpression expr(
                Expression::Kind::kContains,
                new ConstantExpression("abc"),
                new ConstantExpression("bc"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" contains "d"
        RelationalExpression expr(
                Expression::Kind::kContains,
                new ConstantExpression("abc"),
                new ConstantExpression("d"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" contains 1
        RelationalExpression expr(
                Expression::Kind::kContains,
                new ConstantExpression("abc1"),
                new ConstantExpression(1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        // 1234 contains 1
        RelationalExpression expr(
                Expression::Kind::kContains,
                new ConstantExpression(1234),
                new ConstantExpression(1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
}

TEST_F(ExpressionTest, RelationStartsWith) {
    {
        // "abc" starts with "a"
        RelationalExpression expr(
                Expression::Kind::kStartsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("a"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" starts with "ab"
        RelationalExpression expr(
                Expression::Kind::kStartsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("ab"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "1234"" starts with "12"
        RelationalExpression expr(
                Expression::Kind::kStartsWith,
                new ConstantExpression("1234"),
                new ConstantExpression("12"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "1234"" starts with "34"
        RelationalExpression expr(
                Expression::Kind::kStartsWith,
                new ConstantExpression("1234"),
                new ConstantExpression("34"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" starts with "bc"
        RelationalExpression expr(
                Expression::Kind::kStartsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("bc"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" starts with "ac"
        RelationalExpression expr(
                Expression::Kind::kStartsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("ac"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" starts with "AB"
        RelationalExpression expr(
                Expression::Kind::kStartsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("AB"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" starts with 1
        RelationalExpression expr(
                Expression::Kind::kStartsWith,
                new ConstantExpression("abc1"),
                new ConstantExpression(1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        // 1234 starts with 1
        RelationalExpression expr(
                Expression::Kind::kStartsWith,
                new ConstantExpression(1234),
                new ConstantExpression(1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kStartsWith,
                new ConstantExpression(1234),
                new ConstantExpression(Value::kNullBadData));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kStartsWith,
                new ConstantExpression(1234),
                new ConstantExpression("1234"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kStartsWith,
                new ConstantExpression(Value::kNullValue),
                new ConstantExpression("NULL"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kStartsWith,
                new ConstantExpression("Null"),
                new ConstantExpression(Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
}

TEST_F(ExpressionTest, RelationNotStartsWith) {
    {
        // "abc" not starts with "a"
        RelationalExpression expr(
                Expression::Kind::kNotStartsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("a"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" not starts with "ab"
        RelationalExpression expr(
                Expression::Kind::kNotStartsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("ab"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "1234"" not starts with "12"
        RelationalExpression expr(
                Expression::Kind::kNotStartsWith,
                new ConstantExpression("1234"),
                new ConstantExpression("12"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "1234"" not starts with "34"
        RelationalExpression expr(
                Expression::Kind::kNotStartsWith,
                new ConstantExpression("1234"),
                new ConstantExpression("34"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" not starts with "bc"
        RelationalExpression expr(
                Expression::Kind::kNotStartsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("bc"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" not starts with "ac"
        RelationalExpression expr(
                Expression::Kind::kNotStartsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("ac"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" not starts with "AB"
        RelationalExpression expr(
                Expression::Kind::kNotStartsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("AB"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" not starts with 1
        RelationalExpression expr(
                Expression::Kind::kNotStartsWith,
                new ConstantExpression("abc1"),
                new ConstantExpression(1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        // 1234 not starts with 1
        RelationalExpression expr(
                Expression::Kind::kNotStartsWith,
                new ConstantExpression(1234),
                new ConstantExpression(1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kNotStartsWith,
                new ConstantExpression(1234),
                new ConstantExpression(Value::kNullBadData));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kNotStartsWith,
                new ConstantExpression(1234),
                new ConstantExpression("1234"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kNotStartsWith,
                new ConstantExpression(Value::kNullValue),
                new ConstantExpression("NULL"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kNotStartsWith,
                new ConstantExpression("Null"),
                new ConstantExpression(Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
}

TEST_F(ExpressionTest, RelationEndsWith) {
    {
        // "abc" ends with "a"
        RelationalExpression expr(
                Expression::Kind::kEndsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("a"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" ends with "ab"
        RelationalExpression expr(
                Expression::Kind::kEndsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("ab"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "1234"" ends with "12"
        RelationalExpression expr(
                Expression::Kind::kEndsWith,
                new ConstantExpression("1234"),
                new ConstantExpression("12"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "1234"" ends with "34"
        RelationalExpression expr(
                Expression::Kind::kEndsWith,
                new ConstantExpression("1234"),
                new ConstantExpression("34"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" ends with "bc"
        RelationalExpression expr(
                Expression::Kind::kEndsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("bc"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" ends with "ac"
        RelationalExpression expr(
                Expression::Kind::kEndsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("ac"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" ends with "AB"
        RelationalExpression expr(
                Expression::Kind::kEndsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("AB"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" ends with "BC"
        RelationalExpression expr(
                Expression::Kind::kEndsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("BC"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" ends with 1
        RelationalExpression expr(
                Expression::Kind::kEndsWith,
                new ConstantExpression("abc1"),
                new ConstantExpression(1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        // 1234 ends with 1
        RelationalExpression expr(
                Expression::Kind::kEndsWith,
                new ConstantExpression(1234),
                new ConstantExpression(1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        // "steve jobs" ends with "jobs"
        RelationalExpression expr(
                Expression::Kind::kEndsWith,
                new ConstantExpression("steve jobs"),
                new ConstantExpression("jobs"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kEndsWith,
                new ConstantExpression(1234),
                new ConstantExpression(Value::kNullBadData));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kEndsWith,
                new ConstantExpression(1234),
                new ConstantExpression("1234"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kEndsWith,
                new ConstantExpression(Value::kNullValue),
                new ConstantExpression("NULL"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kEndsWith,
                new ConstantExpression("Null"),
                new ConstantExpression(Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
}

TEST_F(ExpressionTest, RelationNotEndsWith) {
    {
        // "abc" not ends with "a"
        RelationalExpression expr(
                Expression::Kind::kNotEndsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("a"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" not ends with "ab"
        RelationalExpression expr(
                Expression::Kind::kNotEndsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("ab"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "1234"" not ends with "12"
        RelationalExpression expr(
                Expression::Kind::kNotEndsWith,
                new ConstantExpression("1234"),
                new ConstantExpression("12"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "1234"" not ends with "34"
        RelationalExpression expr(
                Expression::Kind::kNotEndsWith,
                new ConstantExpression("1234"),
                new ConstantExpression("34"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" not ends with "bc"
        RelationalExpression expr(
                Expression::Kind::kNotEndsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("bc"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" not ends with "ac"
        RelationalExpression expr(
                Expression::Kind::kNotEndsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("ac"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" not ends with "AB"
        RelationalExpression expr(
                Expression::Kind::kNotEndsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("AB"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" not ends with "BC"
        RelationalExpression expr(
                Expression::Kind::kNotEndsWith,
                new ConstantExpression("abc"),
                new ConstantExpression("BC"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" not ends with 1
        RelationalExpression expr(
                Expression::Kind::kNotEndsWith,
                new ConstantExpression("abc1"),
                new ConstantExpression(1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        // 1234 not ends with 1
        RelationalExpression expr(
                Expression::Kind::kNotEndsWith,
                new ConstantExpression(1234),
                new ConstantExpression(1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kNotEndsWith,
                new ConstantExpression(1234),
                new ConstantExpression(Value::kNullBadData));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kNotEndsWith,
                new ConstantExpression(1234),
                new ConstantExpression("1234"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kNotEndsWith,
                new ConstantExpression(Value::kNullValue),
                new ConstantExpression("NULL"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kNotEndsWith,
                new ConstantExpression("Null"),
                new ConstantExpression(Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
}

TEST_F(ExpressionTest, ContainsToString) {
    {
        // "abc" contains "a"
        RelationalExpression expr(
                Expression::Kind::kContains,
                new ConstantExpression("abc"),
                new ConstantExpression("a"));
        ASSERT_EQ("(\"abc\" CONTAINS \"a\")", expr.toString());
    }
    {
        RelationalExpression expr(
                Expression::Kind::kContains,
                new ConstantExpression(1234),
                new ConstantExpression(Value::kNullBadData));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kContains,
                new ConstantExpression(1234),
                new ConstantExpression("1234"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kContains,
                new ConstantExpression(Value::kNullValue),
                new ConstantExpression("NULL"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kContains,
                new ConstantExpression("Null"),
                new ConstantExpression(Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
}

TEST_F(ExpressionTest, NotContainsToString) {
    {
        // "abc" not contains "a"
        RelationalExpression expr(
                Expression::Kind::kNotContains,
                new ConstantExpression("abc"),
                new ConstantExpression("a"));
        ASSERT_EQ("(\"abc\" NOT CONTAINS \"a\")", expr.toString());
    }
    {
        RelationalExpression expr(
                Expression::Kind::kNotContains,
                new ConstantExpression(1234),
                new ConstantExpression(Value::kNullBadData));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kNotContains,
                new ConstantExpression(1234),
                new ConstantExpression("1234"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kNotContains,
                new ConstantExpression(Value::kNullValue),
                new ConstantExpression("NULL"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
    {
        RelationalExpression expr(
                Expression::Kind::kNotContains,
                new ConstantExpression("Null"),
                new ConstantExpression(Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
}

TEST_F(ExpressionTest, CaseExprToString) {
    {
        auto *cases = new CaseList();
        cases->add(new ConstantExpression(24), new ConstantExpression(1));
        CaseExpression expr(cases);
        expr.setCondition(new ConstantExpression(23));
        ASSERT_EQ("CASE 23 WHEN 24 THEN 1 END", expr.toString());
    }
    {
        auto *cases = new CaseList();
        cases->add(new ConstantExpression(24), new ConstantExpression(1));
        CaseExpression expr(cases);
        expr.setCondition(new ConstantExpression(23));
        expr.setDefault(new ConstantExpression(2));
        ASSERT_EQ("CASE 23 WHEN 24 THEN 1 ELSE 2 END", expr.toString());
    }
    {
        auto *cases = new CaseList();
        cases->add(new ConstantExpression(false), new ConstantExpression(1));
        cases->add(new ConstantExpression(true), new ConstantExpression(2));
        CaseExpression expr(cases);
        expr.setCondition(RelationalExpression::makeStartsWith(&pool,
                                                   new ConstantExpression("nebula"),
                                                   new ConstantExpression("nebu")));
        expr.setDefault(new ConstantExpression(3));
        ASSERT_EQ(
            "CASE (\"nebula\" STARTS WITH \"nebu\") WHEN false THEN 1 WHEN true THEN 2 ELSE 3 END",
            expr.toString());
    }
    {
        auto *cases = new CaseList();
        cases->add(new ConstantExpression(7), new ConstantExpression(1));
        cases->add(new ConstantExpression(8), new ConstantExpression(2));
        cases->add(new ConstantExpression(8), new ConstantExpression("jack"));
        CaseExpression expr(cases);
        expr.setCondition(ArithmeticExpression::makeAdd(
            &pool, new ConstantExpression(3), new ConstantExpression(5)));
        expr.setDefault(new ConstantExpression(false));
        ASSERT_EQ("CASE (3+5) WHEN 7 THEN 1 WHEN 8 THEN 2 WHEN 8 THEN \"jack\" ELSE false END",
                  expr.toString());
    }
    {
        auto *cases = new CaseList();
        cases->add(new ConstantExpression(false), new ConstantExpression(18));
        CaseExpression expr(cases);
        ASSERT_EQ("CASE WHEN false THEN 18 END", expr.toString());
    }
    {
        auto *cases = new CaseList();
        cases->add(new ConstantExpression(false), new ConstantExpression(18));
        CaseExpression expr(cases);
        expr.setDefault(new ConstantExpression("ok"));
        ASSERT_EQ("CASE WHEN false THEN 18 ELSE \"ok\" END", expr.toString());
    }
    {
        auto *cases = new CaseList();
        cases->add(RelationalExpression::makeStartsWith(&pool,
                                            new ConstantExpression("nebula"),
                                            new ConstantExpression("nebu")),
                   new ConstantExpression("yes"));
        CaseExpression expr(cases);
        expr.setDefault(new ConstantExpression(false));
        ASSERT_EQ("CASE WHEN (\"nebula\" STARTS WITH \"nebu\") THEN \"yes\" ELSE false END",
                  expr.toString());
    }
    {
        auto *cases = new CaseList();
        cases->add(
            new RelationalExpression(
                Expression::Kind::kRelLT, new ConstantExpression(23), new ConstantExpression(17)),
            new ConstantExpression(1));
        cases->add(
            new RelationalExpression(
                Expression::Kind::kRelEQ, new ConstantExpression(37), new ConstantExpression(37)),
            new ConstantExpression(2));
        cases->add(
            new RelationalExpression(
                Expression::Kind::kRelNE, new ConstantExpression(45), new ConstantExpression(99)),
            new ConstantExpression(3));
        CaseExpression expr(cases);
        expr.setDefault(new ConstantExpression(4));
        ASSERT_EQ("CASE WHEN (23<17) THEN 1 WHEN (37==37) THEN 2 WHEN (45!=99) THEN 3 ELSE 4 END",
                  expr.toString());
    }
    {
        auto *cases = new CaseList();
        cases->add(
            new RelationalExpression(
                Expression::Kind::kRelLT, new ConstantExpression(23), new ConstantExpression(17)),
            new ConstantExpression(1));
        CaseExpression expr(cases, false);
        expr.setDefault(new ConstantExpression(2));
        ASSERT_EQ("((23<17) ? 1 : 2)", expr.toString());
    }
    {
        auto *cases = new CaseList();
        cases->add(new ConstantExpression(false), new ConstantExpression(1));
        CaseExpression expr(cases, false);
        expr.setDefault(new ConstantExpression("ok"));
        ASSERT_EQ("(false ? 1 : \"ok\")", expr.toString());
    }
}

TEST_F(ExpressionTest, CaseEvaluate) {
    {
        // CASE 23 WHEN 24 THEN 1 END
        auto *cases = new CaseList();
        cases->add(new ConstantExpression(24), new ConstantExpression(1));
        CaseExpression expr(cases);
        expr.setCondition(new ConstantExpression(23));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_EQ(value, Value::kNullValue);
    }
    {
        // CASE 23 WHEN 24 THEN 1 ELSE false END
        auto *cases = new CaseList();
        cases->add(new ConstantExpression(24), new ConstantExpression(1));
        CaseExpression expr(cases);
        expr.setCondition(new ConstantExpression(23));
        expr.setDefault(new ConstantExpression(false));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(value.getBool(), false);
    }
    {
        // CASE ("nebula" STARTS WITH "nebu") WHEN false THEN 1 WHEN true THEN 2 ELSE 3 END
        auto *cases = new CaseList();
        cases->add(new ConstantExpression(false), new ConstantExpression(1));
        cases->add(new ConstantExpression(true), new ConstantExpression(2));
        CaseExpression expr(cases);
        expr.setCondition(RelationalExpression::makeStartsWith(&pool,
                                                   new ConstantExpression("nebula"),
                                                   new ConstantExpression("nebu")));
        expr.setDefault(new ConstantExpression(3));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(2, value.getInt());
    }
    {
        // CASE (3+5) WHEN 7 THEN 1 WHEN 8 THEN 2 WHEN 8 THEN "jack" ELSE "no" END
        auto *cases = new CaseList();
        cases->add(new ConstantExpression(7), new ConstantExpression(1));
        cases->add(new ConstantExpression(8), new ConstantExpression(2));
        cases->add(new ConstantExpression(8), new ConstantExpression("jack"));
        CaseExpression expr(cases);
        expr.setCondition(ArithmeticExpression::makeAdd(
            &pool, new ConstantExpression(3), new ConstantExpression(5)));
        expr.setDefault(new ConstantExpression("no"));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(2, value.getInt());
    }
    {
        // CASE WHEN false THEN 18 END
        auto *cases = new CaseList();
        cases->add(new ConstantExpression(false), new ConstantExpression(18));
        CaseExpression expr(cases);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_EQ(value, Value::kNullValue);
    }
    {
        // CASE WHEN false THEN 18 ELSE ok END
        auto *cases = new CaseList();
        cases->add(new ConstantExpression(false), new ConstantExpression(18));
        CaseExpression expr(cases);
        expr.setDefault(new ConstantExpression("ok"));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("ok", value.getStr());
    }
    {
        // CASE WHEN "invalid when" THEN "no" ELSE 3 END
        auto *cases = new CaseList();
        cases->add(new ConstantExpression("invalid when"), new ConstantExpression("no"));
        CaseExpression expr(cases);
        expr.setDefault(new ConstantExpression(3));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_EQ(value, Value::kNullBadType);
    }
    {
        // CASE WHEN (23<17) THEN 1 WHEN (37==37) THEN 2 WHEN (45!=99) THEN 3 ELSE 4 END
        auto *cases = new CaseList();
        cases->add(
            new RelationalExpression(
                Expression::Kind::kRelLT, new ConstantExpression(23), new ConstantExpression(17)),
            new ConstantExpression(1));
        cases->add(
            new RelationalExpression(
                Expression::Kind::kRelEQ, new ConstantExpression(37), new ConstantExpression(37)),
            new ConstantExpression(2));
        cases->add(
            new RelationalExpression(
                Expression::Kind::kRelNE, new ConstantExpression(45), new ConstantExpression(99)),
            new ConstantExpression(3));
        CaseExpression expr(cases);
        expr.setDefault(new ConstantExpression(4));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(2, value.getInt());
    }
    {
        // ((23<17) ? 1 : 2)
        auto *cases = new CaseList();
        cases->add(
            new RelationalExpression(
                Expression::Kind::kRelLT, new ConstantExpression(23), new ConstantExpression(17)),
            new ConstantExpression(1));
        CaseExpression expr(cases, false);
        expr.setDefault(new ConstantExpression(2));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(2, value.getInt());
    }
    {
        // (false ? 1 : "ok")
        auto *cases = new CaseList();
        cases->add(new ConstantExpression(false), new ConstantExpression(1));
        CaseExpression expr(cases, false);
        expr.setDefault(new ConstantExpression("ok"));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("ok", value.getStr());
    }
}

TEST_F(ExpressionTest, PredicateExprToString) {
    {
        ArgumentList *argList = ArgumentList::make(&pool);
        argList->addArgument(ConstantExpression::make(&pool, 1));
        argList->addArgument(ConstantExpression::make(&pool, 5));
        PredicateExpression expr(
            "all",
            "n",
            FunctionCallExpression::make(&pool, "range", argList),
            new RelationalExpression(
                Expression::Kind::kRelGE,
                new LabelExpression("n"),
                new ConstantExpression(2)));
        ASSERT_EQ("all(n IN range(1,5) WHERE (n>=2))", expr.toString());
    }
}

TEST_F(ExpressionTest, PredicateEvaluate) {
    {
        // all(n IN [0, 1, 2, 4, 5) WHERE n >= 2)
        auto *listItems = new ExpressionList();
        (*listItems)
            .add(new ConstantExpression(0))
            .add(new ConstantExpression(1))
            .add(new ConstantExpression(2))
            .add(new ConstantExpression(4))
            .add(new ConstantExpression(5));
        PredicateExpression expr(
            "all",
            "n",
            new ListExpression(listItems),
            new RelationalExpression(
                Expression::Kind::kRelGE,
                new VariableExpression("n"),
                new ConstantExpression(2)));

        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value.getBool());
    }
    {
        // any(n IN nodes(p) WHERE n.age >= 19)
        auto v1 = Vertex("101", {Tag("player", {{"name", "joe"}, {"age", 18}})});
        auto v2 = Vertex("102", {Tag("player", {{"name", "amber"}, {"age", 19}})});
        auto v3 = Vertex("103", {Tag("player", {{"name", "shawdan"}, {"age", 20}})});
        Path path;
        path.src = v1;
        path.steps.emplace_back(Step(v2, 1, "like", 0, {}));
        path.steps.emplace_back(Step(v3, 1, "like", 0, {}));
        gExpCtxt.setVar("p", path);

        ArgumentList *argList = ArgumentList::make(&pool);
        argList->addArgument(new VariableExpression("p"));
        PredicateExpression expr(
            "any",
            "n",
            FunctionCallExpression::make(&pool, "nodes", argList),
            new RelationalExpression(
                Expression::Kind::kRelGE,
                AttributeExpression::make(&pool, new VariableExpression("n"),
                                        new ConstantExpression("age")),
                new ConstantExpression(19)));

        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(true, value.getBool());
    }
    {
        // single(n IN [0, 1, 2, 4, 5) WHERE n == 2)
        auto *listItems = new ExpressionList();
        (*listItems)
            .add(new ConstantExpression(0))
            .add(new ConstantExpression(1))
            .add(new ConstantExpression(2))
            .add(new ConstantExpression(4))
            .add(new ConstantExpression(5));
        PredicateExpression expr(
            "single",
            "n",
            new ListExpression(listItems),
            new RelationalExpression(
                Expression::Kind::kRelEQ,
                new VariableExpression("n"),
                new ConstantExpression(2)));

        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(true, value.getBool());
    }
    {
        // none(n IN nodes(p) WHERE n.age >= 19)
        auto v1 = Vertex("101", {Tag("player", {{"name", "joe"}, {"age", 18}})});
        auto v2 = Vertex("102", {Tag("player", {{"name", "amber"}, {"age", 19}})});
        auto v3 = Vertex("103", {Tag("player", {{"name", "shawdan"}, {"age", 20}})});
        Path path;
        path.src = v1;
        path.steps.emplace_back(Step(v2, 1, "like", 0, {}));
        path.steps.emplace_back(Step(v3, 1, "like", 0, {}));
        gExpCtxt.setVar("p", path);

        ArgumentList *argList = ArgumentList::make(&pool);
        argList->addArgument(new VariableExpression("p"));
        PredicateExpression expr(
            "none",
            "n",
            FunctionCallExpression::make(&pool, "nodes", argList),
            new RelationalExpression(
                Expression::Kind::kRelGE,
                AttributeExpression::make(&pool, new VariableExpression("n"),
                                        new ConstantExpression("age")),
                new ConstantExpression(19)));

        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value.getBool());
    }
    {
        // single(n IN null WHERE n > 1)
        PredicateExpression expr(
            "all",
            "n",
            new ConstantExpression(Value(NullType::__NULL__)),
            new RelationalExpression(
                Expression::Kind::kRelEQ,
                new VariableExpression("n"),
                new ConstantExpression(1)));

        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_EQ(Value::kNullValue, value.getNull());
    }
}

TEST_F(ExpressionTest, ReduceExprToString) {
    {
        // reduce(totalNum = 2 * 10, n IN range(1, 5) | totalNum + n * 2)
        ArgumentList *argList = ArgumentList::make(&pool);
        argList->addArgument(ConstantExpression::make(&pool, 1));
        argList->addArgument(ConstantExpression::make(&pool, 5));
        ReduceExpression expr(
            "totalNum",
            ArithmeticExpression::makeMultiply(
                &pool, new ConstantExpression(2), new ConstantExpression(10)),
            "n",
            FunctionCallExpression::make(&pool, "range", argList),
            ArithmeticExpression::makeAdd(
                &pool,
                new LabelExpression("totalNum"),
                ArithmeticExpression::makeMultiply(
                    &pool, new LabelExpression("n"), new ConstantExpression(2))));
        ASSERT_EQ("reduce(totalNum = (2*10), n IN range(1,5) | (totalNum+(n*2)))", expr.toString());
    }
}

TEST_F(ExpressionTest, ReduceEvaluate) {
    {
        // reduce(totalNum = 2 * 10, n IN range(1, 5) | totalNum + n * 2)
        ArgumentList *argList = ArgumentList::make(&pool);
        argList->addArgument(ConstantExpression::make(&pool, 1));
        argList->addArgument(ConstantExpression::make(&pool, 5));
        ReduceExpression expr(
            "totalNum",
            ArithmeticExpression::makeMultiply(
                &pool, new ConstantExpression(2), new ConstantExpression(10)),
            "n",
            FunctionCallExpression::make(&pool, "range", argList),
            ArithmeticExpression::makeAdd(
                &pool,
                new VariableExpression("totalNum"),
                ArithmeticExpression::makeMultiply(&pool,
                                                   new VariableExpression("n"),
                                                   new ConstantExpression(2))));

        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_EQ(Value::Type::INT, value.type());
        ASSERT_EQ(50, value.getInt());
    }
}

TEST_F(ExpressionTest, TestExprClone) {
    ConstantExpression expr(1);
    auto clone = expr.clone();
    ASSERT_EQ(*clone, expr);

    auto aexpr =
        ArithmeticExpression::makeAdd(&pool, new ConstantExpression(1), new ConstantExpression(1));
    auto aclone = aexpr->clone();
    ASSERT_EQ(*aexpr, *aexpr->clone());

    auto aggExpr = AggregateExpression::make(
        &pool, "COUNT", new ConstantExpression("$-.*"), true);
    ASSERT_EQ(*aggExpr, *aggExpr->clone());

    auto edgeExpr = EdgeExpression::make(&pool);
    ASSERT_EQ(*edgeExpr, *edgeExpr->clone());

    VertexExpression vertExpr;
    ASSERT_EQ(vertExpr, *vertExpr.clone());

    auto labelExpr = LabelExpression::make(&pool, "label");
    ASSERT_EQ(*labelExpr, *labelExpr->clone());

    auto attrExpr = AttributeExpression::make(
        &pool, LabelExpression::make(&pool, "label"), LabelExpression::make(&pool, "label"));
    ASSERT_EQ(*attrExpr, *attrExpr->clone());

    auto labelAttrExpr =
        LabelAttributeExpression::make(&pool,
                                       LabelExpression::make(&pool, "label"),
                                       ConstantExpression::make(&pool, "prop"));
    ASSERT_EQ(*labelAttrExpr, *labelAttrExpr->clone());

    TypeCastingExpression typeCastExpr(Value::Type::STRING, new ConstantExpression(100));
    ASSERT_EQ(typeCastExpr, *typeCastExpr.clone());

    auto fnCallExpr =
        FunctionCallExpression::make(&pool, "count", ArgumentList::make(&pool));
    ASSERT_EQ(*fnCallExpr, *fnCallExpr->clone());

    UUIDExpression uuidExpr("hello");
    ASSERT_EQ(uuidExpr, *uuidExpr.clone());

    SubscriptExpression subExpr(new VariableExpression("var"),
                                new ConstantExpression(0));
    ASSERT_EQ(subExpr, *subExpr.clone());

    ListExpression listExpr;
    ASSERT_EQ(listExpr, *listExpr.clone());

    SetExpression setExpr;
    ASSERT_EQ(setExpr, *setExpr.clone());

    MapExpression mapExpr;
    ASSERT_EQ(mapExpr, *mapExpr.clone());

    EdgePropertyExpression edgePropExpr("edge", "prop");
    ASSERT_EQ(edgePropExpr, *edgePropExpr.clone());

    TagPropertyExpression tagPropExpr("tag", "prop");
    ASSERT_EQ(tagPropExpr, *tagPropExpr.clone());

    InputPropertyExpression inputPropExpr("input");
    ASSERT_EQ(inputPropExpr, *inputPropExpr.clone());

    VariablePropertyExpression varPropExpr("var", "prop");
    ASSERT_EQ(varPropExpr, *varPropExpr.clone());

    SourcePropertyExpression srcPropExpr("tag", "prop");
    ASSERT_EQ(srcPropExpr, *srcPropExpr.clone());

    DestPropertyExpression dstPropExpr("tag", "prop");
    ASSERT_EQ(dstPropExpr, *dstPropExpr.clone());

    EdgeSrcIdExpression edgeSrcIdExpr("edge");
    ASSERT_EQ(edgeSrcIdExpr, *edgeSrcIdExpr.clone());

    EdgeTypeExpression edgeTypeExpr("edge");
    ASSERT_EQ(edgeTypeExpr, *edgeTypeExpr.clone());

    EdgeRankExpression edgeRankExpr("edge");
    ASSERT_EQ(edgeRankExpr, *edgeRankExpr.clone());

    EdgeDstIdExpression edgeDstIdExpr("edge");
    ASSERT_EQ(edgeDstIdExpr, *edgeDstIdExpr.clone());

    VariableExpression varExpr("VARNAME");
    auto varExprClone = varExpr.clone();
    ASSERT_EQ(varExpr, *varExprClone);

    VersionedVariableExpression verVarExpr("VARNAME", new ConstantExpression(0));
    ASSERT_EQ(*verVarExpr.clone(), verVarExpr);

    auto *cases = new CaseList();
    cases->add(new ConstantExpression(3), new ConstantExpression(9));
    CaseExpression caseExpr(cases);
    caseExpr.setCondition(new ConstantExpression(2));
    caseExpr.setDefault(new ConstantExpression(8));
    ASSERT_EQ(caseExpr, *caseExpr.clone());

    auto pathBuild = *PathBuildExpression::make(&pool);
    pathBuild.add(new VariablePropertyExpression("var1", "path_src"))
        .add(new VariablePropertyExpression("var1", "path_edge1"))
        .add(new VariablePropertyExpression("var1", "path_v1"));
    ASSERT_EQ(pathBuild, *pathBuild.clone());

    auto argList = ArgumentList::make(&pool);
    argList->addArgument(ConstantExpression::make(&pool, 1));
    argList->addArgument(ConstantExpression::make(&pool, 5));
    auto lcExpr = ListComprehensionExpression::make(
        &pool,
        "n",
        FunctionCallExpression::make(&pool, "range", argList),
        RelationalExpression::makeGE(
            &pool, LabelExpression::make(&pool, "n"), ConstantExpression::make(&pool, 2)));
    ASSERT_EQ(*lcExpr, *lcExpr->clone());

    argList = ArgumentList::make(&pool);
    argList->addArgument(ConstantExpression::make(&pool, 1));
    argList->addArgument(ConstantExpression::make(&pool, 5));
    PredicateExpression predExpr(
        "all",
        "n",
        FunctionCallExpression::make(&pool, "range", argList),
        RelationalExpression::makeGE(&pool,
                                 LabelExpression::make(&pool, "n"),
                                 ConstantExpression::make(&pool, 2)));
    ASSERT_EQ(predExpr, *predExpr.clone());

    argList = ArgumentList::make(&pool);
    argList->addArgument(ConstantExpression::make(&pool, 1));
    argList->addArgument(ConstantExpression::make(&pool, 5));
    ReduceExpression reduceExpr(
        "totalNum",
        ArithmeticExpression::makeMultiply(
            &pool, new ConstantExpression(2), new ConstantExpression(10)),
        "n",
        FunctionCallExpression::make(&pool, "range", argList),
        ArithmeticExpression::makeAdd(
            &pool,
            LabelExpression::make(&pool, "totalNum"),
            ArithmeticExpression::makeMultiply(&pool,
                                               LabelExpression::make(&pool, "n"),
                                               new ConstantExpression(2))));
    ASSERT_EQ(reduceExpr, *reduceExpr.clone());
}

TEST_F(ExpressionTest, PathBuild) {
    {
        auto expr = *(PathBuildExpression::make(&pool));
        expr.add(new VariablePropertyExpression("var1", "path_src"))
            .add(new VariablePropertyExpression("var1", "path_edge1"))
            .add(new VariablePropertyExpression("var1", "path_v1"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::PATH);
        Path expected;
        expected.src = Vertex("1", {});
        expected.steps.emplace_back(Step(Vertex("2", {}), 1, "edge", 0, {}));
        EXPECT_EQ(eval.getPath(), expected);
    }
    {
        auto expr = *(PathBuildExpression::make(&pool));
        expr.add(new VariablePropertyExpression("var1", "path_src"))
            .add(new VariablePropertyExpression("var1", "path_edge1"))
            .add(new VariablePropertyExpression("var1", "path_v1"))
            .add(new VariablePropertyExpression("var1", "path_edge2"))
            .add(new VariablePropertyExpression("var1", "path_v2"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::PATH);
        Path expected;
        expected.src = Vertex("1", {});
        expected.steps.emplace_back(Step(Vertex("2", {}), 1, "edge", 0, {}));
        expected.steps.emplace_back(Step(Vertex("3", {}), 1, "edge", 0, {}));
        EXPECT_EQ(eval.getPath(), expected);
    }
    {
        auto expr = *(PathBuildExpression::make(&pool));
        expr.add(new VariablePropertyExpression("var1", "path_src"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::PATH);
        Path expected;
        expected.src = Vertex("1", {});
        EXPECT_EQ(eval.getPath(), expected);
    }
    {
        auto expr = *(PathBuildExpression::make(&pool));
        expr.add(new VariablePropertyExpression("var1", "path_src"))
            .add(new VariablePropertyExpression("var1", "path_edge1"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::PATH);
    }

    auto varPropExpr = [](const std::string &name) {
        return new VariablePropertyExpression("var1", name);
    };

    {
        auto expr0 = PathBuildExpression::make(&pool);
        expr0->add(varPropExpr("path_src"));
        auto expr = PathBuildExpression::make(&pool);
        expr->add(expr0);
        expr->add(varPropExpr("path_edge1"));

        {
            // Test: Path + Edge
            auto result = Expression::eval(expr, gExpCtxt);
            EXPECT_EQ(result.type(), Value::Type::PATH);
            const auto &path = result.getPath();
            EXPECT_EQ(path.steps.size(), 1);
            EXPECT_EQ(path.steps.back().dst.vid, "2");
        }

        auto expr1 = PathBuildExpression::make(&pool);
        expr1->add(varPropExpr("path_v1"));
        expr->add(expr1);

        {
            // Test: Path + Edge + Path
            auto result = Expression::eval(expr, gExpCtxt);
            EXPECT_EQ(result.type(), Value::Type::PATH);
            const auto &path = result.getPath();
            EXPECT_EQ(path.steps.size(), 1);
            EXPECT_EQ(path.steps.back().dst.vid, "2");
        }

        expr->add(varPropExpr("path_edge2"));

        {
            // Test: Path + Edge + Path + Edge
            auto result = Expression::eval(expr, gExpCtxt);
            EXPECT_EQ(result.type(), Value::Type::PATH);
            const auto &path = result.getPath();
            EXPECT_EQ(path.steps.size(), 2);
            EXPECT_EQ(path.steps.back().dst.vid, "3");
        }

        auto pathExpr2 = PathBuildExpression::make(&pool);
        pathExpr2->add(varPropExpr("path_v2"));
        pathExpr2->add(varPropExpr("path_edge3"));

        auto pathExpr3 = PathBuildExpression::make(&pool);
        pathExpr3->add(std::move(expr));
        pathExpr3->add(std::move(pathExpr2));

        {
            // Test: Path + Path
            auto result = Expression::eval(pathExpr3, gExpCtxt);
            EXPECT_EQ(result.type(), Value::Type::PATH);
            const auto &path = result.getPath();
            EXPECT_EQ(path.steps.size(), 3);
            EXPECT_EQ(path.steps.back().dst.vid, "4");
        }
    }
}

TEST_F(ExpressionTest, PathBuildToString) {
    {
        auto expr = *(PathBuildExpression::make(&pool));
        expr.add(new VariablePropertyExpression("var1", "path_src"))
            .add(new VariablePropertyExpression("var1", "path_edge1"))
            .add(new VariablePropertyExpression("var1", "path_v1"));
        EXPECT_EQ(expr.toString(), "PathBuild[$var1.path_src,$var1.path_edge1,$var1.path_v1]");
    }
}

}  // namespace nebula

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);

    return RUN_ALL_TESTS();
}
