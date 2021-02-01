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

TEST(Edge, hashEdge) {
    std::unordered_set<size_t> uniqueEdge;
    for (size_t i = 0; i < 20; ++i) {
        Edge edge(static_cast<int64_t>(i), static_cast<int64_t>(i + 1), 1, "like", 0, {});
        uniqueEdge.emplace(std::hash<nebula::Edge>()(edge));
    }
    for (size_t i = 0; i < 20; ++i) {
        Edge edge(static_cast<int64_t>(i + 1), static_cast<int64_t>(i), -1, "like", 0, {});
        uniqueEdge.emplace(std::hash<nebula::Edge>()(edge));
    }
    EXPECT_EQ(uniqueEdge.size(), 20);
}


}  // namespace nebula
