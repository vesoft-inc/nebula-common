/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */


#include <gtest/gtest.h>
#include "common/function/FunctionManager.h"

namespace nebula {

class FunctionManagerTest : public ::testing::Test {
public:
    void SetUp() override {}
    void TearDown() override {}

protected:
    void testExpr(const char *expr, std::vector<Value> &args, Value expect) {
        auto result = FunctionManager::get(expr, args.size());
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value()(args), expect);
    }

    static std::unordered_map<std::string, std::vector<Value>> args_;
};

std::unordered_map<std::string, std::vector<Value>> FunctionManagerTest::args_ = {
    {"null", {}},
    {"int", {4}},
    {"float", {1.1}},
    {"neg_int", {-1}},
    {"neg_float", {-1.1}},
    {"rand", {1, 10}},
    {"one", {-1.2}},
    {"two", {2, 4}},
    {"pow", {2, 3}},
    {"string", {"AbcDeFG"}},
    {"trim", {" abc  "}},
    {"substr", {"abcdefghi", 2, 4}},
    {"side", {"abcdefghijklmnopq", 5}},
    {"neg_side", {"abcdefghijklmnopq", -2}},
};

// expr -- the expression can evaluate by nGQL parser may not evaluated by c++
// expected -- the expected value of expression must evaluated by c++
#define TEST_EXPR(expr, args, expected)                                                            \
    do {                                                                                           \
        testExpr(#expr, args, expected);                                                           \
    } while (0);

TEST_F(FunctionManagerTest, functionCall) {
    {
        TEST_EXPR(abs, args_["neg_int"], 1);
        TEST_EXPR(abs, args_["neg_float"], 1.1);
        TEST_EXPR(abs, args_["int"], 4);
        TEST_EXPR(abs, args_["float"], 1.1);
    }
    {
        TEST_EXPR(floor, args_["neg_int"], -1);
        TEST_EXPR(floor, args_["float"], 1);
        TEST_EXPR(floor, args_["neg_float"], -2);
        TEST_EXPR(floor, args_["int"], 4);
    }
    {
        TEST_EXPR(sqrt, args_["int"], 2);
        TEST_EXPR(sqrt, args_["float"], std::sqrt(1.1));
    }

    {
        TEST_EXPR(pow, args_["pow"], 8);
        TEST_EXPR(exp, args_["int"], std::exp(4));
        TEST_EXPR(exp2, args_["int"], 16);

        TEST_EXPR(log, args_["int"], std::log(4));
        TEST_EXPR(log2, args_["int"], 2);
    }
    {
        TEST_EXPR(lower, args_["string"], "abcdefg");
        TEST_EXPR(upper, args_["string"], "ABCDEFG");
        TEST_EXPR(length, args_["string"], 7);

        TEST_EXPR(trim, args_["trim"], "abc");
        TEST_EXPR(ltrim, args_["trim"], "abc  ");
        TEST_EXPR(rtrim, args_["trim"], " abc");
    }
    {
        TEST_EXPR(substr, args_["substr"], "bcde");
        TEST_EXPR(left, args_["side"], "abcde");
        TEST_EXPR(right, args_["side"], "mnopq");
        TEST_EXPR(left, args_["neg_side"], "");
        TEST_EXPR(right, args_["neg_side"], "");
    }
    {
        auto result = FunctionManager::get("rand32", args_["rand"].size());
        ASSERT_TRUE(result.ok());
        result.value()(args_["rand"]);
    }
    {
        auto result = FunctionManager::get("rand32", args_["null"].size());
        ASSERT_TRUE(result.ok());
        result.value()(args_["null"]);
    }
    {
        auto result = FunctionManager::get("now", args_["null"].size());
        ASSERT_TRUE(result.ok());
        result.value()(args_["null"]);
    }
}

}   // namespace nebula

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);
    return RUN_ALL_TESTS();
}
