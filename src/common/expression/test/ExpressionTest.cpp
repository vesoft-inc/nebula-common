/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */


#include <gtest/gtest.h>
#include <boost/algorithm/string.hpp>
#include "common/datatypes/List.h"
#include "common/datatypes/Map.h"
#include "common/datatypes/Set.h"
#include "common/datatypes/DataSet.h"
#include "common/expression/test/ExpressionContextMock.h"
#include "common/expression/ArithmeticExpression.h"
#include "common/expression/ConstantExpression.h"
#include "common/expression/SymbolPropertyExpression.h"
#include "common/expression/RelationalExpression.h"
#include "common/expression/UnaryExpression.h"
#include "common/expression/VariableExpression.h"
#include "common/expression/TypeCastingExpression.h"
#include "common/expression/LogicalExpression.h"
#include "common/expression/FunctionCallExpression.h"

nebula::ExpressionContextMock gExpCtxt;

namespace nebula {

static void InsertSpace(std::string &str) {
    for (unsigned int i = 0; i < str.size(); i++) {
        if (str[i] == '(') {
            str.insert(i + 1, 1, ' ');
        } else if (str[i] == ')') {
            str.insert(i, 1, ' ');
            i += 1;
        } else {
            continue;
        }
    }
}

static std::vector<std::string> InfixToSuffix(const std::vector<std::string> &expr) {
    std::vector<std::string> values;
    std::stack<std::string> operators;
    std::unordered_map<std::string, int8_t> priority = {{"||", 1},
                                                        {"&&", 2},
                                                        {"^", 3},
                                                        {"==", 4},
                                                        {"!=", 4},
                                                        {">=", 5},
                                                        {"<=", 5},
                                                        {">", 5},
                                                        {"<", 5},
                                                        {"+", 6},
                                                        {"-", 6},
                                                        {"*", 7},
                                                        {"/", 7},
                                                        {"%", 7},
                                                        {"!", 8}};

    for (const auto &str : expr) {
        if (priority.find(str) != priority.end() || str == "(") {
            if (operators.empty() || str == "(") {
                operators.push(str);
            } else {
                if (operators.top() == "(" || priority[str] > priority[operators.top()]) {
                    operators.push(str);
                } else {
                    while (!operators.empty() && priority[str] <= priority[operators.top()]) {
                        values.push_back(operators.top());
                        operators.pop();
                    }
                    operators.push(str);
                }
            }
        } else if (str == ")") {
            while (!operators.empty() && operators.top() != "(") {
                values.push_back(operators.top());
                operators.pop();
            }
            operators.pop();
        } else {
            values.push_back(str);
        }
    }
    while (!operators.empty()) {
        values.push_back(operators.top());
        operators.pop();
    }
    return values;
}

class ExpressionTest : public ::testing::Test {
public:
    void SetUp() override {}
    void TearDown() override {}

private:
    static std::unordered_map<std::string, Value> boolen_;
    static std::unordered_map<std::string, Expression::Kind> op_;

protected:
    Expression *ExpressionCalu(const std::vector<std::string> &expr) {
        std::vector<std::string> relationOp = {">", ">=", "<", "<=", "==", "!="};
        std::vector<std::string> logicalOp = {"&&", "||", "^"};
        std::vector<std::string> arithmeticOp = {"+", "-", "*", "/", "%"};

        std::vector<std::string> symbol = InfixToSuffix(expr);
        if (symbol.size() == 1) {
            // TEST_EXPR(true, true)
            if (boolen_.find(symbol.front()) != boolen_.end()) {
                return new ConstantExpression(boolen_[symbol.front()]);
            } else if (symbol.front().find('.') != std::string::npos) {
                // TEST_EXPR(123.0, 123.0)
                return new ConstantExpression(::atof(symbol.front().c_str()));
            }
            // TEST_EXPR(123, 123)
            return new ConstantExpression(::atoi(symbol.front().c_str()));
        }

        // calu suffix expression
        std::stack<Expression *> value;
        for (const auto &str : symbol) {
            if (op_.find(str) == op_.end()) {
                Expression *ep = nullptr;
                if (boolen_.find(str) != boolen_.end()) {
                    ep = new ConstantExpression(boolen_[str.c_str()]);
                } else if (str.find('.') != std::string::npos) {
                    ep = new ConstantExpression(::atof(str.c_str()));
                } else {
                    ep = new ConstantExpression(::atoi(str.c_str()));
                }
                value.push(ep);
            } else {
                Expression *result = nullptr;
                Expression *rhs = value.top();
                value.pop();
                Expression *lhs = value.top();
                value.pop();
                if (std::find(arithmeticOp.begin(), arithmeticOp.end(), str) !=
                    arithmeticOp.end()) {
                    result = new ArithmeticExpression(op_[str], lhs, rhs);
                } else if (std::find(relationOp.begin(), relationOp.end(), str) !=
                           relationOp.end()) {
                    result = new RelationalExpression(op_[str], lhs, rhs);
                } else if (std::find(logicalOp.begin(), logicalOp.end(), str) != logicalOp.end()) {
                    result = new LogicalExpression(op_[str], lhs, rhs);
                } else {
                    return new ConstantExpression(NullType::UNKNOWN_PROP);
                }
                value.push(result);
            }
        }
        return value.top();
    }

