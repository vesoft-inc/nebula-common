/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/base/Base.h"
#include <gtest/gtest.h>
#include "common/datatypes/Edge.h"

namespace nebula {
TEST(Edge, Format) {
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

TEST(Edge, Reverse) {
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
TEST(Edge, FormatIntegerID) {
    {
        Edge edge(1, 2, -1, "e1", 0, {});
        edge.format();

        Edge expect(2, 1, 1, "e1", 0, {});
        EXPECT_EQ(expect, edge);
    }
    {
        Edge edge(1, 2, 1, "e1", 0, {});
        edge.format();

        Edge expect(1, 2, 1, "e1", 0, {});
        EXPECT_EQ(expect, edge);
    }
}

TEST(Edge, ReverseInteger) {
    {
        Edge edge(1, 2, -1, "e1", 0, {});
        edge.reverse();

        Edge expect(2, 1, 1, "e1", 0, {});
        EXPECT_EQ(expect, edge);
    }
    {
        Edge edge(1, 2, 1, "e1", 0, {});
        edge.reverse();

        Edge expect(2, 1, -1, "e1", 0, {});
        EXPECT_EQ(expect, edge);
    }
}

TEST(Edge, edgeKey) {
    {
        Edge edge(1, 2, -1, "e1", 0, {});
        auto edgeKey = edge.edgeKey();
        auto expect = "211110";
        EXPECT_EQ(expect, edgeKey);
    }
    {
        Edge edge("Tim Duncan", "Tony Parker", 1, "e1", 0, {});
        auto edgeKey = edge.edgeKey();
        auto expect = "Tim Duncan10Tony Parker1110";
        EXPECT_EQ(expect, edgeKey);
    }
    {
        Edge edge("Tim Duncan", "Tony Parker", -1, "e1", 0, {});
        auto edgeKey = edge.edgeKey();
        auto expect = "Tony Parker11Tim Duncan1010";
        EXPECT_EQ(expect, edgeKey);
    }
}

}  // namespace nebula
