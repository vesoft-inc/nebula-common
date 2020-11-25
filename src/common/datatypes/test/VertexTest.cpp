/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <folly/init/Init.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include <algorithm>

#include "common/datatypes/Vertex.h"
#include "common/datatypes/test/DataTest.h"

class VertexTest : public DataTest {};

TEST_F(VertexTest, Basic) {
    nebula::Vertex v{"vid",
                     {nebula::Tag{"tag1", {{"prop1", 1}, {"prop2", true}}},
                      nebula::Tag{"tag2", {{"prop3", "prop"}, {"prop4", 3.14}}}}};

    // tag names
    EXPECT_EQ(std::vector<std::string>({"tag1", "tag2"}), v.tagNames());

    // hasTag
    EXPECT_TRUE(v.hasTag("tag1"));
    EXPECT_FALSE(v.hasTag("not_exists_tag"));

    // properties
    std::unordered_map<std::string, nebula::Value> props({{"prop1", 1}, {"prop2", true}});
    EXPECT_EQ(v.properties("tag1"), props);
    std::unordered_map<std::string, nebula::Value> kEmpty;
    EXPECT_EQ(v.properties("not_exists_tag"), kEmpty);

    // keys
    EXPECT_TRUE(
        verifyVectorWithoutOrder(std::vector<std::string>({"prop1", "prop2"}), v.keys("tag1")));
    EXPECT_EQ(std::vector<std::string>(), v.keys("not_exists_tag"));

    // values
    EXPECT_TRUE(verifyVectorWithoutOrder(std::vector<nebula::Value>({1, true}), v.values("tag1")));
    EXPECT_EQ(std::vector<nebula::Value>(), v.values("not_exists_tag"));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);

    return RUN_ALL_TESTS();
}
