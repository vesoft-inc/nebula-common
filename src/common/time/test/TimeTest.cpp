/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <gtest/gtest.h>
#include <folly/Random.h>

#include "common/time/Time.h"

namespace nebula {

TEST(Time, secondsTimeCovertion) {
    // DateTime
    {
        std::vector<DateTime> values;
        // TODO(shylock) support before Epoch
        // values.emplace_back(DateTime(1969, 1, 1, 0, 0, 0, 0));
        for (std::size_t i = 0; i < 4; ++i) {
            values.emplace_back(DateTime(
                                    folly::Random::rand32(1970,
                                                          std::numeric_limits<int16_t>::max()),
                                    folly::Random::rand32(1, 13),
                                    folly::Random::rand32(1, 29),
                                    folly::Random::rand32(0, 24),
                                    folly::Random::rand32(0, 60),
                                    folly::Random::rand32(0, 60),
                                    0));
        }
        for (const auto &dt : values) {
            EXPECT_EQ(dt, time::secondsToDateTime(time::dateTimeToSeconds(dt)));
        }
    }
    // Date
    {
        std::vector<Date> values;
        // TODO(shylock) support before Epoch
        // values.emplace_back(DateTime(1969, 1, 1));
        for (std::size_t i = 0; i < 4; ++i) {
            values.emplace_back(Date(
                                    folly::Random::rand32(1970,
                                                          std::numeric_limits<int16_t>::max()),
                                    folly::Random::rand32(1, 13),
                                    folly::Random::rand32(1, 29)));
        }
        for (const auto &d : values) {
            EXPECT_EQ(d, time::secondsToDate(time::dateToSeconds(d)));
        }
    }
    // Time
    {
        std::vector<Time> values;
        for (std::size_t i = 0; i < 4; ++i) {
            values.emplace_back(Time(folly::Random::rand32(0, 24),
                                         folly::Random::rand32(0, 60),
                                         folly::Random::rand32(0, 60),
                                         0));
        }
        for (const auto &t : values) {
            EXPECT_EQ(t, time::secondsToTime(time::timeToSeconds(t)));
        }
    }
}

}  // namespace nebula

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);

    return RUN_ALL_TESTS();
}