    void testExpr(const std::string &exprSymbol, Value expected) {
        std::string expr(exprSymbol);
        InsertSpace(expr);
        std::vector<std::string> splitString;
        boost::split(splitString, expr, boost::is_any_of(" \t"));
        Expression *ep = ExpressionCalu(splitString);
        auto eval = Expression::eval(ep, gExpCtxt);
        EXPECT_EQ(eval.type(), expected.type());
        EXPECT_EQ(eval, expected);
        delete ep;
    }
};

std::unordered_map<std::string, Expression::Kind> ExpressionTest::op_ = {
    {"+", Expression::Kind::kAdd},
    {"-", Expression::Kind::kMinus},
    {"*", Expression::Kind::kMultiply},
    {"/", Expression::Kind::kDivision},
    {"%", Expression::Kind::kMod},
    {"||", Expression::Kind::kLogicalOr},
    {"&&", Expression::Kind::kLogicalAnd},
    {"^", Expression::Kind::kLogicalXor},
    {">", Expression::Kind::kRelGT},
    {"<", Expression::Kind::kRelLT},
    {">=", Expression::Kind::kRelGE},
    {"<=", Expression::Kind::kRelLE},
    {"==", Expression::Kind::kRelEQ},
    {"!=", Expression::Kind::kRelNE},
    {"!", Expression::Kind::kUnaryNot}};

std::unordered_map<std::string, Value> ExpressionTest::boolen_ = {{"true", Value(true)},
                                                                  {"false", Value(false)}};

// expr -- the expression can evaluate by nGQL parser may not evaluated by c++
// expected -- the expected value of expression must evaluated by c++
#define TEST_EXPR(expr, expected)                                                                  \
    do {                                                                                           \
        testExpr(#expr, expected);                                                                 \
    } while (0);

TEST_F(ExpressionTest, Constant) {
    {
        ConstantExpression integer(1);
        auto eval = Expression::eval(&integer, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        ConstantExpression doubl(1.0);
        auto eval = Expression::eval(&doubl, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::FLOAT);
        EXPECT_EQ(eval, 1.0);
    }
    {
        ConstantExpression boolean(true);
        auto eval = Expression::eval(&boolean, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        ConstantExpression boolean(false);
        auto eval = Expression::eval(&boolean, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        ConstantExpression str("abcd");
        auto eval = Expression::eval(&str, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::STRING);
        EXPECT_EQ(eval, "abcd");
    }
    {
        Value emptyValue;
        ConstantExpression empty(emptyValue);
        auto eval = Expression::eval(&empty, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::__EMPTY__);
        EXPECT_EQ(eval, emptyValue);
    }
    {
        NullType null;
        ConstantExpression nul(null);
        auto eval = Expression::eval(&nul, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::NULLVALUE);
        EXPECT_EQ(eval, null);
    }
    {
        ConstantExpression date(Date(1234));
        auto eval = Expression::eval(&date, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::DATE);
        EXPECT_EQ(eval, Date(1234));
    }
    {
        DateTime dateTime;
        dateTime.year = 1900;
        dateTime.month = 2;
        dateTime.day = 23;
        ConstantExpression datetime(dateTime);
        auto eval = Expression::eval(&datetime, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::DATETIME);
        EXPECT_EQ(eval, dateTime);
    }
    {
        List listValue(std::vector<Value>{1, 2, 3});
        ConstantExpression list(listValue);
        auto eval = Expression::eval(&list, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::LIST);
        EXPECT_EQ(eval, listValue);
    }
    {
        Map mapValue;
        ConstantExpression map(mapValue);
        auto eval = Expression::eval(&map, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::MAP);
        EXPECT_EQ(eval, mapValue);
    }
    {
        Set setValue;
        ConstantExpression set(setValue);
        auto eval = Expression::eval(&set, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::SET);
        EXPECT_EQ(eval, setValue);
    }
}

TEST_F(ExpressionTest, GetProp) {
    {
        // e1.int
        EdgePropertyExpression ep(new std::string("e1"), new std::string("int"));
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // $-.int
        InputPropertyExpression ep(new std::string("int"));
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // $^.source.int
        SourcePropertyExpression ep(new std::string("source"), new std::string("int"));
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // $$.dest.int
        DestPropertyExpression ep(new std::string("dest"), new std::string("int"));
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // $var.float
        VariablePropertyExpression ep(new std::string("var"), new std::string("float"));
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::FLOAT);
        EXPECT_EQ(eval, 1.1);
    }
}

TEST_F(ExpressionTest, EdgeTest) {
    {
        // EdgeName._src
        EdgeSrcIdExpression ep(new std::string("edge1"));
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // EdgeName._type
        EdgeTypeExpression ep(new std::string("edge1"));
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // EdgeName._rank
        EdgeRankExpression ep(new std::string("edge1"));
        auto eval = Expression::eval(&ep, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // EdgeName._dst
        EdgeDstIdExpression ep(new std::string("edge1"));
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
        TEST_EXPR(true ^ true, false);
        TEST_EXPR(true ^ false, true);
        TEST_EXPR(false ^ false, false);
        TEST_EXPR(false ^ true, true);
    }
    {
        TEST_EXPR(true && true && true, true);
        TEST_EXPR(true && true || false, true);
        TEST_EXPR(true && true && false, false);
        TEST_EXPR(true || false && true || false, true);
        TEST_EXPR(true ^ true ^ false, false);
    }
    {
        // AND
        TEST_EXPR(true && true, true);
        TEST_EXPR(true && false, false);
        TEST_EXPR(false && true, false);
        TEST_EXPR(false && false, false);

        // AND AND  ===  (AND) AND
        TEST_EXPR(true && true && true, true);
        TEST_EXPR(true && true && false, false);
        TEST_EXPR(true && false && true, false);
        TEST_EXPR(true && false && false, false);
        TEST_EXPR(false && true && true, false);
        TEST_EXPR(false && true && false, false);
        TEST_EXPR(false && false && true, false);
        TEST_EXPR(false && false && false, false);

        // OR
        TEST_EXPR(true || true, true);
        TEST_EXPR(true || false, true);
        TEST_EXPR(false || true, true);
        TEST_EXPR(false || false, false);

        // OR OR  ===  (OR) OR
        TEST_EXPR(true || true || true, true);
        TEST_EXPR(true || true || false, true);
        TEST_EXPR(true || false || true, true);
        TEST_EXPR(true || false || false, true);
        TEST_EXPR(false || true || true, true);
        TEST_EXPR(false || true || false, true);
        TEST_EXPR(false || false || true, true);
        TEST_EXPR(false || false || false, false);

        // AND OR  ===  (AND) OR
        TEST_EXPR(true && true || true, true);
        TEST_EXPR(true && true || false, true);
        TEST_EXPR(true && false || true, true);
        TEST_EXPR(true && false || false, false);
        TEST_EXPR(false && true || true, true);
        TEST_EXPR(false && true || false, false);
        TEST_EXPR(false && false || true, true);
        TEST_EXPR(false && false || false, false);

        // OR AND  === OR (AND)
        TEST_EXPR(true || true && true, true);
        TEST_EXPR(true || true && false, true);
        TEST_EXPR(true || false && true, true);
        TEST_EXPR(true || false && false, true);
        TEST_EXPR(false || true && true, true);
        TEST_EXPR(false || true && false, false);
        TEST_EXPR(false || false && true, false);
        TEST_EXPR(false || false && false, false);
    }
    {
        TEST_EXPR(2 > 1 && 3 > 2, true);
        TEST_EXPR(2 <= 1 && 3 > 2, false);
        TEST_EXPR(2 > 1 && 3 < 2, false);
        TEST_EXPR(2 < 1 && 3 < 2, false);
    }
}

TEST_F(ExpressionTest, LiteralConstantsRelational) {
    {
        TEST_EXPR(true == 1.0, false);
        TEST_EXPR(true == 2.0, false);
        TEST_EXPR(true != 1.0, true);
        TEST_EXPR(true != 2.0, true);
        TEST_EXPR(true > 1.0, false);
        TEST_EXPR(true >= 1.0, true);
        TEST_EXPR(true < 1.0, false);
        TEST_EXPR(true <= 1.0, true);
        TEST_EXPR(false == 0.0, false);
        TEST_EXPR(false == 1.0, false);
        TEST_EXPR(false != 0.0, true);
        TEST_EXPR(false != 1.0, true);
        TEST_EXPR(false > 0.0, false);
        TEST_EXPR(false >= 0.0, true);
        TEST_EXPR(false < 0.0, false);
        TEST_EXPR(false <= 0.0, true);

        TEST_EXPR(true == 1, false);
        TEST_EXPR(true == 2, false);
        TEST_EXPR(true != 1, true);
        TEST_EXPR(true != 2, true);
        TEST_EXPR(true > 1, false);
        TEST_EXPR(true >= 1, true);
        TEST_EXPR(true < 1, false);
        TEST_EXPR(true <= 1, true);
        TEST_EXPR(false == 0, false);
        TEST_EXPR(false == 1, false);
        TEST_EXPR(false != 0, true);
        TEST_EXPR(false != 1, true);
        TEST_EXPR(false > 0, false);
        TEST_EXPR(false >= 0, true);
        TEST_EXPR(false < 0, false);
        TEST_EXPR(false <= 0, true);
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

TEST_F(ExpressionTest, FunctionCall) {
    {
        FunctionCallExpression functionCall(new std::string("int"), new ArgumentList());
        auto eval = Expression::eval(&functionCall, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::__EMPTY__);
        EXPECT_EQ(eval, Value());
    }
}

TEST_F(ExpressionTest, Arithmetics) {
    {
        TEST_EXPR(123, 123);
        TEST_EXPR(-123, -123);
        TEST_EXPR(12.23, 12.23);
        TEST_EXPR(143., 143.);
    }
    {
        TEST_EXPR(10 % 3, 1);
        TEST_EXPR(10 + 3, 13);
        TEST_EXPR(1 - 4, -3);
        TEST_EXPR(11 * 2, 22);
        TEST_EXPR(11 * 2.2, 24.2);
        TEST_EXPR(100.4 / 4, 25.1);
        TEST_EXPR(10.4 % 0, NullType::DIV_BY_ZERO);
        TEST_EXPR(10 % 0.0, NullType::DIV_BY_ZERO);
        TEST_EXPR(10.4 % 0.0, NullType::DIV_BY_ZERO);
        TEST_EXPR(10 / 0, NullType::DIV_BY_ZERO);
        TEST_EXPR(12 / 0.0, NullType::DIV_BY_ZERO);
        TEST_EXPR(187. / 0.0, NullType::DIV_BY_ZERO);
        TEST_EXPR(17. / 0, NullType::DIV_BY_ZERO);
    }
    {
        TEST_EXPR(1 + 2 + 3.2, 6.2);
        TEST_EXPR(3 * 4 - 6, 6);
        TEST_EXPR(76 - 100 / 20 * 4, 56);
        TEST_EXPR(17 % 7 + 4 - 2, 5);
        TEST_EXPR(17 + 7 % 4 - 2, 18);
        TEST_EXPR(17 + 7 + 4 % 2, 24);
        TEST_EXPR(3.14 * 3 * 3 / 2, 14.13);
        TEST_EXPR(16 * 8 + 4 - 2, 130);
        TEST_EXPR(16 + 8 * 4 - 2, 46);
        TEST_EXPR(16 + 8 + 4 * 2, 32);
    }
    {
        TEST_EXPR(16 + 8 * (4 - 2), 32);
        TEST_EXPR(16 * (8 + 4) - 2, 190);
        TEST_EXPR(2 * (4 + 3) - 6, 8);
        TEST_EXPR((3 + 5) * 3 / (6 - 2), 6);
    }
    {
        // 1 + 2 + e1.int
        ArithmeticExpression add(
                Expression::Kind::kAdd,
                new ArithmeticExpression(
                    Expression::Kind::kAdd,
                    new ConstantExpression(1), new ConstantExpression(2)),
                new EdgePropertyExpression(new std::string("e1"), new std::string("int")));
        auto eval = Expression::eval(&add, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 4);
    }
    {
        // e1.string16 + e1.string16
        ArithmeticExpression add(
                Expression::Kind::kAdd,
                new EdgePropertyExpression(new std::string("e1"), new std::string("string16")),
                new EdgePropertyExpression(new std::string("e1"), new std::string("string16")));
        auto eval = Expression::eval(&add, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::STRING);
        EXPECT_EQ(eval, std::string(32, 'a'));
    }
    {
        // $^.source.string16 + $$.dest.string16
        ArithmeticExpression add(
            Expression::Kind::kAdd,
            new SourcePropertyExpression(new std::string("source"), new std::string("string16")),
            new DestPropertyExpression(new std::string("dest"), new std::string("string16")));
        auto eval = Expression::eval(&add, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::STRING);
        EXPECT_EQ(eval, std::string(32, 'a'));
    }
    {
        // 10 - e1.int
        ArithmeticExpression minus(
            Expression::Kind::kMinus,
            new ConstantExpression(10),
            new EdgePropertyExpression(new std::string("e1"), new std::string("int")));
        auto eval = Expression::eval(&minus, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 9);
    }
    {
        // 10 - $^.source.int
        ArithmeticExpression minus(
            Expression::Kind::kMinus,
            new ConstantExpression(10),
            new SourcePropertyExpression(new std::string("source"), new std::string("int")));
        auto eval = Expression::eval(&minus, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 9);
    }
    {
        // e1.string128 - e1.string64
        ArithmeticExpression minus(
            Expression::Kind::kMinus,
            new EdgePropertyExpression(new std::string("e1"), new std::string("string128")),
            new EdgePropertyExpression(new std::string("e1"), new std::string("string64")));
        auto eval = Expression::eval(&minus, gExpCtxt);
        EXPECT_NE(eval.type(), Value::Type::STRING);
        EXPECT_NE(eval, std::string(64, 'a'));
    }
    {
        // $^.source.srcProperty % $$.dest.dstProperty
        ArithmeticExpression mod(
            Expression::Kind::kMod,
            new SourcePropertyExpression(new std::string("source"), new std::string("srcProperty")),
            new DestPropertyExpression(new std::string("dest"), new std::string("dstProperty")));
        auto eval = Expression::eval(&mod, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
}

TEST_F(ExpressionTest, RelationEQ) {
    {
        // e1.list == NULL
        RelationalExpression expr(
                Expression::Kind::kRelEQ,
                new EdgePropertyExpression(new std::string("e1"), new std::string("list")),
                new ConstantExpression(Value(NullType::NaN)));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // e1.list_of_list == NULL
        RelationalExpression expr(
                Expression::Kind::kRelEQ,
                new EdgePropertyExpression(new std::string("e1"), new std::string("list_of_list")),
                new ConstantExpression(Value(NullType::NaN)));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
    {
        // e1.list == e1.list
        RelationalExpression expr(
                Expression::Kind::kRelEQ,
                new EdgePropertyExpression(new std::string("e1"), new std::string("list")),
                new EdgePropertyExpression(new std::string("e1"), new std::string("list")));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // e1.list_of_list == e1.list_of_list
        RelationalExpression expr(
                Expression::Kind::kRelEQ,
                new EdgePropertyExpression(new std::string("e1"), new std::string("list_of_list")),
                new EdgePropertyExpression(new std::string("e1"), new std::string("list_of_list")));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
}

TEST_F(ExpressionTest, RelationIN) {
    {
        // 1 IN [1, 2, 3]
        RelationalExpression expr(
                Expression::Kind::kRelIn,
                new ConstantExpression(1),
                new ConstantExpression(Value(List(std::vector<Value>{1, 2, 3}))));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, true);
    }
    {
        // 5 IN [1, 2, 3]
        RelationalExpression expr(
                Expression::Kind::kRelIn,
                new ConstantExpression(5),
                new ConstantExpression(Value(List(std::vector<Value>{1, 2, 3}))));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::BOOL);
        EXPECT_EQ(eval, false);
    }
}

TEST_F(ExpressionTest, UnaryINCR) {
    {
        // ++var_int
        UnaryExpression expr(
                Expression::Kind::kUnaryIncr,
                new VariableExpression(new std::string("var_int")));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 2);
    }
    {
        // ++versioned_var{0}
        UnaryExpression expr(
                Expression::Kind::kUnaryIncr,
                new VersionedVariableExpression(
                    new std::string("versioned_var"),
                    new ConstantExpression(0)));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 2);
    }
}

TEST_F(ExpressionTest, UnaryDECR) {
    {
        // --var_int
        UnaryExpression expr(
                Expression::Kind::kUnaryDecr,
                new VariableExpression(new std::string("var_int")));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 0);
    }
}

TEST_F(ExpressionTest, VersionedVar) {
    {
        // versioned_var{0}
        VersionedVariableExpression expr(
                new std::string("versioned_var"),
                new ConstantExpression(0));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 1);
    }
    {
        // versioned_var{0}
        VersionedVariableExpression expr(
                new std::string("versioned_var"),
                new ConstantExpression(-1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 2);
    }
    {
        // versioned_var{0}
        VersionedVariableExpression expr(
                new std::string("versioned_var"),
                new ConstantExpression(1));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 8);
    }
    {
        // versioned_var{-cnt}
        VersionedVariableExpression expr(
                new std::string("versioned_var"),
                new UnaryExpression(
                    Expression::Kind::kUnaryNegate,
                    new VariableExpression(new std::string("cnt"))));
        auto eval = Expression::eval(&expr, gExpCtxt);
        EXPECT_EQ(eval.type(), Value::Type::INT);
        EXPECT_EQ(eval, 2);
    }
}

TEST_F(ExpressionTest, CheckComponent) {
    {
        // single node
        const auto root = std::make_unique<ConstantExpression>();

        ASSERT_TRUE(root->isAnyKind(Expression::Kind::kConstant));
        ASSERT_TRUE(root->hasAnyKind(Expression::Kind::kConstant));

        ASSERT_TRUE(root->isAnyKind(Expression::Kind::kConstant, Expression::Kind::kAdd));
        ASSERT_TRUE(root->hasAnyKind(Expression::Kind::kConstant, Expression::Kind::kAdd));

        ASSERT_FALSE(root->isAnyKind(Expression::Kind::kAdd));
        ASSERT_FALSE(root->hasAnyKind(Expression::Kind::kAdd));

        ASSERT_FALSE(root->isAnyKind(Expression::Kind::kDivision, Expression::Kind::kAdd));
        ASSERT_FALSE(root->hasAnyKind(Expression::Kind::kDstProperty, Expression::Kind::kAdd));
    }

    {
        // list like
        const auto root = std::make_unique<TypeCastingExpression>(Value::Type::BOOL,
            new TypeCastingExpression(Value::Type::BOOL,
                new TypeCastingExpression(Value::Type::BOOL,
                    new ConstantExpression())));

        ASSERT_TRUE(root->isAnyKind(Expression::Kind::kTypeCasting));
        ASSERT_TRUE(root->hasAnyKind(Expression::Kind::kConstant));

        ASSERT_TRUE(root->isAnyKind(Expression::Kind::kTypeCasting, Expression::Kind::kAdd));
        ASSERT_TRUE(root->hasAnyKind(Expression::Kind::kTypeCasting, Expression::Kind::kAdd));

        ASSERT_FALSE(root->isAnyKind(Expression::Kind::kAdd));
        ASSERT_FALSE(root->hasAnyKind(Expression::Kind::kAdd));

        ASSERT_FALSE(root->isAnyKind(Expression::Kind::kDivision, Expression::Kind::kAdd));
        ASSERT_FALSE(root->hasAnyKind(Expression::Kind::kDstProperty, Expression::Kind::kAdd));
    }

    {
        // tree like
        const auto root = std::make_unique<ArithmeticExpression>(Expression::Kind::kAdd,
            new ArithmeticExpression(Expression::Kind::kDivision,
                new ConstantExpression(3),
                new ArithmeticExpression(Expression::Kind::kMinus,
                    new ConstantExpression(4),
                    new ConstantExpression(2))),
            new ArithmeticExpression(Expression::Kind::kMod,
                new ArithmeticExpression(Expression::Kind::kMultiply,
                    new ConstantExpression(3),
                    new ConstantExpression(10)),
                new ConstantExpression(2)));

        ASSERT_TRUE(root->isAnyKind(Expression::Kind::kAdd));
        ASSERT_TRUE(root->hasAnyKind(Expression::Kind::kMinus));

        ASSERT_TRUE(root->isAnyKind(Expression::Kind::kTypeCasting, Expression::Kind::kAdd));
        ASSERT_TRUE(root->hasAnyKind(Expression::Kind::kSymProperty, Expression::Kind::kDivision));

        ASSERT_FALSE(root->isAnyKind(Expression::Kind::kConstant));
        ASSERT_FALSE(root->hasAnyKind(Expression::Kind::kFunctionCall));

        ASSERT_FALSE(root->isAnyKind(Expression::Kind::kDivision, Expression::Kind::kEdgeProperty));
        ASSERT_FALSE(root->hasAnyKind(Expression::Kind::kDstProperty,
                                      Expression::Kind::kLogicalAnd));
    }
}

// TODO(cpw): more test cases.
}  // namespace nebula
