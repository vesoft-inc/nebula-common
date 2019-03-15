/* Copyright (c) 2018 - present, VE Software Inc. All rights reserved
 *
 * This source code is licensed under Apache 2.0 License
 *  (found in the LICENSE.Apache file in the root directory)
 */

#include "base/Base.h"
#include <gtest/gtest.h>
#include <folly/json.h>
#include "webservice/WebService.h"
#include "process/ProcessUtils.h"
#include "stats/StatsManager.h"

DECLARE_int32(ws_http_port);
DECLARE_string(ws_ip);


namespace nebula {

using nebula::stats::StatsManager;

class StatsReaderTestEnv : public ::testing::Environment {
public:
    void SetUp() override {
        VLOG(1) << "Starting web service...";
        WebService::start();
    }

    void TearDown() override {
        WebService::stop();
        VLOG(1) << "Web service stopped";
    }
};

bool getUrl(const std::string& urlPath, std::string& respBody) {
    auto url = folly::stringPrintf("http://%s:%d%s",
                                   FLAGS_ws_ip.c_str(),
                                   FLAGS_ws_http_port,
                                   urlPath.c_str());
    VLOG(1) << "Retrieving url: " << url;

    auto command = folly::stringPrintf("/usr/bin/curl -G \"%s\" 2> /dev/null",
                                       url.c_str());
    auto status = ProcessUtils::runCommand(command.c_str());
    if (!status.ok()) {
        LOG(ERROR) << "Failed to run curl: " << status.status();
        return false;
    }
    respBody = status.value();
    return true;
}


TEST(StatsReaderTest, GetStatsTest) {
    auto statId = StatsManager::registerStats("stat01");
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([statId, i] () {
            for (int k = i * 10 + 1; k <= i * 10 + 10; k++) {
                StatsManager::addValue(statId, k);
             }
        });
    }

    for (auto& t : threads)  {
        t.join();
    }

    {
        int64_t statSum = StatsManager::readValue("stat01.sum.60");
        EXPECT_EQ(5050, statSum);
        std::string resp1;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat01.sum.60", resp1));
        EXPECT_EQ(folly::stringPrintf("stat01.sum.60=%ld\n", statSum), resp1);

        statSum = 0;
        statSum = StatsManager::readValue("stat01.sum.600");
        EXPECT_EQ(5050, statSum);
        std::string resp2;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat01.sum.600", resp2));
        EXPECT_EQ(folly::stringPrintf("stat01.sum.600=%ld\n", statSum), resp2);

        statSum = 0;
        statSum = StatsManager::readValue("stat01.sum.3600");
        EXPECT_EQ(5050, statSum);
        std::string resp3;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat01.sum.3600", resp3));
        EXPECT_EQ(folly::stringPrintf("stat01.sum.3600=%ld\n", statSum), resp3);
    }

    {
        int64_t statCount = StatsManager::readValue("stat01.count.60");
        EXPECT_EQ(100, statCount);
        std::string resp1;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat01.count.60", resp1));
        EXPECT_EQ(folly::stringPrintf("stat01.count.60=%ld\n", statCount), resp1);

        statCount = 0;
        statCount = StatsManager::readValue("stat01.count.600");
        EXPECT_EQ(100, statCount);
        std::string resp2;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat01.count.600", resp2));
        EXPECT_EQ(folly::stringPrintf("stat01.count.600=%ld\n", statCount), resp2);

        statCount = 0;
        statCount = StatsManager::readValue("stat01.count.3600");
        EXPECT_EQ(100, statCount);
        std::string resp3;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat01.count.3600", resp3));
        EXPECT_EQ(folly::stringPrintf("stat01.count.3600=%ld\n", statCount), resp3);
    }

    {
        // get multi stats
        std::string resp;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat01.sum.60,stat01.avg.60,stat01.count.60", resp));
        EXPECT_EQ(std::string("stat01.sum.60=5050\nstat01.avg.60=50\nstat01.count.60=100\n"), resp);
    }

    {
        // return json
        std::string resp;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat01.sum.60&returnjson", resp));
        auto json = folly::parseJson(resp);
        ASSERT_TRUE(json.isArray());
        ASSERT_EQ(1UL, json.size());
        ASSERT_TRUE(json[0].isObject());
        ASSERT_EQ(2UL, json[0].size());

        auto it = json[0].find("name");
        ASSERT_NE(json[0].items().end(), it);
        ASSERT_TRUE(it->second.isString());
        EXPECT_EQ("stat01.sum.60", it->second.getString());

        it = json[0].find("value");
        ASSERT_NE(json[0].items().end(), it);
        ASSERT_TRUE(it->second.isInt());
        EXPECT_EQ(5050, it->second.getInt());
    }

    {
        // get all stats(sum,count,avg,rate)
        std::string resp;
        ASSERT_TRUE(getUrl("/get_stats?stats= ", resp));
        EXPECT_FALSE(resp.empty());
    }

    {
        // Error test
        std::string resp;
        ASSERT_TRUE(getUrl("/get_stats123?stats=stat01.sum.60", resp));
        EXPECT_TRUE(resp.empty());
    }
}

TEST(StatsReaderTest, GetHistoTest) {
    auto statId = StatsManager::registerHisto("stat02", 1, 1, 100);
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([statId, i] () {
            for (int k = i * 10 + 1; k <= i * 10 + 10; k++) {
                StatsManager::addValue(statId, k);
            }
        });
    }

    for (auto& t : threads)  {
        t.join();
    }

    {
        int64_t statAvg = StatsManager::readValue("stat02.avg.60");
        EXPECT_EQ(50, statAvg);
        std::string resp1;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat02.avg.60", resp1));
        EXPECT_EQ(folly::stringPrintf("stat02.avg.60=%ld\n", statAvg), resp1);

        statAvg = 0;
        statAvg = StatsManager::readValue("stat02.Avg.600");
        EXPECT_EQ(50, statAvg);
        std::string resp2;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat02.Avg.600", resp2));
        EXPECT_EQ(folly::stringPrintf("stat02.Avg.600=%ld\n", statAvg), resp2);

        statAvg = 0;
        statAvg = StatsManager::readValue("stat02.AVG.3600");
        EXPECT_EQ(50, statAvg);
        std::string resp3;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat02.AVG.3600", resp3));
        EXPECT_EQ(folly::stringPrintf("stat02.AVG.3600=%ld\n", statAvg), resp3);
    }

    {
        int64_t statP = StatsManager::readValue("stat02.p99.60");
        EXPECT_EQ(100, statP);
        std::string resp1;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat02.p99.60", resp1));
        EXPECT_EQ(folly::stringPrintf("stat02.p99.60=%ld\n", statP), resp1);

        statP = 0;
        statP = StatsManager::readValue("stat02.P99.600");
        EXPECT_EQ(100, statP);
        std::string resp2;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat02.P99.600", resp2));
        EXPECT_EQ(folly::stringPrintf("stat02.P99.600=%ld\n", statP), resp2);

        statP = 0;
        statP = StatsManager::readValue("stat02.t99.3600");
        EXPECT_EQ(0, statP);
        std::string resp3;
        ASSERT_TRUE(getUrl("/get_stats?stats=stat02.t99.3600", resp3));
        EXPECT_EQ(folly::stringPrintf("stat02.t99.3600=%ld\n", statP), resp3);
    }
}

}  // namespace nebula


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);

    ::testing::AddGlobalTestEnvironment(new nebula::StatsReaderTestEnv());

    return RUN_ALL_TESTS();
}

