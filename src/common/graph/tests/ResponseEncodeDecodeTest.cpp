/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <gtest/gtest.h>
#include <thrift/lib/cpp2/protocol/Serializer.h>

#include "common/graph/Response.h"

namespace nebula {

using serializer = apache::thrift::CompactSerializer;

TEST(ResponseEncodDecodeTest, Basic) {
    // auth response
    {
        AuthResponse resp {};
        std::string buf;
        buf.reserve(128);
        serializer::serialize(resp, &buf);
        AuthResponse copy;
        std::size_t s = serializer::deserialize(buf, copy);
        ASSERT_EQ(s, buf.size());
        EXPECT_EQ(resp, copy);
    }
    // response
    {
        ExecutionResponse resp {};
        std::string buf;
        buf.reserve(128);
        serializer::serialize(resp, &buf);
        ExecutionResponse copy;
        std::size_t s = serializer::deserialize(buf, copy);
        ASSERT_EQ(s, buf.size());
        EXPECT_EQ(resp, copy);
    }
}

}   // namespace nebula

