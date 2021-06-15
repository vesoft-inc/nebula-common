/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/base/Base.h"
#include <gtest/gtest.h>
#include "common/base/Status.h"


namespace nebula {

TEST(Status, Basic) {
    ASSERT_TRUE(Status().ok());
    ASSERT_TRUE(Status::OK().ok());
    ASSERT_FALSE(Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), "Error").ok());
    ASSERT_EQ(8UL, sizeof(Status));
}


TEST(Status, toString) {
    ASSERT_EQ("SUCCEEDED", Status().toString());
    ASSERT_EQ("SUCCEEDED", Status::OK().toString());
    ASSERT_EQ("-1:Internal error: Error.",
              Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), "Error").toString());
    ASSERT_EQ("-1:Internal error: SomeError.",
              Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), "SomeError").toString());
    ASSERT_EQ("-1:Internal error: message.",
              Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), "message").toString());
}


TEST(Status, StreamOperator) {
    {
        auto result = testing::AssertionSuccess();
        result << Status::OK();
        ASSERT_STREQ("SUCCEEDED", result.message());
    }
    {
        auto result = testing::AssertionSuccess();
        result << Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), "SomeError");
        ASSERT_STREQ("-1:Internal error: SomeError.", result.message());
    }
    {
        std::ostringstream os;
        os << Status();
        ASSERT_EQ("SUCCEEDED", os.str());
    }
    {
        std::ostringstream os;
        os << Status::OK();
        ASSERT_EQ("SUCCEEDED", os.str());
    }
    {
        std::ostringstream os;
        os << Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), "SomeError");
        ASSERT_EQ("-1:Internal error: SomeError.", os.str());
    }
}


TEST(Status, Copy) {
    {
        Status ok;
        auto copy = ok;
        ASSERT_TRUE(ok.ok());
        ASSERT_TRUE(copy.ok());
    }
    {
        auto error = Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), "SomeError");
        auto copy = error;
        ASSERT_FALSE(error.ok());
        ASSERT_FALSE(copy.ok());
        ASSERT_EQ("-1:Internal error: SomeError.", error.toString());
        ASSERT_EQ("-1:Internal error: SomeError.", copy.toString());
    }
}


TEST(Status, Move) {
    {
        Status ok;
        auto move = std::move(ok);
        ASSERT_TRUE(ok.ok());
        ASSERT_TRUE(move.ok());
    }
    {
        auto error = Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), "SomeError");
        ASSERT_FALSE(error.ok());
        ASSERT_EQ("-1:Internal error: SomeError.", error.toString());
        auto move = std::move(error);
        ASSERT_TRUE(error.ok());
        ASSERT_EQ("SUCCEEDED", error.toString());
        ASSERT_FALSE(move.ok());
        ASSERT_EQ("-1:Internal error: SomeError.", move.toString());
    }
}

TEST(Status, ReturnIfError) {
    auto testReturnIfError = []() {
        NG_RETURN_IF_ERROR(Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), "error"));
        return Status::OK();
    };
    auto testReturnOK = []() {
        NG_RETURN_IF_ERROR(Status::OK());
        return Status::OK();
    };
    EXPECT_FALSE(testReturnIfError().ok());
    EXPECT_TRUE(testReturnOK().ok());
}

TEST(Status, Message) {
    Status err = Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), "error");
    EXPECT_EQ(err.message(), "Internal error: error.");
    EXPECT_EQ("Internal error: some reason.",
              Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), "some reason").message());
    EXPECT_EQ("SUCCEEDED", Status::OK().message());
    std::string msg;
    {
        msg = Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), "reason").message();
    }
    EXPECT_EQ(msg, "Internal error: reason.");
}

}   // namespace nebula
