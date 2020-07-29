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
    void testFunction(const char *expr, std::vector<Value> &args, Value expect) {
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
    {"pad", {"abcdefghijkl", 16, "123"}},
    {"udf_is_in", {4, 1, 2, 8, 4, 3, 1, 0}}};

#define TEST_FUNCTION(expr, args, expected)                                                        \
    do {                                                                                           \
        testFunction(#expr, args, expected);                                                       \
    } while (0);

TEST_F(FunctionManagerTest, functionCall) {
    {
        TEST_FUNCTION(abs, args_["neg_int"], 1);
        TEST_FUNCTION(abs, args_["neg_float"], 1.1);
        TEST_FUNCTION(abs, args_["int"], 4);
        TEST_FUNCTION(abs, args_["float"], 1.1);
    }
    {
        TEST_FUNCTION(floor, args_["neg_int"], -1);
        TEST_FUNCTION(floor, args_["float"], 1);
        TEST_FUNCTION(floor, args_["neg_float"], -2);
        TEST_FUNCTION(floor, args_["int"], 4);
    }
    {
        TEST_FUNCTION(sqrt, args_["int"], 2);
        TEST_FUNCTION(sqrt, args_["float"], std::sqrt(1.1));
    }

    {
        TEST_FUNCTION(pow, args_["pow"], 8);
        TEST_FUNCTION(exp, args_["int"], std::exp(4));
        TEST_FUNCTION(exp2, args_["int"], 16);

        TEST_FUNCTION(log, args_["int"], std::log(4));
        TEST_FUNCTION(log2, args_["int"], 2);
    }
    {
        TEST_FUNCTION(lower, args_["string"], "abcdefg");
        TEST_FUNCTION(upper, args_["string"], "ABCDEFG");
        TEST_FUNCTION(length, args_["string"], 7);

        TEST_FUNCTION(trim, args_["trim"], "abc");
        TEST_FUNCTION(ltrim, args_["trim"], "abc  ");
        TEST_FUNCTION(rtrim, args_["trim"], " abc");
    }
    {
        TEST_FUNCTION(substr, args_["substr"], "bcde");
        TEST_FUNCTION(left, args_["side"], "abcde");
        TEST_FUNCTION(right, args_["side"], "mnopq");
        TEST_FUNCTION(left, args_["neg_side"], "");
        TEST_FUNCTION(right, args_["neg_side"], "");

        TEST_FUNCTION(lpad, args_["pad"], "1231abcdefghijkl");
        TEST_FUNCTION(rpad, args_["pad"], "abcdefghijkl1231");
        TEST_FUNCTION(udf_is_in, args_["udf_is_in"], true);
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
    {
        auto result = FunctionManager::get("hash", args_["string"].size());
        ASSERT_TRUE(result.ok());
        result.value()(args_["string"]);
    }
    {
        auto result = FunctionManager::get("hash", 1);
        ASSERT_TRUE(result.ok());
        auto res = std::move(result).value()({"Hello"});
        EXPECT_EQ(res, 2275118702903107253);
    }
    {
        auto result = FunctionManager::get("hash", 1);
        ASSERT_TRUE(result.ok());
        auto res = std::move(result).value()({3.14159265});
        EXPECT_EQ(res, -8359970742410469755);
    }
    {
        auto result = FunctionManager::get("hash", 1);
        ASSERT_TRUE(result.ok());
        auto res = std::move(result).value()({1234567890});
        EXPECT_EQ(res, 1234567890);
    }
    {
        auto result = FunctionManager::get("hash", 1);
        ASSERT_TRUE(result.ok());
        auto res = std::move(result).value()({true});
        EXPECT_EQ(res, 1);
    }
    {
        auto result = FunctionManager::get("hash", 1);
        ASSERT_TRUE(result.ok());
        auto res = std::move(result).value()({false});
        EXPECT_EQ(res, 0);
    }
}

TEST_F(FunctionManagerTest, returnType) {
    {
        auto result = FunctionManager::getReturnType("abs", {Value::Type::INT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result = FunctionManager::getReturnType("abs", {Value::Type::FLOAT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::FLOAT);
    }
    {
        auto result = FunctionManager::getReturnType("abs", {Value::Type::BOOL});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Parameter's type error");
    }
    {
        auto result = FunctionManager::getReturnType("now", {});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result = FunctionManager::getReturnType("now", {Value::Type::BOOL});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Parameter's type error");
    }
    {
        auto result = FunctionManager::getReturnType("rand32", {});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result = FunctionManager::getReturnType("rand32", {Value::Type::INT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result =
            FunctionManager::getReturnType("rand32", {Value::Type::INT, Value::Type::INT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result =
            FunctionManager::getReturnType("rand32", {Value::Type::INT, Value::Type::FLOAT});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Parameter's type error");
    }
    {
        auto result = FunctionManager::getReturnType("sqrt", {Value::Type::INT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::FLOAT);
    }
    {
        auto result = FunctionManager::getReturnType("sqrt", {Value::Type::FLOAT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::FLOAT);
    }
    {
        auto result = FunctionManager::getReturnType("floor", {Value::Type::FLOAT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result = FunctionManager::getReturnType("floor", {Value::Type::INT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result = FunctionManager::getReturnType("floor", {Value::Type::STRING});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Parameter's type error");
    }
    {
        auto result = FunctionManager::getReturnType("round", {Value::Type::INT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result = FunctionManager::getReturnType("round", {Value::Type::FLOAT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result = FunctionManager::getReturnType("cbrt", {Value::Type::INT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result = FunctionManager::getReturnType("cbrt", {Value::Type::FLOAT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::FLOAT);
    }
    {
        auto result = FunctionManager::getReturnType("pow", {Value::Type::INT, Value::Type::INT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result = FunctionManager::getReturnType("pow", {Value::Type::INT, Value::Type::FLOAT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::FLOAT);
    }
    {
        auto result = FunctionManager::getReturnType("pow", {Value::Type::FLOAT, Value::Type::INT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::FLOAT);
    }
    {
        auto result =
            FunctionManager::getReturnType("pow", {Value::Type::FLOAT, Value::Type::FLOAT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::FLOAT);
    }
    {
        auto result =
            FunctionManager::getReturnType("cos", {Value::Type::FLOAT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::FLOAT);
    }
    {
        auto result =
            FunctionManager::getReturnType("sin", {Value::Type::INT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::FLOAT);
    }
    {
        auto result =
            FunctionManager::getReturnType("asin", {Value::Type::INT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::FLOAT);
    }
    {
        auto result =
            FunctionManager::getReturnType("acos", {Value::Type::FLOAT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::FLOAT);
    }
    {
        auto result =
            FunctionManager::getReturnType("acos", {Value::Type::STRING});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Parameter's type error");
    }
    {
        auto result =
            FunctionManager::getReturnType("hypot", {Value::Type::FLOAT, Value::Type::INT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::FLOAT);
    }
    {
        auto result = FunctionManager::getReturnType("hypot", {Value::Type::INT, Value::Type::INT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result =
            FunctionManager::getReturnType("hypot", {Value::Type::BOOL, Value::Type::INT});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Parameter's type error");
    }
    {
        auto result = FunctionManager::getReturnType("lower", {Value::Type::STRING});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::STRING);
    }
    {
        auto result = FunctionManager::getReturnType("upper", {Value::Type::STRING});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::STRING);
    }
    {
        auto result = FunctionManager::getReturnType("upper", {Value::Type::BOOL});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Parameter's type error");
    }
    {
        auto result = FunctionManager::getReturnType("length", {Value::Type::STRING});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result = FunctionManager::getReturnType("length", {});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Parameter's type error");
    }
    {
        auto result =
            FunctionManager::getReturnType("length", {Value::Type::STRING, Value::Type::INT});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Parameter's type error");
    }
    {
        auto result = FunctionManager::getReturnType("length", {Value::Type::FLOAT});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Parameter's type error");
    }
    {
        auto result = FunctionManager::getReturnType("trim", {Value::Type::STRING});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::STRING);
    }
    {
        auto result = FunctionManager::getReturnType("ltrim", {Value::Type::STRING});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::STRING);
    }
    {
        auto result = FunctionManager::getReturnType("strcasecmp",
                                                     {Value::Type::STRING, Value::Type::STRING});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::STRING);
    }
    {
        auto result = FunctionManager::getReturnType("strcasecmp", {Value::Type::STRING});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Parameter's type error");
    }
    {
        auto result = FunctionManager::getReturnType(
            "lpad", {Value::Type::STRING, Value::Type::INT, Value::Type::STRING});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::STRING);
    }
    {
        auto result = FunctionManager::getReturnType(
            "lpad", {Value::Type::STRING, Value::Type::INT, Value::Type::INT});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Parameter's type error");
    }
    {
        auto result =
            FunctionManager::getReturnType("lpad", {Value::Type::STRING, Value::Type::INT});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Parameter's type error");
    }
    {
        auto result = FunctionManager::getReturnType("hash", {Value::Type::STRING});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result = FunctionManager::getReturnType("hash", {Value::Type::INT});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result = FunctionManager::getReturnType("hash", {Value::Type::NULLVALUE});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result = FunctionManager::getReturnType("hash", {Value::Type::__EMPTY__});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result = FunctionManager::getReturnType("hash", {Value::Type::DATE});
        ASSERT_TRUE(result.ok());
        EXPECT_EQ(result.value(), Value::Type::INT);
    }
    {
        auto result =
            FunctionManager::getReturnType("hash", {Value::Type::STRING, Value::Type::INT});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Parameter's type error");
    }
    {
        auto result = FunctionManager::getReturnType("hash", {});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Parameter's type error");
    }
    {
        auto result = FunctionManager::getReturnType("hash", {Value::Type::DATASET});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Parameter's type error");
    }
    {
        auto result =
            FunctionManager::getReturnType("noexist", {Value::Type::INT});
        ASSERT_FALSE(result.ok());
        EXPECT_EQ(result.status().toString(), "Function `noexist' not defined");
    }
}

}   // namespace nebula

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);
    return RUN_ALL_TESTS();
}
