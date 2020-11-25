/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/base/Base.h"
#include <gtest/gtest.h>
#include "common/datatypes/Edge.h"
#include "common/datatypes/test/DataTest.h"

class EdgeTest : public DataTest {};

namespace nebula {
TEST_F(EdgeTest, Format) {
    {
        Edge edge("1", "2", -1, "e1", 0, {});
        edge.format();

        Edge expect("2", "1", 1, "e1", 0, {});
        EXPECT_EQ(expect, edge);
    }
    {
        Edge edge("1", "2", 1, "e1", 0, {});
        edge.format();

        Edge expect("1", "2", 1, "e1", 0, {});
        EXPECT_EQ(expect, edge);
    }
}

TEST_F(EdgeTest, Reverse) {
    {
        Edge edge("1", "2", -1, "e1", 0, {});
        edge.reverse();

        Edge expect("2", "1", 1, "e1", 0, {});
        EXPECT_EQ(expect, edge);
    }
    {
        Edge edge("1", "2", 1, "e1", 0, {});
        edge.reverse();

        Edge expect("2", "1", -1, "e1", 0, {});
        EXPECT_EQ(expect, edge);
    }
}

TEST_F(EdgeTest, KeyValues) {
    Edge edge("1", "2", -1, "e1", 0, {{"prop1", 1}, {"prop2", true}});

    // keys
    EXPECT_TRUE(verifyVectorWithoutOrder(edge.keys(),
                std::vector<std::string>({"prop1", "prop2"})));

    // values
    EXPECT_TRUE(verifyVectorWithoutOrder(edge.values(), std::vector<Value>({1, true})));
}

}  // namespace nebula
