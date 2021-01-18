/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/base/Base.h"
#include <gtest/gtest.h>
#include "common/stats/StatsManager.h"
#include "common/thread/GenericWorker.h"

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
    EXPECT_FALSE(StatsManager::readValue("stat01.sum.60").ok());
    EXPECT_FALSE(StatsManager::readValue("stat01.SUM.600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat01.Sum.3600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat01.Sum.300").ok());
    EXPECT_FALSE(StatsManager::readValue("stat01.count.5").ok());
    EXPECT_FALSE(StatsManager::readValue("stat01.count.60").ok());
    EXPECT_FALSE(StatsManager::readValue("stat01.COUNT.600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat01.Count.3600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat01.Count.4600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat01.avg.5").ok());
    EXPECT_FALSE(StatsManager::readValue("stat01.avg.60").ok());
    EXPECT_FALSE(StatsManager::readValue("stat01.AVG.600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat01.Avg.3600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat01.Avg1.3600").ok());
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
    EXPECT_FALSE(StatsManager::readValue("stat02.sum.60").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.SUM.600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.Sum.3600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.Sum.300").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.count.5").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.count.60").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.COUNT.600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.Count.3600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.Count.4600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.avg.5").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.avg.60").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.AVG.600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.Avg.3600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.Avg1.3600").ok());

    EXPECT_FALSE(StatsManager::readValue("stat02.p99.60").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.P99.600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.p99.3600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.t99.60").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.p9599.60").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.P9599.600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.p9599.3600").ok());
    EXPECT_FALSE(StatsManager::readValue("stat02.t9599.60").ok());
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
