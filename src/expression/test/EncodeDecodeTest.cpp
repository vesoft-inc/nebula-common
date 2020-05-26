/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "base/Base.h"
#include <gtest/gtest.h>
#include "expression/AliasPropertyExpression.h"
#include "expression/ArithmeticExpression.h"
#include "expression/ConstantExpression.h"
#include "expression/FunctionCallExpression.h"
#include "expression/LogicalExpression.h"
#include "expression/RelationalExpression.h"
#include "expression/TypeCastingExpression.h"
#include "expression/UUIDExpression.h"
#include "expression/UnaryExpression.h"

namespace nebula {

TEST(ExpressionEncodeDecode, ConstantExpression) {
    ConstantExpression val1(123);
    ConstantExpression val2("Hello world");
    ConstantExpression val3(true);
    ConstantExpression val4(3.14159);

    std::string encoded;

    // ConstantExpression
    val1.encode(encoded);
    char* ptr = &(encoded[0]);
    auto decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(val1, *decoded);
    encoded.clear();

    val2.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(val2, *decoded);
    encoded.clear();

    val3.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(val3, *decoded);
    encoded.clear();

    val4.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(val4, *decoded);
    encoded.clear();
}


TEST(ExpressionEncodeDecode, AliasPropertyExpression) {
    std::string encoded;

    // AliasPropertyExpression
    InputPropertyExpression inputEx(new std::string("prop"));
    inputEx.encode(encoded);
    char* ptr = &(encoded[0]);
    auto decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(inputEx, *decoded);
    encoded.clear();

    VariablePropertyExpression varEx(new std::string("var"), new std::string("prop"));
    varEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(varEx, *decoded);
    encoded.clear();

    SourcePropertyExpression spEx(new std::string("tag"), new std::string("prop"));
    spEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(spEx, *decoded);
    encoded.clear();

    DestPropertyExpression dpEx(new std::string("tag"), new std::string("prop"));
    dpEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(dpEx, *decoded);
    encoded.clear();

    EdgeSrcIdExpression srcIdEx(new std::string("alias"));
    srcIdEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(srcIdEx, *decoded);
    encoded.clear();

    EdgeTypeExpression etEx(new std::string("alias"));
    etEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(etEx, *decoded);
    encoded.clear();

    EdgeRankExpression erEx(new std::string("alias"));
    erEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(erEx, *decoded);
    encoded.clear();

    EdgeDstIdExpression dstIdEx(new std::string("alias"));
    dstIdEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(dstIdEx, *decoded);
    encoded.clear();
}


TEST(ExpressionEncodeDecode, ArithmeticExpression) {
    std::string encoded;

    // ArithmeticExpression
    ArithmeticExpression addEx(Expression::Kind::kAdd,
                               std::make_unique<ConstantExpression>(123),
                               std::make_unique<ConstantExpression>("Hello"));
    addEx.encode(encoded);
    char* ptr = &(encoded[0]);
    auto decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(addEx, *decoded);
    encoded.clear();

    ArithmeticExpression minusEx(Expression::Kind::kMinus,
                                 std::make_unique<ConstantExpression>(3.14),
                                 std::make_unique<ConstantExpression>("Hello"));
    minusEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(minusEx, *decoded);
    encoded.clear();

    ArithmeticExpression multiEx(Expression::Kind::kMultiply,
                                 std::make_unique<ConstantExpression>(3.14),
                                 std::make_unique<ConstantExpression>(1234));
    multiEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(multiEx, *decoded);
    encoded.clear();

    ArithmeticExpression divEx(Expression::Kind::kDivision,
                               std::make_unique<ConstantExpression>(3.14),
                               std::make_unique<ConstantExpression>(1234));
    divEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(divEx, *decoded);
    encoded.clear();

    ArithmeticExpression modEx(Expression::Kind::kDivision,
                               std::make_unique<ConstantExpression>(1234567),
                               std::make_unique<ConstantExpression>(123));
    modEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(modEx, *decoded);
    encoded.clear();
}


TEST(ExpressionEncodeDecode, FunctionCallExpression) {
    std::string encoded;

    // FunctionCallExpression
    ArgumentList* args = new ArgumentList();
    args->addArgument(std::make_unique<ConstantExpression>(123));
    args->addArgument(std::make_unique<ConstantExpression>(3.14));
    args->addArgument(std::make_unique<ConstantExpression>("Hello world"));
    FunctionCallExpression fcEx(new std::string("func"), args);
    fcEx.encode(encoded);
    char* ptr = &(encoded[0]);
    auto decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(fcEx, *decoded);
    encoded.clear();
}


TEST(ExpressionEncodeDecode, RelationalExpression) {
    std::string encoded;

    // RelationalExpression
    RelationalExpression eqEx(Expression::Kind::kRelEQ,
                              std::make_unique<ConstantExpression>(123),
                              std::make_unique<ConstantExpression>(123));
    eqEx.encode(encoded);
    char* ptr = &(encoded[0]);
    auto decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(eqEx, *decoded);
    encoded.clear();

    RelationalExpression neEx(Expression::Kind::kRelEQ,
                              std::make_unique<ConstantExpression>(123),
                              std::make_unique<ConstantExpression>("Hello"));
    neEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(neEx, *decoded);
    encoded.clear();

    RelationalExpression ltEx(Expression::Kind::kRelEQ,
                              std::make_unique<ConstantExpression>(123),
                              std::make_unique<ConstantExpression>(12345));
    ltEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(ltEx, *decoded);
    encoded.clear();

    RelationalExpression leEx(Expression::Kind::kRelEQ,
                              std::make_unique<ConstantExpression>(123),
                              std::make_unique<ConstantExpression>(12345));
    leEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(leEx, *decoded);
    encoded.clear();

    RelationalExpression gtEx(Expression::Kind::kRelEQ,
                              std::make_unique<ConstantExpression>(12345),
                              std::make_unique<ConstantExpression>(123));
    gtEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(gtEx, *decoded);
    encoded.clear();

    RelationalExpression geEx(Expression::Kind::kRelEQ,
                              std::make_unique<ConstantExpression>(12345),
                              std::make_unique<ConstantExpression>(123));
    geEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(geEx, *decoded);
    encoded.clear();
}


TEST(ExpressionEncodeDecode, LogicalExpression) {
    std::string encoded;

    // LogicalExpression
    LogicalExpression andEx(Expression::Kind::kLogicalAnd,
                            std::make_unique<ConstantExpression>(true),
                            std::make_unique<ConstantExpression>(false));
    andEx.encode(encoded);
    char* ptr = &(encoded[0]);
    auto decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(andEx, *decoded);
    encoded.clear();

    auto lhs = std::make_unique<RelationalExpression>(
        Expression::Kind::kRelLT,
        std::make_unique<ConstantExpression>(123),
        std::make_unique<ConstantExpression>(12345));
    auto rhs = std::make_unique<RelationalExpression>(
        Expression::Kind::kRelEQ,
        std::make_unique<ConstantExpression>("Hello"),
        std::make_unique<ConstantExpression>("World"));
    RelationalExpression orEx(Expression::Kind::kLogicalOr,
                              std::move(lhs),
                              std::move(rhs));
    orEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(orEx, *decoded);
    encoded.clear();

    auto arEx = std::make_unique<ArithmeticExpression>(
        Expression::Kind::kAdd,
        std::make_unique<ConstantExpression>(123),
        std::make_unique<ConstantExpression>(456));
    lhs = std::make_unique<RelationalExpression>(
        Expression::Kind::kRelLT,
        std::make_unique<ConstantExpression>(12345),
        std::move(arEx));
    rhs = std::make_unique<RelationalExpression>(
        Expression::Kind::kRelEQ,
        std::make_unique<ConstantExpression>("Hello"),
        std::make_unique<ConstantExpression>("World"));
    RelationalExpression xorEx(Expression::Kind::kLogicalXor,
                               std::move(lhs),
                               std::move(rhs));
    xorEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(xorEx, *decoded);
    encoded.clear();
}


TEST(ExpressionEncodeDecode, TypeCastingExpression) {
    std::string encoded;

    // TypeCastingExpression
    TypeCastingExpression tcEx(Value::Type::INT,
                               std::make_unique<ConstantExpression>(3.14));
    tcEx.encode(encoded);
    char* ptr = &(encoded[0]);
    auto decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(tcEx, *decoded);
    encoded.clear();
}


TEST(ExpressionEncodeDecode, UUIDExpression) {
    std::string encoded;

    // UUIDExpression
    UUIDExpression uuidEx(new std::string("field"));
    uuidEx.encode(encoded);
    char* ptr = &(encoded[0]);
    auto decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(uuidEx, *decoded);
    encoded.clear();
}


TEST(ExpressionEncodeDecode, UnaryExpression) {
    std::string encoded;

    // UnaryExpression
    UnaryExpression plusEx(Expression::Kind::kUnaryPlus,
                           std::make_unique<ConstantExpression>(12345));
    plusEx.encode(encoded);
    char* ptr = &(encoded[0]);
    auto decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(plusEx, *decoded);
    encoded.clear();

    UnaryExpression negEx(Expression::Kind::kUnaryNegate,
                          std::make_unique<ConstantExpression>(12345));
    negEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(negEx, *decoded);
    encoded.clear();

    UnaryExpression notEx(Expression::Kind::kUnaryNot,
                          std::make_unique<ConstantExpression>(false));
    notEx.encode(encoded);
    ptr = &(encoded[0]);
    decoded = Expression::decode(ptr, ptr + encoded.size());
    EXPECT_EQ(notEx, *decoded);
    encoded.clear();
}

}  // namespace nebula


int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);

    return RUN_ALL_TESTS();
}

