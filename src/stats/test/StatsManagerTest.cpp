/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <gtest/gtest.h>
#include "base/Base.h"
#include "stats/StatsManager.h"
#include "thread/GenericWorker.h"

namespace nebula {
namespace stats {

TEST(StatsManager, StatsTest) {
    auto statId = StatsManager::registerStats("stat01.sum.5");
    ASSERT_TRUE(StatsManager::statsIndex(statId));
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([statId, i]() {
            for (int k = i * 10 + 1; k <= i * 10 + 10; k++) {
                sleep(6);
                StatsManager::addValue(statId, k);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Let's check
    EXPECT_EQ(550, StatsManager::readValue("stat01.sum.5").value());
}

TEST(StatsManager, HistogramTest) {
    auto statId = StatsManager::registerHisto("stat02.sum.5", 1, 1, 100);
    ASSERT_TRUE(StatsManager::histogramIndex(statId));
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([statId, i]() {
            for (int k = i * 10 + 1; k <= i * 10 + 10; k++) {
                sleep(6);
                StatsManager::addValue(statId, k);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Let's check
    EXPECT_EQ(550, StatsManager::readValue("stat02.sum.5").value());
}

TEST(StatsManager, Invalid) {
    auto statId = StatsManager::registerStats("..");
    ASSERT_EQ(statId, 0);

    statId = StatsManager::registerStats("stats02..");
    ASSERT_EQ(statId, 0);

    statId = StatsManager::registerStats("stats02.avg.");
    ASSERT_EQ(statId, 0);

    statId = StatsManager::registerStats("stats02.xxx.60");
    ASSERT_EQ(statId, 0);

    statId = StatsManager::registerStats("stats02.p99.60");
    ASSERT_EQ(statId, 0);

    statId = StatsManager::registerHisto("stat02.", 1, 1, 100);
    ASSERT_EQ(statId, 0);
}

}   // namespace stats
}   // namespace nebula

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);

    return RUN_ALL_TESTS();
}
