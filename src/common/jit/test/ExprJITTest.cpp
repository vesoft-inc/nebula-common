/* Copyright (c) 2020 nebula inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/base/Base.h"
#include <gtest/gtest.h>
#include <folly/init/Init.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include "common/expression/ConstantExpression.h"
#include "common/expression/ArithmeticExpression.h"
#include "common/expression/RelationalExpression.h"
#include "common/expression/LogicalExpression.h"
#include "common/expression/UnaryExpression.h"
#include "common/expression/SymbolPropertyExpression.h"
#include "common/jit/NebulaJIT.h"
#include "common/jit/JITUtils.h"
#include "common/jit/test/AdHocSchemaManager.h"
#include "common/expression/test/ExpressionContextMock.h"

namespace nebula {

void dumpValue(llvm::Value* val) {
    std::string outStr;
    llvm::raw_string_ostream out(outStr);
    val->getType()->print(out);
    LOG(INFO) << "Current value type: " << out.str();
}

void runExprAndCheck(JITExprContext& context,
                     JITResult expected,
                     bool createFuncFailed = false,
                     bool onlyCheckType = false) {
    if (context.funcName_.empty()) {
        context.funcName_ = context.expr_->toString();
    }
    LOG(INFO) << "====================== " << context.funcName_ << " ===============";
    ExpressionContextMock exprCtx;
    FuncPtr func = JITUtils::createExprFunc(context);
    if (createFuncFailed) {
        CHECK(func == nullptr);
        return;
    }
    CHECK_NOTNULL(func);
    JITResult ret;
    func(reinterpret_cast<intptr_t>(&exprCtx), &ret);
    LOG(INFO) << ret.toString();
    if (expected.isNull()) {
        EXPECT_TRUE(ret.isNull());
    } else {
        if (!onlyCheckType) {
            EXPECT_EQ(expected, ret);
        } else {
            EXPECT_EQ(expected.getType(), ret.getType());
        }
    }
}

std::shared_ptr<meta::NebulaSchemaProvider> mockSchema() {
    std::shared_ptr<meta::NebulaSchemaProvider> schema(new meta::NebulaSchemaProvider(0));
    schema->addField("int", meta::cpp2::PropertyType::INT64);
    schema->addField("float", meta::cpp2::PropertyType::DOUBLE);
    schema->addField("bool_true", meta::cpp2::PropertyType::BOOL);
    schema->addField("bool_false", meta::cpp2::PropertyType::BOOL);
    schema->addField("int_null", meta::cpp2::PropertyType::INT64);
    schema->addField("float_null", meta::cpp2::PropertyType::DOUBLE);
    schema->addField("bool_null", meta::cpp2::PropertyType::BOOL);
    schema->addField("string16", meta::cpp2::PropertyType::STRING);
    return schema;
}

TEST(ExprJITTest, ConstantExprTest) {
    auto jit = std::make_unique<NebulaJIT>();
    mock::AdHocSchemaManager schemaMan;
    schemaMan.addEdgeSchema(0, 1, mockSchema());
    schemaMan.addTagSchema(0, 10, mockSchema());

    llvm::LLVMContext context;
    llvm::IRBuilder<> builder(context);

    JITExprContext jitctx;
    jitctx.jit_ = jit.get();
    jitctx.schemaMan_ = &schemaMan;
    jitctx.builder_ = &builder;
    jitctx.spaceId_ = 0;
    {
        ConstantExpression expr(1.0);
        jitctx.funcName_ = "1.0";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, 1.0);
    }
    {
        ConstantExpression expr("hello world!");
        jitctx.funcName_ = "hello world!";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, std::string("hello world!"));
    }
}

TEST(ExprJITTest, ArithmeticExprTest) {
    auto jit = std::make_unique<NebulaJIT>();
    mock::AdHocSchemaManager schemaMan;
    schemaMan.addEdgeSchema(0, 1, mockSchema());
    schemaMan.addTagSchema(0, 10, mockSchema());

    llvm::LLVMContext context;
    llvm::IRBuilder<> builder(context);

    JITExprContext jitctx;
    jitctx.jit_ = jit.get();
    jitctx.schemaMan_ = &schemaMan;
    jitctx.builder_ = &builder;
    jitctx.spaceId_ = 0;
    {
        ArithmeticExpression expr(Expression::Kind::kAdd,
                                  new ConstantExpression(1.0),
                                  new ConstantExpression(2.0));

        jitctx.funcName_ = "1.0 + 2.0";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, 3.0);
    }
    {
        ArithmeticExpression expr(Expression::Kind::kAdd,
                                  new ConstantExpression(1),
                                  new ConstantExpression(2.0));

        jitctx.funcName_ = "1 + 2.0";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, 3.0);
    }
    {
        ArithmeticExpression expr(Expression::Kind::kDivision,
                                  new ConstantExpression(5),
                                  new ConstantExpression(2.0));

        jitctx.funcName_ = "5 / 2.0";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, 2.5);
    }
    {
        ArithmeticExpression expr(Expression::Kind::kDivision,
                                  new ConstantExpression(5),
                                  new ConstantExpression(0));

        jitctx.funcName_ = "5 / 0";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, JITResult(1L), false, true);
    }
    {
        ArithmeticExpression expr(Expression::Kind::kAdd,
                                  new ConstantExpression(1),
                                  new ConstantExpression(std::numeric_limits<int64_t>::max()));

        jitctx.funcName_ = "1 + int64_max";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, JITResult(1L), false, true);
    }
    {
        ArithmeticExpression expr(Expression::Kind::kMultiply,
                                  new ConstantExpression(5),
                                  new ConstantExpression(2.0));

        jitctx.funcName_ = "5 * 2.0";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, 10.0);
    }
    {
        // (1 + 2) - 1 * 2;
        Expression* expr1 = nullptr;
        Expression* expr2 = nullptr;
        expr1 = new ArithmeticExpression(Expression::Kind::kAdd,
                                         new ConstantExpression(1.0),
                                         new ConstantExpression(2.0));
        expr2 = new ArithmeticExpression(Expression::Kind::kMultiply,
                                         new ConstantExpression(1.0),
                                         new ConstantExpression(2.0));
        auto expr = std::make_unique<ArithmeticExpression>(Expression::Kind::kMinus,
                                                           expr1,
                                                           expr2);
        jitctx.funcName_ = "(1.0 + 2.0) - 1.0 * 2.0";
        jitctx.expr_ = expr.get();
        runExprAndCheck(jitctx, 1.0);
    }
    {
        // -2 + 10
        ArithmeticExpression expr(Expression::Kind::kAdd,
                                  new ConstantExpression(10),
                                  new UnaryExpression(Expression::Kind::kUnaryNegate,
                                                      new ConstantExpression(2)));
        jitctx.funcName_ = "10 + (-2)";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, 8L);
    }
    {
        ArithmeticExpression expr(Expression::Kind::kMod,
                                  new ConstantExpression(5),
                                  new ConstantExpression(3));
        jitctx.funcName_ = "5 % 3";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, 2L);
    }
    {
        ArithmeticExpression expr(Expression::Kind::kMod,
                                  new ConstantExpression(-5),
                                  new ConstantExpression(3));
        jitctx.funcName_ = "-5 % 3";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, -2L);
    }
    {
        ArithmeticExpression expr(Expression::Kind::kMod,
                                  new ConstantExpression(5),
                                  new ConstantExpression(-3));
        jitctx.funcName_ = "5 % -3";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, 2L);
    }
    {
        ArithmeticExpression expr(Expression::Kind::kMod,
                                  new ConstantExpression(-5),
                                  new ConstantExpression(-3));
        jitctx.funcName_ = "-5 % -3";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, -2L);
    }
    {
        ArithmeticExpression expr(Expression::Kind::kMod,
                                  new ConstantExpression(5.0),
                                  new ConstantExpression(3.0));
        jitctx.funcName_ = "5.0 % 3.0";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, 2.0);
    }
    {
        auto* left = new ArithmeticExpression(
                                  Expression::Kind::kAdd,
                                  new ConstantExpression(1),
                                  new EdgePropertyExpression(new std::string("1"),
                                                             new std::string("int")));
        ArithmeticExpression expr(Expression::Kind::kMultiply,
                                  left,
                                  new EdgePropertyExpression(new std::string("1"),
                                                             new std::string("int")));
        jitctx.funcName_ = "(1 + 1.int) * 1.int";
        jitctx.expr_ = &expr;

        runExprAndCheck(jitctx, 2L);
    }
    {
        ArithmeticExpression expr(Expression::Kind::kAdd,
                                  new ConstantExpression(1.0),
                                  new EdgePropertyExpression(new std::string("1"),
                                                             new std::string("float")));
        jitctx.funcName_ = "(1.0 + 1.float)";
        jitctx.expr_ = &expr;

        runExprAndCheck(jitctx, 2.1);
    }
    {
        ArithmeticExpression expr(Expression::Kind::kAdd,
                                  new ConstantExpression(1.0),
                                  new EdgePropertyExpression(new std::string("1"),
                                                             new std::string("float_null")));
        jitctx.funcName_ = "(1.0 + 1.float_null)";
        jitctx.expr_ = &expr;

        runExprAndCheck(jitctx, JITResult());
    }
    {
        ArithmeticExpression expr(Expression::Kind::kAdd,
                                  new EdgePropertyExpression(new std::string("1"),
                                                             new std::string("bool_true")),
                                  new EdgePropertyExpression(new std::string("1"),
                                                             new std::string("bool_false")));
        jitctx.funcName_ = "(1.bool_true + 1.bool_false)";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, JITResult(1L));
    }
    {
        ArithmeticExpression expr(Expression::Kind::kAdd,
                                  new ConstantExpression(1L),
                                  new EdgePropertyExpression(new std::string("1"),
                                                             new std::string("bool_true")));
        jitctx.funcName_ = "(1 + 1.bool_true)";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, JITResult(2L));
    }
}

TEST(ExprJITTest, UnaryExprTest) {
    auto jit = std::make_unique<NebulaJIT>();
    mock::AdHocSchemaManager schemaMan;
    schemaMan.addEdgeSchema(0, 1, mockSchema());
    schemaMan.addTagSchema(0, 10, mockSchema());

    llvm::LLVMContext context;
    llvm::IRBuilder<> builder(context);

    JITExprContext jitctx;
    jitctx.jit_ = jit.get();
    jitctx.schemaMan_ = &schemaMan;
    jitctx.builder_ = &builder;
    jitctx.spaceId_ = 0;
    {
        UnaryExpression expr(
                Expression::Kind::kUnaryIncr,
                new EdgePropertyExpression(new std::string("1"), new std::string("float")));

        jitctx.funcName_ = "++1.float";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, 2.1);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kUnaryDecr,
                new EdgePropertyExpression(new std::string("1"), new std::string("int")));

        jitctx.funcName_ = "--1.int";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, 0L);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kUnaryNot,
                new EdgePropertyExpression(new std::string("1"), new std::string("bool_true")));

        jitctx.funcName_ = "!1.bool_true";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, false);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kUnaryNegate,
                new EdgePropertyExpression(new std::string("1"), new std::string("float")));

        jitctx.funcName_ = "-1.float";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, -1.1);
    }
    {
        UnaryExpression expr(
                Expression::Kind::kUnaryPlus,
                new EdgePropertyExpression(new std::string("1"), new std::string("float")));

        jitctx.funcName_ = "+1.float";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, 1.1);
    }
}

TEST(ExprJITTest, RelationExprTest) {
    auto jit = std::make_unique<NebulaJIT>();
    mock::AdHocSchemaManager schemaMan;
    schemaMan.addEdgeSchema(0, 1, mockSchema());
    schemaMan.addTagSchema(0, 10, mockSchema());

    llvm::LLVMContext context;
    llvm::IRBuilder<> builder(context);

    JITExprContext jitctx;
    jitctx.jit_ = jit.get();
    jitctx.schemaMan_ = &schemaMan;
    jitctx.builder_ = &builder;
    jitctx.spaceId_ = 0;
    {
        RelationalExpression expr(Expression::Kind::kRelGT,
                                  new ConstantExpression(2),
                                  new ConstantExpression(1));
        jitctx.funcName_ = "2 > 1";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, true);
    }
    {
        // 5 IN [1, 2, 3]
        RelationalExpression expr(
                Expression::Kind::kRelIn,
                new ConstantExpression(5),
                new ConstantExpression(Value(List(std::vector<Value>{1, 2, 3}))));
        jitctx.funcName_ = "5 IN [1, 2, 3]";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, false);
    }
    {
        // 5 IN []
        RelationalExpression expr(
                Expression::Kind::kRelIn,
                new ConstantExpression(5),
                new ConstantExpression(Value(List(std::vector<Value>()))));
        jitctx.funcName_ = "5 IN []";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, false, true);
    }
    {
        // 2.0 IN [1, 2, 3]
        RelationalExpression expr(
                Expression::Kind::kRelIn,
                new ConstantExpression(2.0),
                new ConstantExpression(Value(List(std::vector<Value>{1, 2, 3}))));
        jitctx.funcName_ = "2.0 IN [1, 2, 3]";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, true);
    }
    {
        // 2.0 IN [1, 2.0, 3]
        RelationalExpression expr(
                Expression::Kind::kRelIn,
                new ConstantExpression(2.0),
                new ConstantExpression(Value(List(std::vector<Value>{1, 2.0, 3}))));
        jitctx.funcName_ = "2.0 IN [1, 2.0, 3]";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, false, true);
    }
    {
        RelationalExpression expr(Expression::Kind::kRelGT,
                                  new ConstantExpression(1.0),
                                  new EdgePropertyExpression(new std::string("1"),
                                                             new std::string("float")));
        jitctx.funcName_ = "(1.0 > 1.float)";
        jitctx.expr_ = &expr;

        runExprAndCheck(jitctx, false);
    }
    {
        RelationalExpression expr(Expression::Kind::kRelGT,
                                  new ConstantExpression(1.0),
                                  new EdgePropertyExpression(new std::string("1"),
                                                             new std::string("float_null")));
        jitctx.funcName_ = "(1.0 > 1.float_null)";
        jitctx.expr_ = &expr;

        runExprAndCheck(jitctx, JITResult());
    }
    {
        // 1.int IN [1, 2, 3]
        RelationalExpression expr(
                Expression::Kind::kRelIn,
                new EdgePropertyExpression(new std::string("1"), new std::string("int")),
                new ConstantExpression(Value(List(std::vector<Value>{1, 2, 3}))));
        jitctx.funcName_ = "1.int IN [1, 2, 3]";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, true);
    }
    {
        // 1.float IN [1, 2, 3]
        RelationalExpression expr(
                Expression::Kind::kRelIn,
                new EdgePropertyExpression(new std::string("1"), new std::string("float")),
                new ConstantExpression(Value(List(std::vector<Value>{1, 2, 3}))));
        jitctx.funcName_ = "1.float IN [1, 2, 3]";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, false);
    }
    {
        auto expr = std::make_unique<RelationalExpression>(
                                          Expression::Kind::kRelEQ,
                                          new ConstantExpression(std::string(16, 'a')),
                                          new EdgePropertyExpression(new std::string("1"),
                                                                     new std::string("string16")));
        jitctx.funcName_ = "'aaaaaaaaaaaaaaaa' == 1.string16";
        jitctx.expr_ = expr.get();
        runExprAndCheck(jitctx, true);
    }
    {
        auto expr = std::make_unique<RelationalExpression>(
                                          Expression::Kind::kRelNE,
                                          new ConstantExpression(std::string(16, 'a')),
                                          new EdgePropertyExpression(new std::string("1"),
                                                                     new std::string("string16")));
        jitctx.funcName_ = "'aaaaaaaaaaaaaaaa' != 1.string16";
        jitctx.expr_ = expr.get();
        runExprAndCheck(jitctx, false);
    }
    {
        auto expr = std::make_unique<RelationalExpression>(
                                          Expression::Kind::kRelGT,
                                          new ConstantExpression(std::string(1, 'b')),
                                          new EdgePropertyExpression(new std::string("1"),
                                                                     new std::string("string16")));
        jitctx.funcName_ = "'b' > 1.string16";
        jitctx.expr_ = expr.get();
        runExprAndCheck(jitctx, true);
    }
    {
        auto expr = std::make_unique<RelationalExpression>(
                                          Expression::Kind::kRelLT,
                                          new ConstantExpression(std::string(15, 'a')),
                                          new EdgePropertyExpression(new std::string("1"),
                                                                     new std::string("string16")));
        jitctx.funcName_ = "'aaaaaaaaaaaaaaa' == 1.string16";
        jitctx.expr_ = expr.get();
        runExprAndCheck(jitctx, true);
    }
    {
        auto expr = std::make_unique<RelationalExpression>(
                                          Expression::Kind::kRelGE,
                                          new ConstantExpression(std::string(16, 'a')),
                                          new EdgePropertyExpression(new std::string("1"),
                                                                     new std::string("string16")));
        jitctx.funcName_ = "'aaaaaaaaaaaaaaaa' == 1.string16";
        jitctx.expr_ = expr.get();
        runExprAndCheck(jitctx, true);
    }
    {
        auto expr = std::make_unique<RelationalExpression>(
                                          Expression::Kind::kRelGE,
                                          new ConstantExpression(std::string(17, 'a')),
                                          new EdgePropertyExpression(new std::string("1"),
                                                                     new std::string("string16")));
        jitctx.funcName_ = "'aaaaaaaaaaaaaaaaa' == 1.string16";
        jitctx.expr_ = expr.get();
        runExprAndCheck(jitctx, true);
    }
}

TEST(ExprJITTest, LogicalExprTest) {
    auto jit = std::make_unique<NebulaJIT>();
    mock::AdHocSchemaManager schemaMan;
    schemaMan.addEdgeSchema(0, 1, mockSchema());
    schemaMan.addTagSchema(0, 10, mockSchema());

    llvm::LLVMContext context;
    llvm::IRBuilder<> builder(context);

    JITExprContext jitctx;
    jitctx.jit_ = jit.get();
    jitctx.schemaMan_ = &schemaMan;
    jitctx.builder_ = &builder;
    jitctx.spaceId_ = 0;
    {
        // (2 > 1) && (1.0 < 3.0)
        Expression* expr1 = nullptr;
        Expression* expr2 = nullptr;
        expr1 = new RelationalExpression(Expression::Kind::kRelGT,
                                         new ConstantExpression(2),
                                         new ConstantExpression(1));
        expr2 = new RelationalExpression(Expression::Kind::kRelLT,
                                         new ConstantExpression(1.0),
                                         new ConstantExpression(3.0));
        LogicalExpression expr(Expression::Kind::kLogicalAnd, expr1, expr2);
        jitctx.funcName_ = "(2 > 1) && (1.0 < 3.0)";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, true);
    }
    {
        auto* left = new RelationalExpression(Expression::Kind::kRelLT,
                                              new ConstantExpression(1.0),
                                              new EdgePropertyExpression(new std::string("1"),
                                                                         new std::string("float")));
        auto* right = new RelationalExpression(Expression::Kind::kRelGE,
                                               new SourcePropertyExpression(new std::string("10"),
                                                                            new std::string("int")),
                                               new ConstantExpression(1.0));
        LogicalExpression expr(Expression::Kind::kLogicalAnd, left, right);

        jitctx.funcName_ = "(1.0 < 1.float) && (10.int >= 1.0)";
        jitctx.expr_ = &expr;

        runExprAndCheck(jitctx, true);
    }
    {
        auto* left = new RelationalExpression(Expression::Kind::kRelLT,
                                              new ConstantExpression(1.0),
                                              new EdgePropertyExpression(new std::string("1"),
                                                                         new std::string("float")));
        LogicalExpression expr(Expression::Kind::kLogicalAnd,
                               left,
                               new DestPropertyExpression(new std::string("10"),
                                                          new std::string("bool_true")));

        jitctx.funcName_ = "(1.0 < 1.float) && 10.bool_true";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, true);
    }
    {
        auto* left = new RelationalExpression(Expression::Kind::kRelLT,
                                              new ConstantExpression(1.0),
                                              new EdgePropertyExpression(new std::string("1"),
                                                                         new std::string("float")));
        LogicalExpression expr(Expression::Kind::kLogicalAnd,
                               left,
                               new DestPropertyExpression(new std::string("10"),
                                                          new std::string("bool_null")));

        jitctx.funcName_ = "(1.0 < 1.float) && 10.bool_null";
        jitctx.expr_ = &expr;
        runExprAndCheck(jitctx, JITResult());
    }
}


}   // namespace nebula

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);
    nebula::NebulaJIT::init();
    return RUN_ALL_TESTS();
}


