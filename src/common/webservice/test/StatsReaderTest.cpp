/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/base/Base.h"
#include <gtest/gtest.h>
#include <folly/json.h>
#include "common/webservice/WebService.h"
#include "common/stats/StatsManager.h"
#include "common/webservice/test/TestUtils.h"

namespace nebula {

using nebula::stats::StatsManager;

class StatsReaderTestEnv : public ::testing::Environment {
public:
    void SetUp() override {
        FLAGS_ws_http_port = 0;
        FLAGS_ws_h2_port = 0;
        VLOG(1) << "Starting web service...";
        webSvc_ = std::make_unique<WebService>();
        auto status = webSvc_->start();
        ASSERT_TRUE(status.ok()) << status;
    }

    void TearDown() override {
        webSvc_.reset();
        VLOG(1) << "Web service stopped";
    }

private:
    std::unique_ptr<WebService> webSvc_;
};

TEST(StatsReaderTest, GetStatsTest) {
    auto statId = StatsManager::registerStats("stat01.sum.60");
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([statId, i]() {
            for (int k = i * 10 + 1; k <= i * 10 + 10; k++) {
                StatsManager::addValue(statId, k);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto ret = StatsManager::readValue("stat01.sum.60");
    ASSERT_TRUE(ret.ok());
    auto value = ret.value();

    {
        EXPECT_EQ(5050, value);
        std::string resp1;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat01.sum.60", resp1));
        EXPECT_EQ(folly::stringPrintf("stat01.sum.60=%ld\n", value), resp1);
    }

    {
        // Invalid counter
        std::string resp1;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat01.count.60", resp1));
        EXPECT_EQ("stat01.count.60=stat01.count.60 isn't registered\n", resp1);
    }

    {
        // return json
        std::string resp;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat01.sum.60&returnjson", resp));
        auto json = folly::parseJson(resp);
        ASSERT_TRUE(json.isObject());
        EXPECT_EQ(value, json["stat01.sum.60"].asInt());
    }

    {
        std::string resp;
        ASSERT_TRUE(getUrl("/get_stats?&returnjson", resp));
        auto json = folly::parseJson(resp);
        ASSERT_TRUE(json.isObject());
        EXPECT_EQ(value, json["stat01.sum.60"].asInt());
    }

    {
        // get all stats(sum,count,avg,rate)
        std::string resp;
        ASSERT_TRUE(getUrl("/get_stats?stats= ", resp));
        EXPECT_EQ(folly::stringPrintf("stat01.sum.60=%ld\n", value), resp);
    }

    {
        // Error test
        std::string resp;
        ASSERT_TRUE(getUrl("/get_stats123?stats=stat01.sum.60", resp));
        EXPECT_TRUE(resp.empty());
    }
}   // namespace nebula

TEST(StatsReaderTest, GetHistoTest) {
    auto statId = StatsManager::registerHisto("stat02.p99.60", 1, 1, 100);
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([statId, i]() {
            for (int k = i * 10 + 1; k <= i * 10 + 10; k++) {
                StatsManager::addValue(statId, k);
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    {
        auto status = StatsManager::readValue("stat02.p99.60");
        ASSERT_TRUE(status.ok()) << status.status().toString();
        auto statP = status.value();
        EXPECT_EQ(100, statP);
        std::string resp1;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat02.p99.60", resp1));
        EXPECT_EQ(folly::stringPrintf("stat02.p99.60=%ld\n", statP), resp1);

        std::string resp2;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat02.p99.60&returnjson", resp2));
        auto json = folly::parseJson(resp2);
        ASSERT_TRUE(json.isObject());
        ASSERT_EQ(json["stat02.p99.60"].asInt(), statP);

        std::string resp3;
        ASSERT_TRUE(getUrl("/get_stats?returnjson", resp3));
        auto json2 = folly::parseJson(resp3);
        ASSERT_TRUE(json2.isObject());
        ASSERT_EQ(json2["stat02.p99.60"].asInt(), statP);

        EXPECT_FALSE(StatsManager::readValue("stat02.t99.3600").ok());
        std::string resp4;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat02.t99.3600", resp4));
        EXPECT_EQ("stat02.t99.3600=stat02.t99.3600 isn't registered\n", resp4);
    }
}

}   // namespace nebula

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);

    ::testing::AddGlobalTestEnvironment(new nebula::StatsReaderTestEnv());

    return RUN_ALL_TESTS();
}
