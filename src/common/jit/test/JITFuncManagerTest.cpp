/* Copyright (c) 2020 nebula inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/base/Base.h"
#include <gtest/gtest.h>
#include <folly/init/Init.h>
#include "common/jit/NebulaJIT.h"
#include "common/jit/test/AdHocSchemaManager.h"
#include "common/expression/ConstantExpression.h"
#include "common/expression/ArithmeticExpression.h"
#include "common/expression/RelationalExpression.h"
#include "common/expression/LogicalExpression.h"
#include "common/expression/UnaryExpression.h"
#include "common/expression/SymbolPropertyExpression.h"
#include "common/jit/JITFuncManager.h"
#include "common/expression/test/ExpressionContextMock.h"

namespace nebula {

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

TEST(JITTest, FuncTest) {
    mock::AdHocSchemaManager schemaMan;
    schemaMan.addEdgeSchema(0, 1, mockSchema());
    schemaMan.addTagSchema(0, 10, mockSchema());

    JITFuncManager funcMan;
    {
        auto* left = new RelationalExpression(Expression::Kind::kRelLT,
                                              new ConstantExpression(1.0),
                                              new EdgePropertyExpression(new std::string("1"),
                                                                         new std::string("float")));
        LogicalExpression expr(Expression::Kind::kLogicalAnd,
                               left,
                               new DestPropertyExpression(new std::string("10"),
                                                          new std::string("bool_true")));

        auto func = funcMan.getOrCreateExprFunc(0, &schemaMan, &expr);
        ExpressionContextMock exprCtx;
        CHECK_NOTNULL(func);
        JITResult ret;
        func(reinterpret_cast<intptr_t>(&exprCtx), &ret);
        LOG(INFO) << ret.toString();
        EXPECT_EQ(JITResult(true), ret);
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


