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
        auto ep = *EdgePropertyExpression::make(&pool, "e1", "int");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // t1.int
        auto ep = *TagPropertyExpression::make(&pool, "t1", "int");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // $-.int
        auto ep = *InputPropertyExpression::make(&pool, "int");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // $^.source.int
        auto ep = *SourcePropertyExpression::make(&pool, "source", "int");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // $$.dest.int
        auto ep = *DestPropertyExpression::make(&pool, "dest", "int");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // $var.float
        auto ep = *VariablePropertyExpression::make(&pool, "var", "float");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::FLOAT);
        EXPECT_EQ(eval, 1.1);
    }
}

TEST_F(ExpressionTest, EdgeTest) {
    {
        // EdgeName._src
        auto ep = *EdgeSrcIdExpression::make(&pool, "edge1");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // EdgeName._type
        auto ep = *EdgeTypeExpression::make(&pool, "edge1");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // EdgeName._rank
        auto ep = *EdgeRankExpression::make(&pool, "edge1");
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // EdgeName._dst
        auto ep = *EdgeDstIdExpression::make(&pool, "edge1");
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

        //     // test empty
        //     TEST_EXPR(empty, Value::kEmpty);
        //     TEST_EXPR(empty AND true, Value::kEmpty);
        //     TEST_EXPR(empty AND false, false);
        //     TEST_EXPR(true AND empty, Value::kEmpty);
        //     TEST_EXPR(false AND empty, false);
        //     TEST_EXPR(empty AND empty, Value::kEmpty);
        //     TEST_EXPR(empty AND null, Value::kNullValue);
        //     TEST_EXPR(null AND empty, Value::kNullValue);
        //     TEST_EXPR(empty AND true AND empty, Value::kEmpty);

        //     TEST_EXPR(empty OR true, true);
        //     TEST_EXPR(empty OR false, Value::kEmpty);
        //     TEST_EXPR(true OR empty, true);
        //     TEST_EXPR(false OR empty, Value::kEmpty);
        //     TEST_EXPR(empty OR empty, Value::kEmpty);
        //     TEST_EXPR(empty OR null, Value::kNullValue);
        //     TEST_EXPR(null OR empty, Value::kNullValue);
        //     TEST_EXPR(empty OR false OR empty, Value::kEmpty);

        //     TEST_EXPR(empty XOR true, Value::kEmpty);
        //     TEST_EXPR(empty XOR false, Value::kEmpty);
        //     TEST_EXPR(true XOR empty, Value::kEmpty);
        //     TEST_EXPR(false XOR empty, Value::kEmpty);
        //     TEST_EXPR(empty XOR empty, Value::kEmpty);
        //     TEST_EXPR(empty XOR null, Value::kNullValue);
        //     TEST_EXPR(null XOR empty, Value::kNullValue);
        //     TEST_EXPR(true XOR empty XOR false, Value::kEmpty);

        // TEST_EXPR(empty OR false AND true AND null XOR empty, Value::kEmpty);
        // TEST_EXPR(empty OR false AND true XOR empty OR true, true);
        // mark
        TEST_EXPR((empty OR false)AND true XOR empty XOR null AND 2 / 0, Value::kNullValue);
        // // empty OR false AND 2/0
        // TEST_EXPR(empty OR false AND true XOR empty XOR null AND 2 / 0, Value::kEmpty);
        // TEST_EXPR(empty AND true XOR empty XOR null AND 2 / 0, Value::kNullValue);
        // TEST_EXPR(empty OR false AND true XOR empty OR null AND 2 / 0, Value::kNullDivByZero);
        // TEST_EXPR(empty OR false AND empty XOR empty OR null, Value::kNullValue);
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
//                 VariableExpression::make(&pool, "var_int"));
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
//                     ConstantExpression::make(&pool, 0)));
//         auto eval = Expression::eval(&expr, gExpCtxt);
//         EXPECT_EQ(eval.type(), Value::Type::INT);
//         EXPECT_EQ(eval, 2);
//     }
// }

TEST_F(ExpressionTest, IsNull) {
    {
        auto expr =
            *UnaryExpression::makeIsNull(&pool, ConstantExpression::make(&pool, Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr = *UnaryExpression::makeIsNull(&pool, ConstantExpression::make(&pool, 1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        auto expr = *UnaryExpression::makeIsNull(&pool, ConstantExpression::make(&pool, 1.1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        auto expr = *UnaryExpression::makeIsNull(&pool, ConstantExpression::make(&pool, true));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        auto expr =
            *UnaryExpression::makeIsNull(&pool, ConstantExpression::make(&pool, Value::kEmpty));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
}

TEST_F(ExpressionTest, IsNotNull) {
    {
        auto expr = *UnaryExpression::makeIsNotNull(
            &pool, ConstantExpression::make(&pool, Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        auto expr = *UnaryExpression::makeIsNotNull(&pool, ConstantExpression::make(&pool, 1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr = *UnaryExpression::makeIsNotNull(&pool, ConstantExpression::make(&pool, 1.1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr = *UnaryExpression::makeIsNotNull(&pool, ConstantExpression::make(&pool, true));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr =
            *UnaryExpression::makeIsNotNull(&pool, ConstantExpression::make(&pool, Value::kEmpty));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
}

TEST_F(ExpressionTest, IsEmpty) {
    {
        auto expr = *UnaryExpression::makeIsEmpty(
            &pool, ConstantExpression::make(&pool, Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        auto expr = *UnaryExpression::makeIsEmpty(&pool, ConstantExpression::make(&pool, 1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        auto expr = *UnaryExpression::makeIsEmpty(&pool, ConstantExpression::make(&pool, 1.1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        auto expr = *UnaryExpression::makeIsEmpty(&pool, ConstantExpression::make(&pool, true));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        auto expr =
            *UnaryExpression::makeIsEmpty(&pool, ConstantExpression::make(&pool, Value::kEmpty));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
}

TEST_F(ExpressionTest, IsNotEmpty) {
    {
        auto expr = *UnaryExpression::makeIsNotEmpty(
            &pool, ConstantExpression::make(&pool, Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr = *UnaryExpression::makeIsNotEmpty(&pool, ConstantExpression::make(&pool, 1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr = *UnaryExpression::makeIsNotEmpty(&pool, ConstantExpression::make(&pool, 1.1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr = *UnaryExpression::makeIsNotEmpty(&pool, ConstantExpression::make(&pool, true));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr =
            *UnaryExpression::makeIsNotEmpty(&pool, ConstantExpression::make(&pool, Value::kEmpty));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
}

TEST_F(ExpressionTest, UnaryDECR) {
    {
        // --var_int
        auto expr = *UnaryExpression::makeDecr(&pool, VariableExpression::make(&pool, "var_int"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 0);
    }
}

TEST_F(ExpressionTest, VersionedVar) {
    {
        // versioned_var{0}
        auto expr = *VersionedVariableExpression::make(
            &pool, "versioned_var", ConstantExpression::make(&pool, 0));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // versioned_var{0}
        auto expr = *VersionedVariableExpression::make(
            &pool, "versioned_var", ConstantExpression::make(&pool, -1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 2);
    }
    {
        // versioned_var{0}
        auto expr = *VersionedVariableExpression::make(
            &pool, "versioned_var", ConstantExpression::make(&pool, 1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 8);
    }
    {
        // versioned_var{-cnt}
        auto expr = *VersionedVariableExpression::make(
            &pool,
            "versioned_var",
            UnaryExpression::makeNegate(&pool, VariableExpression::make(&pool, "cnt")));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 2);
    }
}

TEST_F(ExpressionTest, toStringTest) {
    {
        auto ep = *ConstantExpression::make(&pool, 1);
        EXPECT_EQ(ep.toString(), "1");
    }
    {
        auto ep = *ConstantExpression::make(&pool, 1.123);
        EXPECT_EQ(ep.toString(), "1.123");
    }
    // FIXME: double/float to string conversion
    // {
    //     auto ep = *ConstantExpression::make(&pool, 1.0);
    //     EXPECT_EQ(ep.toString(), "1.0");
    // }
    {
        auto ep = *ConstantExpression::make(&pool, true);
        EXPECT_EQ(ep.toString(), "true");
    }
    {
        auto ep = *ConstantExpression::make(&pool, List(std::vector<Value>{1, 2, 3, 4, 9, 0, -23}));
        EXPECT_EQ(ep.toString(), "[1,2,3,4,9,0,-23]");
    }
    {
        auto ep = *ConstantExpression::make(&pool, Map({{"hello", "world"}, {"name", "zhang"}}));
        EXPECT_EQ(ep.toString(), "{name:\"zhang\",hello:\"world\"}");
    }
    {
        auto ep = *ConstantExpression::make(&pool, Set({1, 2.3, "hello", true}));
        EXPECT_EQ(ep.toString(), "{\"hello\",2.3,true,1}");
    }
    {
        auto ep = *ConstantExpression::make(&pool, Date(1234));
        EXPECT_EQ(ep.toString(), "-32765-05-19");
    }
    {
        auto ep =
            *ConstantExpression::make(&pool, Edge("100", "102", 2, "like", 3, {{"likeness", 95}}));
        EXPECT_EQ(ep.toString(), "(\"100\")-[like(2)]->(\"102\")@3 likeness:95");
    }
    {
        auto ep =
            *ConstantExpression::make(&pool, Vertex("100", {Tag("player", {{"name", "jame"}})}));
        EXPECT_EQ(ep.toString(), "(\"100\") Tag: player, name:\"jame\"");
    }
    {
        auto ep = *TypeCastingExpression::make(
            &pool, Value::Type::FLOAT, ConstantExpression::make(&pool, 2));
        EXPECT_EQ(ep.toString(), "(FLOAT)2");
    }
    {
        auto plusEx = *UnaryExpression::makePlus(&pool, ConstantExpression::make(&pool, 2));
        EXPECT_EQ(plusEx.toString(), "+(2)");

        auto negEx = *UnaryExpression::makeNegate(&pool, ConstantExpression::make(&pool, 2));
        EXPECT_EQ(negEx.toString(), "-(2)");

        auto incrEx = *UnaryExpression::makeIncr(&pool, ConstantExpression::make(&pool, 2));
        EXPECT_EQ(incrEx.toString(), "++(2)");

        auto decrEx = *UnaryExpression::makeDecr(&pool, ConstantExpression::make(&pool, 2));
        EXPECT_EQ(decrEx.toString(), "--(2)");

        auto notEx = *UnaryExpression::makeNot(&pool, ConstantExpression::make(&pool, 2));
        EXPECT_EQ(notEx.toString(), "!(2)");

        auto isNullEx = *UnaryExpression::makeIsNull(&pool, ConstantExpression::make(&pool, 2));
        EXPECT_EQ(isNullEx.toString(), "2 IS NULL");

        auto isNotNullEx = *UnaryExpression::makeIsNotNull(
            &pool, ConstantExpression::make(&pool, Value::kNullValue));
        EXPECT_EQ(isNotNullEx.toString(), "NULL IS NOT NULL");

        auto isEmptyEx = *UnaryExpression::makeIsEmpty(&pool, ConstantExpression::make(&pool, 2));
        EXPECT_EQ(isEmptyEx.toString(), "2 IS EMPTY");

        auto isNotEmptyEx =
            *UnaryExpression::makeIsNotEmpty(&pool, ConstantExpression::make(&pool, Value::kEmpty));
        EXPECT_EQ(isNotEmptyEx.toString(), "__EMPTY__ IS NOT EMPTY");
    }
    {
        auto var = *VariableExpression::make(&pool, "name");
        EXPECT_EQ(var.toString(), "$name");

        auto versionVar =
            *VersionedVariableExpression::make(&pool, "name", ConstantExpression::make(&pool, 1));
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
        auto ep = *DestPropertyExpression::make(&pool, "like", "likeness");
        EXPECT_EQ(ep.toString(), "$$.like.likeness");
    }
    {
        auto ep = *EdgePropertyExpression::make(&pool, "like", "likeness");
        EXPECT_EQ(ep.toString(), "like.likeness");
    }
    {
        auto ep = *InputPropertyExpression::make(&pool, "name");
        EXPECT_EQ(ep.toString(), "$-.name");
    }
    {
        auto ep = *VariablePropertyExpression::make(&pool, "player", "name");
        EXPECT_EQ(ep.toString(), "$player.name");
    }
    {
        auto ep = *SourcePropertyExpression::make(&pool, "player", "age");
        EXPECT_EQ(ep.toString(), "$^.player.age");
    }
    {
        auto ep = *DestPropertyExpression::make(&pool, "player", "age");
        EXPECT_EQ(ep.toString(), "$$.player.age");
    }
    {
        auto ep = *EdgeSrcIdExpression::make(&pool, "like");
        EXPECT_EQ(ep.toString(), "like._src");
    }
    {
        auto ep = *EdgeTypeExpression::make(&pool, "like");
        EXPECT_EQ(ep.toString(), "like._type");
    }
    {
        auto ep = *EdgeRankExpression::make(&pool, "like");
        EXPECT_EQ(ep.toString(), "like._rank");
    }
    {
        auto ep = *EdgeDstIdExpression::make(&pool, "like");
        EXPECT_EQ(ep.toString(), "like._dst");
    }
}

TEST_F(ExpressionTest, InList) {
    {
        auto *elist = ExpressionList::make(&pool);
        (*elist)
            .add(ConstantExpression::make(&pool, 12345))
            .add(ConstantExpression::make(&pool, "Hello"))
            .add(ConstantExpression::make(&pool, true));
        auto listExpr = ListExpression::make(&pool, elist);
        auto expr =
            *RelationalExpression::makeIn(&pool, ConstantExpression::make(&pool, 12345), listExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(true, value);
    }
    {
        auto *elist = ExpressionList::make(&pool);
        (*elist)
            .add(ConstantExpression::make(&pool, 12345))
            .add(ConstantExpression::make(&pool, "Hello"))
            .add(ConstantExpression::make(&pool, true));
        auto listExpr = ListExpression::make(&pool, elist);
        auto expr =
            *RelationalExpression::makeIn(&pool, ConstantExpression::make(&pool, false), listExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value);
    }
}

TEST_F(ExpressionTest, InSet) {
    {
        auto *elist = ExpressionList::make(&pool);
        (*elist)
            .add(ConstantExpression::make(&pool, 12345))
            .add(ConstantExpression::make(&pool, "Hello"))
            .add(ConstantExpression::make(&pool, true));
        auto setExpr = SetExpression::make(&pool, elist);
        auto expr =
            *RelationalExpression::makeIn(&pool, ConstantExpression::make(&pool, 12345), setExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(true, value);
    }
    {
        auto *elist = ExpressionList::make(&pool);
        (*elist)
            .add(ConstantExpression::make(&pool, 12345))
            .add(ConstantExpression::make(&pool, "Hello"))
            .add(ConstantExpression::make(&pool, true));
        auto setExpr = ListExpression::make(&pool, elist);
        auto expr =
            *RelationalExpression::makeIn(&pool, ConstantExpression::make(&pool, false), setExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value);
    }
}

TEST_F(ExpressionTest, InMap) {
    {
        auto *items = MapItemList::make(&pool);
        (*items)
            .add("key1", ConstantExpression::make(&pool, 12345))
            .add("key2", ConstantExpression::make(&pool, 12345))
            .add("key3", ConstantExpression::make(&pool, "Hello"))
            .add("key4", ConstantExpression::make(&pool, true));
        auto mapExpr = MapExpression::make(&pool, items);
        auto expr =
            *RelationalExpression::makeIn(&pool, ConstantExpression::make(&pool, "key1"), mapExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(true, value);
    }
    {
        auto *items = MapItemList::make(&pool);
        (*items)
            .add("key1", ConstantExpression::make(&pool, 12345))
            .add("key2", ConstantExpression::make(&pool, 12345))
            .add("key3", ConstantExpression::make(&pool, "Hello"))
            .add("key4", ConstantExpression::make(&pool, true));
        auto mapExpr = MapExpression::make(&pool, items);
        auto expr =
            *RelationalExpression::makeIn(&pool, ConstantExpression::make(&pool, "key5"), mapExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value);
    }
    {
        auto *items = MapItemList::make(&pool);
        (*items)
            .add("key1", ConstantExpression::make(&pool, 12345))
            .add("key2", ConstantExpression::make(&pool, 12345))
            .add("key3", ConstantExpression::make(&pool, "Hello"))
            .add("key4", ConstantExpression::make(&pool, true));
        auto mapExpr = MapExpression::make(&pool, items);
        auto expr =
            *RelationalExpression::makeIn(&pool, ConstantExpression::make(&pool, 12345), mapExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value);
    }
}

TEST_F(ExpressionTest, NotInList) {
    {
        auto *elist = ExpressionList::make(&pool);
        (*elist)
            .add(ConstantExpression::make(&pool, 12345))
            .add(ConstantExpression::make(&pool, "Hello"))
            .add(ConstantExpression::make(&pool, true));
        auto listExpr = ListExpression::make(&pool, elist);
        auto expr = *RelationalExpression::makeNotIn(
            &pool, ConstantExpression::make(&pool, 12345), listExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value);
    }
    {
        auto *elist = ExpressionList::make(&pool);
        (*elist)
            .add(ConstantExpression::make(&pool, 12345))
            .add(ConstantExpression::make(&pool, "Hello"))
            .add(ConstantExpression::make(&pool, true));
        auto listExpr = ListExpression::make(&pool, elist);
        auto expr = *RelationalExpression::makeNotIn(
            &pool, ConstantExpression::make(&pool, false), listExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(true, value);
    }
}

TEST_F(ExpressionTest, NotInSet) {
    {
        auto *elist = ExpressionList::make(&pool);
        (*elist)
            .add(ConstantExpression::make(&pool, 12345))
            .add(ConstantExpression::make(&pool, "Hello"))
            .add(ConstantExpression::make(&pool, true));
        auto setExpr = SetExpression::make(&pool, elist);
        auto expr = *RelationalExpression::makeNotIn(
            &pool, ConstantExpression::make(&pool, 12345), setExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value);
    }
    {
        auto *elist = ExpressionList::make(&pool);
        (*elist)
            .add(ConstantExpression::make(&pool, 12345))
            .add(ConstantExpression::make(&pool, "Hello"))
            .add(ConstantExpression::make(&pool, true));
        auto setExpr = ListExpression::make(&pool, elist);
        auto expr = *RelationalExpression::makeNotIn(
            &pool, ConstantExpression::make(&pool, false), setExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(true, value);
    }
}

TEST_F(ExpressionTest, NotInMap) {
    {
        auto *items = MapItemList::make(&pool);
        (*items)
            .add("key1", ConstantExpression::make(&pool, 12345))
            .add("key2", ConstantExpression::make(&pool, 12345))
            .add("key3", ConstantExpression::make(&pool, "Hello"))
            .add("key4", ConstantExpression::make(&pool, true));
        auto mapExpr = MapExpression::make(&pool, items);
        auto expr = *RelationalExpression::makeNotIn(
            &pool, ConstantExpression::make(&pool, "key1"), mapExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value);
    }
    {
        auto *items = MapItemList::make(&pool);
        (*items)
            .add("key1", ConstantExpression::make(&pool, 12345))
            .add("key2", ConstantExpression::make(&pool, 12345))
            .add("key3", ConstantExpression::make(&pool, "Hello"))
            .add("key4", ConstantExpression::make(&pool, true));
        auto mapExpr = MapExpression::make(&pool, items);
        auto expr = *RelationalExpression::makeNotIn(
            &pool, ConstantExpression::make(&pool, "key5"), mapExpr);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(true, value);
    }
    {
        auto *items = MapItemList::make(&pool);
        (*items)
            .add("key1", ConstantExpression::make(&pool, 12345))
            .add("key2", ConstantExpression::make(&pool, 12345))
            .add("key3", ConstantExpression::make(&pool, "Hello"))
            .add("key4", ConstantExpression::make(&pool, true));
        auto mapExpr = MapExpression::make(&pool, items);
        auto expr = *RelationalExpression::makeNotIn(
            &pool, ConstantExpression::make(&pool, 12345), mapExpr);
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
        auto *dataset = ConstantExpression::make(&pool, ds);
        auto *rowIndex = ConstantExpression::make(&pool, 0);
        auto expr = *SubscriptExpression::make(&pool, dataset, rowIndex);
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
        auto *dataset = ConstantExpression::make(&pool, ds);
        auto *rowIndex = ConstantExpression::make(&pool, 0);
        auto *colIndex = ConstantExpression::make(&pool, 1);
        auto expr = *SubscriptExpression::make(
            &pool, SubscriptExpression::make(&pool, dataset, rowIndex), colIndex);
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
        auto *dataset = ConstantExpression::make(&pool, ds);
        auto *rowIndex = ConstantExpression::make(&pool, -1);
        auto expr = *SubscriptExpression::make(&pool, dataset, rowIndex);
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
        auto *dataset = ConstantExpression::make(&pool, ds);
        auto *rowIndex = ConstantExpression::make(&pool, 0);
        auto *colIndex = ConstantExpression::make(&pool, 5);
        auto expr = *SubscriptExpression::make(
            &pool, SubscriptExpression::make(&pool, dataset, rowIndex), colIndex);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBadNull());
    }
}

TEST_F(ExpressionTest, ListSubscript) {
    // [1,2,3,4][0]
    {
        auto *items = ExpressionList::make(&pool);
        (*items)
            .add(ConstantExpression::make(&pool, 1))
            .add(ConstantExpression::make(&pool, 2))
            .add(ConstantExpression::make(&pool, 3))
            .add(ConstantExpression::make(&pool, 4));
        auto *list = ListExpression::make(&pool, items);
        auto *index = ConstantExpression::make(&pool, 0);
        auto expr = *SubscriptExpression::make(&pool, list, index);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(1, value.getInt());
    }
    // [1,2,3,4][3]
    {
        auto *items = ExpressionList::make(&pool);
        (*items)
            .add(ConstantExpression::make(&pool, 1))
            .add(ConstantExpression::make(&pool, 2))
            .add(ConstantExpression::make(&pool, 3))
            .add(ConstantExpression::make(&pool, 4));
        auto *list = ListExpression::make(&pool, items);
        auto *index = ConstantExpression::make(&pool, 3);
        auto expr = *SubscriptExpression::make(&pool, list, index);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(4, value.getInt());
    }
    // [1,2,3,4][4]
    {
        auto *items = ExpressionList::make(&pool);
        (*items)
            .add(ConstantExpression::make(&pool, 1))
            .add(ConstantExpression::make(&pool, 2))
            .add(ConstantExpression::make(&pool, 3))
            .add(ConstantExpression::make(&pool, 4));
        auto *list = ListExpression::make(&pool, items);
        auto *index = ConstantExpression::make(&pool, 4);
        auto expr = *SubscriptExpression::make(&pool, list, index);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBadNull());
    }
    // [1,2,3,4][-1]
    {
        auto *items = ExpressionList::make(&pool);
        (*items)
            .add(ConstantExpression::make(&pool, 1))
            .add(ConstantExpression::make(&pool, 2))
            .add(ConstantExpression::make(&pool, 3))
            .add(ConstantExpression::make(&pool, 4));
        auto *list = ListExpression::make(&pool, items);
        auto *index = ConstantExpression::make(&pool, -1);
        auto expr = *SubscriptExpression::make(&pool, list, index);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(4, value.getInt());
    }
    // [1,2,3,4][-4]
    {
        auto *items = ExpressionList::make(&pool);
        (*items)
            .add(ConstantExpression::make(&pool, 1))
            .add(ConstantExpression::make(&pool, 2))
            .add(ConstantExpression::make(&pool, 3))
            .add(ConstantExpression::make(&pool, 4));
        auto *list = ListExpression::make(&pool, items);
        auto *index = ConstantExpression::make(&pool, -4);
        auto expr = *SubscriptExpression::make(&pool, list, index);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(1, value.getInt());
    }
    // [1,2,3,4][-5]
    {
        auto *items = ExpressionList::make(&pool);
        (*items)
            .add(ConstantExpression::make(&pool, 1))
            .add(ConstantExpression::make(&pool, 2))
            .add(ConstantExpression::make(&pool, 3))
            .add(ConstantExpression::make(&pool, 4));
        auto *list = ListExpression::make(&pool, items);
        auto *index = ConstantExpression::make(&pool, -5);
        auto expr = *SubscriptExpression::make(&pool, list, index);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBadNull());
    }
    // [1,2,3,4]["0"]
    {
        auto *items = ExpressionList::make(&pool);
        (*items)
            .add(ConstantExpression::make(&pool, 1))
            .add(ConstantExpression::make(&pool, 2))
            .add(ConstantExpression::make(&pool, 3))
            .add(ConstantExpression::make(&pool, 4));
        auto *list = ListExpression::make(&pool, items);
        auto *index = ConstantExpression::make(&pool, "0");
        auto expr = *SubscriptExpression::make(&pool, list, index);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBadNull());
    }
    // 1[0]
    {
        auto *integer = ConstantExpression::make(&pool, 1);
        auto *index = ConstantExpression::make(&pool, 0);
        auto expr = *SubscriptExpression::make(&pool, integer, index);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBadNull());
    }
}

TEST_F(ExpressionTest, ListSubscriptRange) {
    auto *items = ExpressionList::make(&pool);
    (*items)
        .add(ConstantExpression::make(&pool, 0))
        .add(ConstantExpression::make(&pool, 1))
        .add(ConstantExpression::make(&pool, 2))
        .add(ConstantExpression::make(&pool, 3))
        .add(ConstantExpression::make(&pool, 4))
        .add(ConstantExpression::make(&pool, 5));
    auto list = ListExpression::make(&pool, items);
    // [0,1,2,3,4,5][0..] => [0,1,2,3,4,5]
    {
        auto *lo = ConstantExpression::make(&pool, 0);
        auto expr = *SubscriptRangeExpression::make(&pool, list->clone(), lo, nullptr);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List({0, 1, 2, 3, 4, 5}), value.getList());
    }
    // [0,1,2,3,4,5][0..0] => []
    {
        auto *lo = ConstantExpression::make(&pool, 0);
        auto *hi = ConstantExpression::make(&pool, 0);
        auto expr = *SubscriptRangeExpression::make(&pool, list->clone(), lo, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List(), value.getList());
    }
    // [0,1,2,3,4,5][0..10] => [0,1,2,3,4,5]
    {
        auto *lo = ConstantExpression::make(&pool, 0);
        auto *hi = ConstantExpression::make(&pool, 10);
        auto expr = *SubscriptRangeExpression::make(&pool, list->clone(), lo, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List({0, 1, 2, 3, 4, 5}), value.getList());
    }
    // [0,1,2,3,4,5][3..2] => []
    {
        auto *lo = ConstantExpression::make(&pool, 3);
        auto *hi = ConstantExpression::make(&pool, 2);
        auto expr = *SubscriptRangeExpression::make(&pool, list->clone(), lo, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List(), value.getList());
    }
    // [0,1,2,3,4,5][..-1] => [0,1,2,3,4]
    {
        auto *hi = ConstantExpression::make(&pool, -1);
        auto expr = *SubscriptRangeExpression::make(&pool, list->clone(), nullptr, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List({0, 1, 2, 3, 4}), value.getList());
    }
    // [0,1,2,3,4,5][-1..-1] => []
    {
        auto *lo = ConstantExpression::make(&pool, -1);
        auto *hi = ConstantExpression::make(&pool, -1);
        auto expr = *SubscriptRangeExpression::make(&pool, list->clone(), lo, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List(), value.getList());
    }
    // [0,1,2,3,4,5][-10..-1] => [0,1,2,3,4]
    {
        auto *lo = ConstantExpression::make(&pool, -10);
        auto *hi = ConstantExpression::make(&pool, -1);
        auto expr = *SubscriptRangeExpression::make(&pool, list->clone(), lo, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List({0, 1, 2, 3, 4}), value.getList());
    }
    // [0,1,2,3,4,5][-2..-3] => []
    {
        auto *lo = ConstantExpression::make(&pool, -2);
        auto *hi = ConstantExpression::make(&pool, -3);
        auto expr = *SubscriptRangeExpression::make(&pool, list->clone(), lo, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List(), value.getList());
    }
    // [0,1,2,3,4,5][2..-3] => [2]
    {
        auto *lo = ConstantExpression::make(&pool, 2);
        auto *hi = ConstantExpression::make(&pool, -3);
        auto expr = *SubscriptRangeExpression::make(&pool, list->clone(), lo, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List({2}), value.getList());
    }
    // [0,1,2,3,4,5][-2..3] => []
    {
        auto *lo = ConstantExpression::make(&pool, -2);
        auto *hi = ConstantExpression::make(&pool, 3);
        auto expr = *SubscriptRangeExpression::make(&pool, list->clone(), lo, hi);
        auto value = Expression::eval(&expr, gExpCtxt);
        EXPECT_TRUE(value.isList());
        EXPECT_EQ(List(), value.getList());
    }
}

TEST_F(ExpressionTest, MapSubscript) {
    // {"key1":1,"key2":2, "key3":3}["key1"]
    {
        auto *items = MapItemList::make(&pool);
        (*items)
            .add("key1", ConstantExpression::make(&pool, 1))
            .add("key2", ConstantExpression::make(&pool, 2))
            .add("key3", ConstantExpression::make(&pool, 3));
        auto *map = MapExpression::make(&pool, items);
        auto *key = ConstantExpression::make(&pool, "key1");
        auto expr = *SubscriptExpression::make(&pool, map, key);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(1, value.getInt());
    }
    // {"key1":1,"key2":2, "key3":3}["key4"]
    {
        auto *items = MapItemList::make(&pool);
        (*items)
            .add("key1", ConstantExpression::make(&pool, 1))
            .add("key2", ConstantExpression::make(&pool, 2))
            .add("key3", ConstantExpression::make(&pool, 3));
        auto *map = MapExpression::make(&pool, items);
        auto *key = ConstantExpression::make(&pool, "key4");
        auto expr = *SubscriptExpression::make(&pool, map, key);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isNull());
        ASSERT_FALSE(value.isBadNull());
    }
    // {"key1":1,"key2":2, "key3":3}[0]
    {
        auto *items = MapItemList::make(&pool);
        (*items)
            .add("key1", ConstantExpression::make(&pool, 1))
            .add("key2", ConstantExpression::make(&pool, 2))
            .add("key3", ConstantExpression::make(&pool, 3));
        auto *map = MapExpression::make(&pool, items);
        auto *key = ConstantExpression::make(&pool, 0);
        auto expr = *SubscriptExpression::make(&pool, map, key);
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
        auto *left = ConstantExpression::make(&pool, Value(vertex));
        auto *right = ConstantExpression::make(&pool, "Mull");
        auto expr = *SubscriptExpression::make(&pool, left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("Kintyre", value.getStr());
    }
    {
        auto *left = ConstantExpression::make(&pool, Value(vertex));
        auto *right = LabelExpression::make(&pool, "Bip");
        auto expr = *SubscriptExpression::make(&pool, left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("Bop", value.getStr());
    }
    {
        auto *left = ConstantExpression::make(&pool, Value(vertex));
        auto *right = LabelExpression::make(&pool, "Venus");
        auto expr = *SubscriptExpression::make(&pool, left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("Mars", value.getStr());
    }
    {
        auto *left = ConstantExpression::make(&pool, Value(vertex));
        auto *right = LabelExpression::make(&pool, "_vid");
        auto expr = *SubscriptExpression::make(&pool, left, right);
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
        auto *left = ConstantExpression::make(&pool, Value(edge));
        auto *right = ConstantExpression::make(&pool, "Rocky");
        auto expr = *SubscriptExpression::make(&pool, left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("Raccoon", value.getStr());
    }
    {
        auto *left = ConstantExpression::make(&pool, Value(edge));
        auto *right = ConstantExpression::make(&pool, kType);
        auto expr = *SubscriptExpression::make(&pool, left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("type", value.getStr());
    }
    {
        auto *left = ConstantExpression::make(&pool, Value(edge));
        auto *right = ConstantExpression::make(&pool, kSrc);
        auto expr = *SubscriptExpression::make(&pool, left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("src", value.getStr());
    }
    {
        auto *left = ConstantExpression::make(&pool, Value(edge));
        auto *right = ConstantExpression::make(&pool, kDst);
        auto expr = *SubscriptExpression::make(&pool, left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("dst", value.getStr());
    }
    {
        auto *left = ConstantExpression::make(&pool, Value(edge));
        auto *right = ConstantExpression::make(&pool, kRank);
        auto expr = *SubscriptExpression::make(&pool, left, right);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(123, value.getInt());
    }
}

TEST_F(ExpressionTest, TypeCastTest) {
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::INT, ConstantExpression::make(&pool, 1));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::INT, ConstantExpression::make(&pool, 1.23));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::INT, ConstantExpression::make(&pool, "1.23"));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::INT, ConstantExpression::make(&pool, "123"));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 123);
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::INT, ConstantExpression::make(&pool, ".123"));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 0);
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::INT, ConstantExpression::make(&pool, ".123ab"));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::INT, ConstantExpression::make(&pool, "abc123"));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::INT, ConstantExpression::make(&pool, "123abc"));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::INT, ConstantExpression::make(&pool, true));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }

    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::FLOAT, ConstantExpression::make(&pool, 1.23));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::FLOAT);
        EXPECT_EQ(eval, 1.23);
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::FLOAT, ConstantExpression::make(&pool, 2));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::FLOAT);
        EXPECT_EQ(eval, 2.0);
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::FLOAT, ConstantExpression::make(&pool, "1.23"));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::FLOAT);
        EXPECT_EQ(eval, 1.23);
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::BOOL, ConstantExpression::make(&pool, 2));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::BOOL, ConstantExpression::make(&pool, 0));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::STRING, ConstantExpression::make(&pool, true));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::STRING);
        EXPECT_EQ(eval, "true");
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::STRING, ConstantExpression::make(&pool, false));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::STRING);
        EXPECT_EQ(eval, "false");
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::STRING, ConstantExpression::make(&pool, 12345));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::STRING);
        EXPECT_EQ(eval, "12345");
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::STRING, ConstantExpression::make(&pool, 34.23));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::STRING);
        EXPECT_EQ(eval, "34.23");
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::STRING, ConstantExpression::make(&pool, .23));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::STRING);
        EXPECT_EQ(eval, "0.23");
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::SET, ConstantExpression::make(&pool, 23));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
    {
        auto typeCast = *TypeCastingExpression::make(
            &pool, Value::Type::INT, ConstantExpression::make(&pool, Set()));
        auto eval = Expression::eval(&typeCast, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
    }
}

TEST_F(ExpressionTest, RelationRegexMatch) {
    {
        auto expr = *RelationalExpression::makeREG(
            &pool,
            ConstantExpression::make(&pool, "abcd\xA3g1234efgh\x49ijkl"),
            ConstantExpression::make(&pool, "\\w{4}\xA3g12\\d*e\\w+\x49\\w+"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr = *RelationalExpression::makeREG(&pool,
                                                   ConstantExpression::make(&pool, "Tony Parker"),
                                                   ConstantExpression::make(&pool, "T.*er"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr =
            *RelationalExpression::makeREG(&pool,
                                           ConstantExpression::make(&pool, "010-12345"),
                                           ConstantExpression::make(&pool, "\\d{3}\\-\\d{3,8}"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr = *RelationalExpression::makeREG(
            &pool,
            ConstantExpression::make(&pool, "test_space_128"),
            ConstantExpression::make(&pool, "[a-zA-Z_][0-9a-zA-Z_]{0,19}"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr = *RelationalExpression::makeREG(
            &pool,
            ConstantExpression::make(&pool, "2001-09-01 08:00:00"),
            ConstantExpression::make(&pool, "\\d+\\-0\\d?\\-\\d+\\s\\d+:00:\\d+"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr = *RelationalExpression::makeREG(
            &pool,
            ConstantExpression::make(&pool, "jack138tom发."),
            ConstantExpression::make(&pool, "j\\w*\\d+\\w+\u53d1\\."));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr = *RelationalExpression::makeREG(
            &pool,
            ConstantExpression::make(&pool, "jack138tom\u53d1.34数数数"),
            ConstantExpression::make(&pool, "j\\w*\\d+\\w+发\\.34[\u4e00-\u9fa5]+"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr = *RelationalExpression::makeREG(&pool,
                                                   ConstantExpression::make(&pool, "a good person"),
                                                   ConstantExpression::make(&pool, "a\\s\\w+"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        auto expr =
            *RelationalExpression::makeREG(&pool,
                                           ConstantExpression::make(&pool, "Tony Parker"),
                                           ConstantExpression::make(&pool, "T\\w+\\s?\\P\\d+"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        auto expr =
            *RelationalExpression::makeREG(&pool,
                                           ConstantExpression::make(&pool, "010-12345"),
                                           ConstantExpression::make(&pool, "\\d?\\-\\d{3,8}"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        auto expr = *RelationalExpression::makeREG(
            &pool,
            ConstantExpression::make(&pool, "test_space_128牛"),
            ConstantExpression::make(&pool, "[a-zA-Z_][0-9a-zA-Z_]{0,19}"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        auto expr =
            *RelationalExpression::makeREG(&pool,
                                           ConstantExpression::make(&pool, "2001-09-01 08:00:00"),
                                           ConstantExpression::make(&pool, "\\d+\\s\\d+:00:\\d+"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        auto expr =
            *RelationalExpression::makeREG(&pool,
                                           ConstantExpression::make(&pool, Value::kNullBadData),
                                           ConstantExpression::make(&pool, Value::kNullBadType));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        auto expr =
            *RelationalExpression::makeREG(&pool,
                                           ConstantExpression::make(&pool, Value::kNullValue),
                                           ConstantExpression::make(&pool, 3));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        auto expr = *RelationalExpression::makeREG(
            &pool, ConstantExpression::make(&pool, 3), ConstantExpression::make(&pool, true));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        auto expr =
            *RelationalExpression::makeREG(&pool,
                                           ConstantExpression::make(&pool, "abc"),
                                           ConstantExpression::make(&pool, Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
}

TEST_F(ExpressionTest, RelationContains) {
    {
        // "abc" contains "a"
        auto expr = *RelationalExpression::makeContains(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "a"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" contains "bc"
        auto expr = *RelationalExpression::makeContains(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "bc"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" contains "d"
        auto expr = *RelationalExpression::makeContains(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "d"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" contains 1
        auto expr = *RelationalExpression::makeContains(
            &pool, ConstantExpression::make(&pool, "abc1"), ConstantExpression::make(&pool, 1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        // 1234 contains 1
        auto expr = *RelationalExpression::makeContains(
            &pool, ConstantExpression::make(&pool, 1234), ConstantExpression::make(&pool, 1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
}

TEST_F(ExpressionTest, RelationStartsWith) {
    {
        // "abc" starts with "a"
        auto expr = *RelationalExpression::makeStartsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "a"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" starts with "ab"
        auto expr = *RelationalExpression::makeStartsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "ab"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "1234"" starts with "12"
        auto expr = *RelationalExpression::makeStartsWith(
            &pool, ConstantExpression::make(&pool, "1234"), ConstantExpression::make(&pool, "12"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "1234"" starts with "34"
        auto expr = *RelationalExpression::makeStartsWith(
            &pool, ConstantExpression::make(&pool, "1234"), ConstantExpression::make(&pool, "34"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" starts with "bc"
        auto expr = *RelationalExpression::makeStartsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "bc"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" starts with "ac"
        auto expr = *RelationalExpression::makeStartsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "ac"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" starts with "AB"
        auto expr = *RelationalExpression::makeStartsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "AB"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" starts with 1
        auto expr = *RelationalExpression::makeStartsWith(
            &pool, ConstantExpression::make(&pool, "abc1"), ConstantExpression::make(&pool, 1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        // 1234 starts with 1
        auto expr = *RelationalExpression::makeStartsWith(
            &pool, ConstantExpression::make(&pool, 1234), ConstantExpression::make(&pool, 1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        auto expr = *RelationalExpression::makeStartsWith(
            &pool,
            ConstantExpression::make(&pool, 1234),
            ConstantExpression::make(&pool, Value::kNullBadData));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        auto expr = *RelationalExpression::makeStartsWith(
            &pool, ConstantExpression::make(&pool, 1234), ConstantExpression::make(&pool, "1234"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        auto expr = *RelationalExpression::makeStartsWith(
            &pool,
            ConstantExpression::make(&pool, Value::kNullValue),
            ConstantExpression::make(&pool, "NULL"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
    {
        auto expr = *RelationalExpression::makeStartsWith(
            &pool,
            ConstantExpression::make(&pool, "Null"),
            ConstantExpression::make(&pool, Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
}

TEST_F(ExpressionTest, RelationNotStartsWith) {
    {
        // "abc" not starts with "a"
        auto expr = *RelationalExpression::makeNotStartsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "a"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" not starts with "ab"
        auto expr = *RelationalExpression::makeNotStartsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "ab"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "1234"" not starts with "12"
        auto expr = *RelationalExpression::makeNotStartsWith(
            &pool, ConstantExpression::make(&pool, "1234"), ConstantExpression::make(&pool, "12"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "1234"" not starts with "34"
        auto expr = *RelationalExpression::makeNotStartsWith(
            &pool, ConstantExpression::make(&pool, "1234"), ConstantExpression::make(&pool, "34"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" not starts with "bc"
        auto expr = *RelationalExpression::makeNotStartsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "bc"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" not starts with "ac"
        auto expr = *RelationalExpression::makeNotStartsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "ac"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" not starts with "AB"
        auto expr = *RelationalExpression::makeNotStartsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "AB"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" not starts with 1
        auto expr = *RelationalExpression::makeNotStartsWith(
            &pool, ConstantExpression::make(&pool, "abc1"), ConstantExpression::make(&pool, 1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        // 1234 not starts with 1
        auto expr = *RelationalExpression::makeNotStartsWith(
            &pool, ConstantExpression::make(&pool, 1234), ConstantExpression::make(&pool, 1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        auto expr = *RelationalExpression::makeNotStartsWith(
            &pool,
            ConstantExpression::make(&pool, 1234),
            ConstantExpression::make(&pool, Value::kNullBadData));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        auto expr = *RelationalExpression::makeNotStartsWith(
            &pool, ConstantExpression::make(&pool, 1234), ConstantExpression::make(&pool, "1234"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        auto expr = *RelationalExpression::makeNotStartsWith(
            &pool,
            ConstantExpression::make(&pool, Value::kNullValue),
            ConstantExpression::make(&pool, "NULL"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
    {
        auto expr = *RelationalExpression::makeNotStartsWith(
            &pool,
            ConstantExpression::make(&pool, "Null"),
            ConstantExpression::make(&pool, Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
}

TEST_F(ExpressionTest, RelationEndsWith) {
    {
        // "abc" ends with "a"
        auto expr = *RelationalExpression::makeEndsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "a"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" ends with "ab"
        auto expr = *RelationalExpression::makeEndsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "ab"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "1234"" ends with "12"
        auto expr = *RelationalExpression::makeEndsWith(
            &pool, ConstantExpression::make(&pool, "1234"), ConstantExpression::make(&pool, "12"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "1234"" ends with "34"
        auto expr = *RelationalExpression::makeEndsWith(
            &pool, ConstantExpression::make(&pool, "1234"), ConstantExpression::make(&pool, "34"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" ends with "bc"
        auto expr = *RelationalExpression::makeEndsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "bc"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" ends with "ac"
        auto expr = *RelationalExpression::makeEndsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "ac"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" ends with "AB"
        auto expr = *RelationalExpression::makeEndsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "AB"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" ends with "BC"
        auto expr = *RelationalExpression::makeEndsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "BC"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" ends with 1
        auto expr = *RelationalExpression::makeEndsWith(
            &pool, ConstantExpression::make(&pool, "abc1"), ConstantExpression::make(&pool, 1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        // 1234 ends with 1
        auto expr = *RelationalExpression::makeEndsWith(
            &pool, ConstantExpression::make(&pool, 1234), ConstantExpression::make(&pool, 1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        // "steve jobs" ends with "jobs"
        auto expr =
            *RelationalExpression::makeEndsWith(&pool,
                                                ConstantExpression::make(&pool, "steve jobs"),
                                                ConstantExpression::make(&pool, "jobs"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        auto expr = *RelationalExpression::makeEndsWith(
            &pool,
            ConstantExpression::make(&pool, 1234),
            ConstantExpression::make(&pool, Value::kNullBadData));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        auto expr = *RelationalExpression::makeEndsWith(
            &pool, ConstantExpression::make(&pool, 1234), ConstantExpression::make(&pool, "1234"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        auto expr =
            *RelationalExpression::makeEndsWith(&pool,
                                                ConstantExpression::make(&pool, Value::kNullValue),
                                                ConstantExpression::make(&pool, "NULL"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
    {
        auto expr =
            *RelationalExpression::makeEndsWith(&pool,
                                                ConstantExpression::make(&pool, "Null"),
                                                ConstantExpression::make(&pool, Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
}

TEST_F(ExpressionTest, RelationNotEndsWith) {
    {
        // "abc" not ends with "a"
        auto expr = *RelationalExpression::makeNotEndsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "a"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" not ends with "ab"
        auto expr = *RelationalExpression::makeNotEndsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "ab"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "1234"" not ends with "12"
        auto expr = *RelationalExpression::makeNotEndsWith(
            &pool, ConstantExpression::make(&pool, "1234"), ConstantExpression::make(&pool, "12"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "1234"" not ends with "34"
        auto expr = *RelationalExpression::makeNotEndsWith(
            &pool, ConstantExpression::make(&pool, "1234"), ConstantExpression::make(&pool, "34"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" not ends with "bc"
        auto expr = *RelationalExpression::makeNotEndsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "bc"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // "abc" not ends with "ac"
        auto expr = *RelationalExpression::makeNotEndsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "ac"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" not ends with "AB"
        auto expr = *RelationalExpression::makeNotEndsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "AB"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" not ends with "BC"
        auto expr = *RelationalExpression::makeNotEndsWith(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "BC"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // "abc" not ends with 1
        auto expr = *RelationalExpression::makeNotEndsWith(
            &pool, ConstantExpression::make(&pool, "abc1"), ConstantExpression::make(&pool, 1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        // 1234 not ends with 1
        auto expr = *RelationalExpression::makeNotEndsWith(
            &pool, ConstantExpression::make(&pool, 1234), ConstantExpression::make(&pool, 1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, Value::kNullBadType);
    }
    {
        auto expr = *RelationalExpression::makeNotEndsWith(
            &pool,
            ConstantExpression::make(&pool, 1234),
            ConstantExpression::make(&pool, Value::kNullBadData));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        auto expr = *RelationalExpression::makeNotEndsWith(
            &pool, ConstantExpression::make(&pool, 1234), ConstantExpression::make(&pool, "1234"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        auto expr = *RelationalExpression::makeNotEndsWith(
            &pool,
            ConstantExpression::make(&pool, Value::kNullValue),
            ConstantExpression::make(&pool, "NULL"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
    {
        auto expr = *RelationalExpression::makeNotEndsWith(
            &pool,
            ConstantExpression::make(&pool, "Null"),
            ConstantExpression::make(&pool, Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
}

TEST_F(ExpressionTest, ContainsToString) {
    {
        // "abc" contains "a"
        auto expr = *RelationalExpression::makeContains(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "a"));
        ASSERT_EQ("(\"abc\" CONTAINS \"a\")", expr.toString());
    }
    {
        auto expr = *RelationalExpression::makeContains(
            &pool,
            ConstantExpression::make(&pool, 1234),
            ConstantExpression::make(&pool, Value::kNullBadData));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        auto expr = *RelationalExpression::makeContains(
            &pool, ConstantExpression::make(&pool, 1234), ConstantExpression::make(&pool, "1234"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        auto expr =
            *RelationalExpression::makeContains(&pool,
                                                ConstantExpression::make(&pool, Value::kNullValue),
                                                ConstantExpression::make(&pool, "NULL"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
    {
        auto expr =
            *RelationalExpression::makeContains(&pool,
                                                ConstantExpression::make(&pool, "Null"),
                                                ConstantExpression::make(&pool, Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
}

TEST_F(ExpressionTest, NotContainsToString) {
    {
        // "abc" not contains "a"
        auto expr = *RelationalExpression::makeNotContains(
            &pool, ConstantExpression::make(&pool, "abc"), ConstantExpression::make(&pool, "a"));
        ASSERT_EQ("(\"abc\" NOT CONTAINS \"a\")", expr.toString());
    }
    {
        auto expr = *RelationalExpression::makeNotContains(
            &pool,
            ConstantExpression::make(&pool, 1234),
            ConstantExpression::make(&pool, Value::kNullBadData));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        auto expr = *RelationalExpression::makeNotContains(
            &pool, ConstantExpression::make(&pool, 1234), ConstantExpression::make(&pool, "1234"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), true);
    }
    {
        auto expr = *RelationalExpression::makeNotContains(
            &pool,
            ConstantExpression::make(&pool, Value::kNullValue),
            ConstantExpression::make(&pool, "NULL"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
    {
        auto expr = *RelationalExpression::makeNotContains(
            &pool,
            ConstantExpression::make(&pool, "Null"),
            ConstantExpression::make(&pool, Value::kNullValue));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval.isBadNull(), false);
    }
}

TEST_F(ExpressionTest, CaseExprToString) {
    {
        auto *cases = CaseList::make(&pool);
        cases->add(ConstantExpression::make(&pool, 24), ConstantExpression::make(&pool, 1));
        auto expr = *CaseExpression::make(&pool, cases);
        expr.setCondition(ConstantExpression::make(&pool, 23));
        ASSERT_EQ("CASE 23 WHEN 24 THEN 1 END", expr.toString());
    }
    {
        auto *cases = CaseList::make(&pool);
        cases->add(ConstantExpression::make(&pool, 24), ConstantExpression::make(&pool, 1));
        auto expr = *CaseExpression::make(&pool, cases);
        expr.setCondition(ConstantExpression::make(&pool, 23));
        expr.setDefault(ConstantExpression::make(&pool, 2));
        ASSERT_EQ("CASE 23 WHEN 24 THEN 1 ELSE 2 END", expr.toString());
    }
    {
        auto *cases = CaseList::make(&pool);
        cases->add(ConstantExpression::make(&pool, false), ConstantExpression::make(&pool, 1));
        cases->add(ConstantExpression::make(&pool, true), ConstantExpression::make(&pool, 2));
        auto expr = *CaseExpression::make(&pool, cases);
        expr.setCondition(
            RelationalExpression::makeStartsWith(&pool,
                                                 ConstantExpression::make(&pool, "nebula"),
                                                 ConstantExpression::make(&pool, "nebu")));
        expr.setDefault(ConstantExpression::make(&pool, 3));
        ASSERT_EQ(
            "CASE (\"nebula\" STARTS WITH \"nebu\") WHEN false THEN 1 WHEN true THEN 2 ELSE 3 END",
            expr.toString());
    }
    {
        auto *cases = CaseList::make(&pool);
        cases->add(ConstantExpression::make(&pool, 7), ConstantExpression::make(&pool, 1));
        cases->add(ConstantExpression::make(&pool, 8), ConstantExpression::make(&pool, 2));
        cases->add(ConstantExpression::make(&pool, 8), ConstantExpression::make(&pool, "jack"));
        auto expr = *CaseExpression::make(&pool, cases);
        expr.setCondition(ArithmeticExpression::makeAdd(
            &pool, ConstantExpression::make(&pool, 3), ConstantExpression::make(&pool, 5)));
        expr.setDefault(ConstantExpression::make(&pool, false));
        ASSERT_EQ("CASE (3+5) WHEN 7 THEN 1 WHEN 8 THEN 2 WHEN 8 THEN \"jack\" ELSE false END",
                  expr.toString());
    }
    {
        auto *cases = CaseList::make(&pool);
        cases->add(ConstantExpression::make(&pool, false), ConstantExpression::make(&pool, 18));
        auto expr = *CaseExpression::make(&pool, cases);
        ASSERT_EQ("CASE WHEN false THEN 18 END", expr.toString());
    }
    {
        auto *cases = CaseList::make(&pool);
        cases->add(ConstantExpression::make(&pool, false), ConstantExpression::make(&pool, 18));
        auto expr = *CaseExpression::make(&pool, cases);
        expr.setDefault(ConstantExpression::make(&pool, "ok"));
        ASSERT_EQ("CASE WHEN false THEN 18 ELSE \"ok\" END", expr.toString());
    }
    {
        auto *cases = CaseList::make(&pool);
        cases->add(RelationalExpression::makeStartsWith(&pool,
                                                        ConstantExpression::make(&pool, "nebula"),
                                                        ConstantExpression::make(&pool, "nebu")),
                   ConstantExpression::make(&pool, "yes"));
        auto expr = *CaseExpression::make(&pool, cases);
        expr.setDefault(ConstantExpression::make(&pool, false));
        ASSERT_EQ("CASE WHEN (\"nebula\" STARTS WITH \"nebu\") THEN \"yes\" ELSE false END",
                  expr.toString());
    }
    {
        auto *cases = CaseList::make(&pool);
        cases->add(
            RelationalExpression::makeLT(
                &pool, ConstantExpression::make(&pool, 23), ConstantExpression::make(&pool, 17)),
            ConstantExpression::make(&pool, 1));
        cases->add(
            RelationalExpression::makeEQ(
                &pool, ConstantExpression::make(&pool, 37), ConstantExpression::make(&pool, 37)),
            ConstantExpression::make(&pool, 2));
        cases->add(
            RelationalExpression::makeNE(
                &pool, ConstantExpression::make(&pool, 45), ConstantExpression::make(&pool, 99)),
            ConstantExpression::make(&pool, 3));
        auto expr = *CaseExpression::make(&pool, cases);
        expr.setDefault(ConstantExpression::make(&pool, 4));
        ASSERT_EQ("CASE WHEN (23<17) THEN 1 WHEN (37==37) THEN 2 WHEN (45!=99) THEN 3 ELSE 4 END",
                  expr.toString());
    }
    {
        auto *cases = CaseList::make(&pool);
        cases->add(
            RelationalExpression::makeLT(
                &pool, ConstantExpression::make(&pool, 23), ConstantExpression::make(&pool, 17)),
            ConstantExpression::make(&pool, 1));
        auto expr = *CaseExpression::make(&pool, cases, false);
        expr.setDefault(ConstantExpression::make(&pool, 2));
        ASSERT_EQ("((23<17) ? 1 : 2)", expr.toString());
    }
    {
        auto *cases = CaseList::make(&pool);
        cases->add(ConstantExpression::make(&pool, false), ConstantExpression::make(&pool, 1));
        auto expr = *CaseExpression::make(&pool, cases, false);
        expr.setDefault(ConstantExpression::make(&pool, "ok"));
        ASSERT_EQ("(false ? 1 : \"ok\")", expr.toString());
    }
}

TEST_F(ExpressionTest, CaseEvaluate) {
    {
        // CASE 23 WHEN 24 THEN 1 END
        auto *cases = CaseList::make(&pool);
        cases->add(ConstantExpression::make(&pool, 24), ConstantExpression::make(&pool, 1));
        auto expr = *CaseExpression::make(&pool, cases);
        expr.setCondition(ConstantExpression::make(&pool, 23));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_EQ(value, Value::kNullValue);
    }
    {
        // CASE 23 WHEN 24 THEN 1 ELSE false END
        auto *cases = CaseList::make(&pool);
        cases->add(ConstantExpression::make(&pool, 24), ConstantExpression::make(&pool, 1));
        auto expr = *CaseExpression::make(&pool, cases);
        expr.setCondition(ConstantExpression::make(&pool, 23));
        expr.setDefault(ConstantExpression::make(&pool, false));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(value.getBool(), false);
    }
    {
        // CASE ("nebula" STARTS WITH "nebu") WHEN false THEN 1 WHEN true THEN 2 ELSE 3 END
        auto *cases = CaseList::make(&pool);
        cases->add(ConstantExpression::make(&pool, false), ConstantExpression::make(&pool, 1));
        cases->add(ConstantExpression::make(&pool, true), ConstantExpression::make(&pool, 2));
        auto expr = *CaseExpression::make(&pool, cases);
        expr.setCondition(
            RelationalExpression::makeStartsWith(&pool,
                                                 ConstantExpression::make(&pool, "nebula"),
                                                 ConstantExpression::make(&pool, "nebu")));
        expr.setDefault(ConstantExpression::make(&pool, 3));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(2, value.getInt());
    }
    {
        // CASE (3+5) WHEN 7 THEN 1 WHEN 8 THEN 2 WHEN 8 THEN "jack" ELSE "no" END
        auto *cases = CaseList::make(&pool);
        cases->add(ConstantExpression::make(&pool, 7), ConstantExpression::make(&pool, 1));
        cases->add(ConstantExpression::make(&pool, 8), ConstantExpression::make(&pool, 2));
        cases->add(ConstantExpression::make(&pool, 8), ConstantExpression::make(&pool, "jack"));
        auto expr = *CaseExpression::make(&pool, cases);
        expr.setCondition(ArithmeticExpression::makeAdd(
            &pool, ConstantExpression::make(&pool, 3), ConstantExpression::make(&pool, 5)));
        expr.setDefault(ConstantExpression::make(&pool, "no"));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(2, value.getInt());
    }
    {
        // CASE WHEN false THEN 18 END
        auto *cases = CaseList::make(&pool);
        cases->add(ConstantExpression::make(&pool, false), ConstantExpression::make(&pool, 18));
        auto expr = *CaseExpression::make(&pool, cases);
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_EQ(value, Value::kNullValue);
    }
    {
        // CASE WHEN false THEN 18 ELSE ok END
        auto *cases = CaseList::make(&pool);
        cases->add(ConstantExpression::make(&pool, false), ConstantExpression::make(&pool, 18));
        auto expr = *CaseExpression::make(&pool, cases);
        expr.setDefault(ConstantExpression::make(&pool, "ok"));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isStr());
        ASSERT_EQ("ok", value.getStr());
    }
    {
        // CASE WHEN "invalid when" THEN "no" ELSE 3 END
        auto *cases = CaseList::make(&pool);
        cases->add(ConstantExpression::make(&pool, "invalid when"),
                   ConstantExpression::make(&pool, "no"));
        auto expr = *CaseExpression::make(&pool, cases);
        expr.setDefault(ConstantExpression::make(&pool, 3));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_EQ(value, Value::kNullBadType);
    }
    {
        // CASE WHEN (23<17) THEN 1 WHEN (37==37) THEN 2 WHEN (45!=99) THEN 3 ELSE 4 END
        auto *cases = CaseList::make(&pool);
        cases->add(
            RelationalExpression::makeLT(
                &pool, ConstantExpression::make(&pool, 23), ConstantExpression::make(&pool, 17)),
            ConstantExpression::make(&pool, 1));
        cases->add(
            RelationalExpression::makeEQ(
                &pool, ConstantExpression::make(&pool, 37), ConstantExpression::make(&pool, 37)),
            ConstantExpression::make(&pool, 2));
        cases->add(
            RelationalExpression::makeNE(
                &pool, ConstantExpression::make(&pool, 45), ConstantExpression::make(&pool, 99)),
            ConstantExpression::make(&pool, 3));
        auto expr = *CaseExpression::make(&pool, cases);
        expr.setDefault(ConstantExpression::make(&pool, 4));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(2, value.getInt());
    }
    {
        // ((23<17) ? 1 : 2)
        auto *cases = CaseList::make(&pool);
        cases->add(
            RelationalExpression::makeLT(
                &pool, ConstantExpression::make(&pool, 23), ConstantExpression::make(&pool, 17)),
            ConstantExpression::make(&pool, 1));
        auto expr = *CaseExpression::make(&pool, cases, false);
        expr.setDefault(ConstantExpression::make(&pool, 2));
        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isInt());
        ASSERT_EQ(2, value.getInt());
    }
    {
        // (false ? 1 : "ok")
        auto *cases = CaseList::make(&pool);
        cases->add(ConstantExpression::make(&pool, false), ConstantExpression::make(&pool, 1));
        auto expr = *CaseExpression::make(&pool, cases, false);
        expr.setDefault(ConstantExpression::make(&pool, "ok"));
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
        auto expr = *PredicateExpression::make(
            &pool,
            "all",
            "n",
            FunctionCallExpression::make(&pool, "range", argList),
            RelationalExpression::makeGE(
                &pool, LabelExpression::make(&pool, "n"), ConstantExpression::make(&pool, 2)));
        ASSERT_EQ("all(n IN range(1,5) WHERE (n>=2))", expr.toString());
    }
}

TEST_F(ExpressionTest, PredicateEvaluate) {
    {
        // all(n IN [0, 1, 2, 4, 5) WHERE n >= 2)
        auto *listItems = ExpressionList::make(&pool);
        (*listItems)
            .add(ConstantExpression::make(&pool, 0))
            .add(ConstantExpression::make(&pool, 1))
            .add(ConstantExpression::make(&pool, 2))
            .add(ConstantExpression::make(&pool, 4))
            .add(ConstantExpression::make(&pool, 5));
        auto expr = *PredicateExpression::make(
            &pool,
            "all",
            "n",
            ListExpression::make(&pool, listItems),
            RelationalExpression::makeGE(
                &pool, VariableExpression::make(&pool, "n"), ConstantExpression::make(&pool, 2)));

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
        argList->addArgument(VariableExpression::make(&pool, "p"));
        auto expr = *PredicateExpression::make(
            &pool,
            "any",
            "n",
            FunctionCallExpression::make(&pool, "nodes", argList),
            RelationalExpression::makeGE(
                &pool,
                AttributeExpression::make(&pool,
                                          VariableExpression::make(&pool, "n"),
                                          ConstantExpression::make(&pool, "age")),
                ConstantExpression::make(&pool, 19)));

        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(true, value.getBool());
    }
    {
        // single(n IN [0, 1, 2, 4, 5) WHERE n == 2)
        auto *listItems = ExpressionList::make(&pool);
        (*listItems)
            .add(ConstantExpression::make(&pool, 0))
            .add(ConstantExpression::make(&pool, 1))
            .add(ConstantExpression::make(&pool, 2))
            .add(ConstantExpression::make(&pool, 4))
            .add(ConstantExpression::make(&pool, 5));
        auto expr = *PredicateExpression::make(
            &pool,
            "single",
            "n",
            ListExpression::make(&pool, listItems),
            RelationalExpression::makeEQ(
                &pool, VariableExpression::make(&pool, "n"), ConstantExpression::make(&pool, 2)));

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
        argList->addArgument(VariableExpression::make(&pool, "p"));
        auto expr = *PredicateExpression::make(
            &pool,
            "none",
            "n",
            FunctionCallExpression::make(&pool, "nodes", argList),
            RelationalExpression::makeGE(
                &pool,
                AttributeExpression::make(&pool,
                                          VariableExpression::make(&pool, "n"),
                                          ConstantExpression::make(&pool, "age")),
                ConstantExpression::make(&pool, 19)));

        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_TRUE(value.isBool());
        ASSERT_EQ(false, value.getBool());
    }
    {
        // single(n IN null WHERE n > 1)
        auto expr = *PredicateExpression::make(
            &pool,
            "all",
            "n",
            ConstantExpression::make(&pool, Value(NullType::__NULL__)),
            RelationalExpression::makeEQ(
                &pool, VariableExpression::make(&pool, "n"), ConstantExpression::make(&pool, 1)));

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
        auto expr = *ReduceExpression::make(
            &pool,
            "totalNum",
            ArithmeticExpression::makeMultiply(
                &pool, ConstantExpression::make(&pool, 2), ConstantExpression::make(&pool, 10)),
            "n",
            FunctionCallExpression::make(&pool, "range", argList),
            ArithmeticExpression::makeAdd(
                &pool,
                LabelExpression::make(&pool, "totalNum"),
                ArithmeticExpression::makeMultiply(
                    &pool, LabelExpression::make(&pool, "n"), ConstantExpression::make(&pool, 2))));
        ASSERT_EQ("reduce(totalNum = (2*10), n IN range(1,5) | (totalNum+(n*2)))", expr.toString());
    }
}

TEST_F(ExpressionTest, ReduceEvaluate) {
    {
        // reduce(totalNum = 2 * 10, n IN range(1, 5) | totalNum + n * 2)
        ArgumentList *argList = ArgumentList::make(&pool);
        argList->addArgument(ConstantExpression::make(&pool, 1));
        argList->addArgument(ConstantExpression::make(&pool, 5));
        auto expr = *ReduceExpression::make(
            &pool,
            "totalNum",
            ArithmeticExpression::makeMultiply(
                &pool, ConstantExpression::make(&pool, 2), ConstantExpression::make(&pool, 10)),
            "n",
            FunctionCallExpression::make(&pool, "range", argList),
            ArithmeticExpression::makeAdd(
                &pool,
                VariableExpression::make(&pool, "totalNum"),
                ArithmeticExpression::makeMultiply(&pool,
                                                   VariableExpression::make(&pool, "n"),
                                                   ConstantExpression::make(&pool, 2))));

        auto value = Expression::eval(&expr, gExpCtxt);
        ASSERT_EQ(Value::Type::INT, value.type());
        ASSERT_EQ(50, value.getInt());
    }
}

TEST_F(ExpressionTest, TestExprClone) {
    auto expr = *ConstantExpression::make(&pool, 1);
    auto clone = expr.clone();
    ASSERT_EQ(*clone, expr);

    auto aexpr = ArithmeticExpression::makeAdd(
        &pool, ConstantExpression::make(&pool, 1), ConstantExpression::make(&pool, 1));
    ASSERT_EQ(*aexpr, *aexpr->clone());

    auto aggExpr =
        AggregateExpression::make(&pool, "COUNT", ConstantExpression::make(&pool, "$-.*"), true);
    ASSERT_EQ(*aggExpr, *aggExpr->clone());

    auto edgeExpr = EdgeExpression::make(&pool);
    ASSERT_EQ(*edgeExpr, *edgeExpr->clone());

    auto vertExpr = VertexExpression::make(&pool);
    ASSERT_EQ(*vertExpr, *vertExpr->clone());

    auto labelExpr = LabelExpression::make(&pool, "label");
    ASSERT_EQ(*labelExpr, *labelExpr->clone());

    auto attrExpr = AttributeExpression::make(
        &pool, LabelExpression::make(&pool, "label"), LabelExpression::make(&pool, "label"));
    ASSERT_EQ(*attrExpr, *attrExpr->clone());

    auto labelAttrExpr = LabelAttributeExpression::make(
        &pool, LabelExpression::make(&pool, "label"), ConstantExpression::make(&pool, "prop"));
    ASSERT_EQ(*labelAttrExpr, *labelAttrExpr->clone());

    auto typeCastExpr = TypeCastingExpression::make(
        &pool, Value::Type::STRING, ConstantExpression::make(&pool, 100));
    ASSERT_EQ(*typeCastExpr, *typeCastExpr->clone());

    auto fnCallExpr = FunctionCallExpression::make(&pool, "count", ArgumentList::make(&pool));
    ASSERT_EQ(*fnCallExpr, *fnCallExpr->clone());

    auto uuidExpr = UUIDExpression::make(&pool, "hello");
    ASSERT_EQ(*uuidExpr, *uuidExpr->clone());

    auto subExpr = SubscriptExpression::make(
        &pool, VariableExpression::make(&pool, "var"), ConstantExpression::make(&pool, 0));
    ASSERT_EQ(*subExpr, *subExpr->clone());

    auto *elist = ExpressionList::make(&pool);
    (*elist)
        .add(ConstantExpression::make(&pool, 12345))
        .add(ConstantExpression::make(&pool, "Hello"))
        .add(ConstantExpression::make(&pool, true));
    auto listExpr = ListExpression::make(&pool, elist);
    ASSERT_EQ(*listExpr, *listExpr->clone());

    auto setExpr = SetExpression::make(&pool, elist);
    ASSERT_EQ(*setExpr, *setExpr->clone());

    auto *mapItems = MapItemList::make(&pool);
    (*mapItems)
        .add("key1", ConstantExpression::make(&pool, 12345))
        .add("key2", ConstantExpression::make(&pool, 12345))
        .add("key3", ConstantExpression::make(&pool, "Hello"))
        .add("key4", ConstantExpression::make(&pool, true));
    auto mapExpr = MapExpression::make(&pool, mapItems);
    ASSERT_EQ(*mapExpr, *mapExpr->clone());

    auto edgePropExpr = EdgePropertyExpression::make(&pool, "edge", "prop");
    ASSERT_EQ(*edgePropExpr, *edgePropExpr->clone());

    auto tagPropExpr = TagPropertyExpression::make(&pool, "tag", "prop");
    ASSERT_EQ(*tagPropExpr, *tagPropExpr->clone());

    auto inputPropExpr = InputPropertyExpression::make(&pool, "input");
    ASSERT_EQ(*inputPropExpr, *inputPropExpr->clone());

    auto varPropExpr = VariablePropertyExpression::make(&pool, "var", "prop");
    ASSERT_EQ(*varPropExpr, *varPropExpr->clone());

    auto srcPropExpr = SourcePropertyExpression::make(&pool, "tag", "prop");
    ASSERT_EQ(*srcPropExpr, *srcPropExpr->clone());

    auto dstPropExpr = DestPropertyExpression::make(&pool, "tag", "prop");
    ASSERT_EQ(*dstPropExpr, *dstPropExpr->clone());

    auto edgeSrcIdExpr = EdgeSrcIdExpression::make(&pool, "edge");
    ASSERT_EQ(*edgeSrcIdExpr, *edgeSrcIdExpr->clone());

    auto edgeTypeExpr = EdgeTypeExpression::make(&pool, "edge");
    ASSERT_EQ(*edgeTypeExpr, *edgeTypeExpr->clone());

    auto edgeRankExpr = EdgeRankExpression::make(&pool, "edge");
    ASSERT_EQ(*edgeRankExpr, *edgeRankExpr->clone());

    auto edgeDstIdExpr = EdgeDstIdExpression::make(&pool, "edge");
    ASSERT_EQ(*edgeDstIdExpr, *edgeDstIdExpr->clone());

    auto varExpr = VariableExpression::make(&pool, "VARNAME");
    ASSERT_EQ(*varExpr, *varExpr->clone());

    auto verVarExpr =
        VersionedVariableExpression::make(&pool, "VARNAME", ConstantExpression::make(&pool, 0));
    ASSERT_EQ(*verVarExpr, *verVarExpr->clone());

    auto *cases = CaseList::make(&pool);
    cases->add(ConstantExpression::make(&pool, 3), ConstantExpression::make(&pool, 9));
    auto caseExpr = CaseExpression::make(&pool, cases);
    caseExpr->setCondition(ConstantExpression::make(&pool, 2));
    caseExpr->setDefault(ConstantExpression::make(&pool, 8));
    ASSERT_EQ(*caseExpr, *caseExpr->clone());

    auto pathBuild = PathBuildExpression::make(&pool);
    pathBuild->add(VariablePropertyExpression::make(&pool, "var1", "path_src"))
        .add(VariablePropertyExpression::make(&pool, "var1", "path_edge1"))
        .add(VariablePropertyExpression::make(&pool, "var1", "path_v1"));
    ASSERT_EQ(*pathBuild, *pathBuild->clone());

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
    auto predExpr = PredicateExpression::make(
        &pool,
        "all",
        "n",
        FunctionCallExpression::make(&pool, "range", argList),
        RelationalExpression::makeGE(
            &pool, LabelExpression::make(&pool, "n"), ConstantExpression::make(&pool, 2)));
    ASSERT_EQ(*predExpr, *predExpr->clone());

    argList = ArgumentList::make(&pool);
    argList->addArgument(ConstantExpression::make(&pool, 1));
    argList->addArgument(ConstantExpression::make(&pool, 5));
    auto reduceExpr = ReduceExpression::make(
        &pool,
        "totalNum",
        ArithmeticExpression::makeMultiply(
            &pool, ConstantExpression::make(&pool, 2), ConstantExpression::make(&pool, 10)),
        "n",
        FunctionCallExpression::make(&pool, "range", argList),
        ArithmeticExpression::makeAdd(
            &pool,
            LabelExpression::make(&pool, "totalNum"),
            ArithmeticExpression::makeMultiply(
                &pool, LabelExpression::make(&pool, "n"), ConstantExpression::make(&pool, 2))));
    ASSERT_EQ(*reduceExpr, *reduceExpr->clone());
}

TEST_F(ExpressionTest, PathBuild) {
    {
        auto expr = *(PathBuildExpression::make(&pool));
        expr.add(VariablePropertyExpression::make(&pool, "var1", "path_src"))
            .add(VariablePropertyExpression::make(&pool, "var1", "path_edge1"))
            .add(VariablePropertyExpression::make(&pool, "var1", "path_v1"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::PATH);
        Path expected;
        expected.src = Vertex("1", {});
        expected.steps.emplace_back(Step(Vertex("2", {}), 1, "edge", 0, {}));
        EXPECT_EQ(eval.getPath(), expected);
    }
    {
        auto expr = *(PathBuildExpression::make(&pool));
        expr.add(VariablePropertyExpression::make(&pool, "var1", "path_src"))
            .add(VariablePropertyExpression::make(&pool, "var1", "path_edge1"))
            .add(VariablePropertyExpression::make(&pool, "var1", "path_v1"))
            .add(VariablePropertyExpression::make(&pool, "var1", "path_edge2"))
            .add(VariablePropertyExpression::make(&pool, "var1", "path_v2"));
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
        expr.add(VariablePropertyExpression::make(&pool, "var1", "path_src"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::PATH);
        Path expected;
        expected.src = Vertex("1", {});
        EXPECT_EQ(eval.getPath(), expected);
    }
    {
        auto expr = *(PathBuildExpression::make(&pool));
        expr.add(VariablePropertyExpression::make(&pool, "var1", "path_src"))
            .add(VariablePropertyExpression::make(&pool, "var1", "path_edge1"));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::PATH);
    }

    auto varPropExpr = [](const std::string &name) {
        return VariablePropertyExpression::make(&pool, "var1", name);
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
        expr.add(VariablePropertyExpression::make(&pool, "var1", "path_src"))
            .add(VariablePropertyExpression::make(&pool, "var1", "path_edge1"))
            .add(VariablePropertyExpression::make(&pool, "var1", "path_v1"));
        EXPECT_EQ(expr.toString(), "PathBuild[$var1.path_src,$var1.path_edge1,$var1.path_v1]");
    }
}

}   // namespace nebula

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);

    return RUN_ALL_TESTS();
}
