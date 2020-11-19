/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <gtest/gtest.h>

#include "common/base/Base.h"
#include "common/datatypes/DataSet.h"

TEST(DataSetTest, Basic) {
    nebula::DataSet data({"col1", "col2", "col3"});
    data.emplace_back(nebula::Row({1, 2, 3}));
    data.emplace_back(nebula::Row({4, 5, 6}));
    data.emplace_back(nebula::Row({7, 8, 9}));

    EXPECT_EQ(std::vector<std::string>({"col1", "col2", "col3"}), data.keys());

    EXPECT_EQ(std::vector<nebula::Value>({4, 5, 6}), data.rowValues(1));

    EXPECT_EQ(std::vector<nebula::Value>({2, 5, 8}), data.colValues("col2"));

    nebula::DataSet data2({"col1", "col2", "col3"});
    for (const auto &it : data) {
        data2.emplace_back(it);
    }
    EXPECT_EQ(data, data2);

    std::vector<std::pair<std::string, nebula::Value>> fields{
        std::make_pair("col1", 1),
        std::make_pair("col2", 2),
        std::make_pair("col3", 3),
        std::make_pair("col1", 4),
        std::make_pair("col2", 5),
        std::make_pair("col3", 6),
        std::make_pair("col1", 7),
        std::make_pair("col2", 8),
        std::make_pair("col3", 9),
    };
    EXPECT_EQ(fields, data.fields());
}


int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);

    return RUN_ALL_TESTS();
}
