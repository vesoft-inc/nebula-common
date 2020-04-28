/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_STATS_STATSMANAGER_H_
#define COMMON_STATS_STATSMANAGER_H_

#include <folly/RWSpinLock.h>
#include <folly/stats/MultiLevelTimeSeries.h>
#include <folly/stats/TimeseriesHistogram.h>
#include <gtest/gtest_prod.h>
#include "base/Base.h"
#include "base/StatusOr.h"
#include "datatypes/HostAddr.h"
#include "time/WallClock.h"

namespace nebula {
namespace stats {

/**
 * This is a utility class to keep track the service's statistic information.
 *
 * It contains a bunch of counters. Each counter has a unique name and three
 * levels of time ranges, which are 1 minute, 10 minues, and 1 hour. Each
 * counter also associates with one or multiple statistic types. The supported
 * statistic types are SUM, COUNT, AVG, RATE, MIN, MAX and percentiles. Among
 * those, MIN, MAX and Percentile are only supported for Histogram counters.
 * The percentile is in the form of "pXXXXX". There could be 2 or more digits
 * after the letter "p", which represents XX.XXXX%.
 *
 * Here is the complete form of a specific counter:
 *   <counter_name>.<statistic_type>.<time_range>
 *
 * Here are some examples:
 *   query.rate.5       -- query per second in the five seconds
 *   query.rate.60      -- query per second in the last minute
 *   latency.p99.600    -- The latency that slower than 99% of all queries
 *                           in the last ten minutes
 *   latency.p9999.60   -- The latency that slower than 99.99% of all queries
 *                           in the last one minute
 *   error.count.600    -- Total number of errors in the last ten minutes
 */
class StatsManager final {
    FRIEND_TEST(StatsManager, StatsTest);
    FRIEND_TEST(StatsManager, HistogramTest);
    using VT = int64_t;
    using StatsType = folly::MultiLevelTimeSeries<VT>;
    using HistogramType = folly::TimeseriesHistogram<VT>;

public:
    enum class StatsMethod { SUM = 1, COUNT, AVG, RATE, PCT };

    static void setDomain(folly::StringPiece domain);

    // addr     -- The ip/port of the stats collector. StatsManager will periodically
    //             report the stats to the collector
    // interval -- The number of seconds between each report
    static void setReportInfo(HostAddr addr, int32_t interval);

    // Both register methods return the index to the internal data structure.
    // This index will be used by addValue() methods.
    // Both register methods is not thread safe, and user need to register stats one
    // by onebefore calling addValue, readStats, readHisto and so on.
    // Note return 0 mean failed, please check it
    static int32_t registerStats(folly::StringPiece counterName);
    static int32_t registerHisto(folly::StringPiece counterName, VT bucketSize, VT min, VT max);

    static void addValue(int32_t index, VT value = 1);

    static StatusOr<VT> readValue(folly::StringPiece counter);

    static folly::dynamic readAllValue();

private:
    static StatsManager& get();

    StatsManager() = default;
    StatsManager(const StatsManager&) = delete;
    StatsManager(StatsManager&&) = delete;

    static StatusOr<VT> readValue(int32_t index, StatsMethod method);
    static StatusOr<VT> readValue(int32_t index, double pct);

    template <class StatsHolder>
    static VT readValue(StatsHolder& stats, uint32_t level, StatsMethod method);

    static bool validIndex(int32_t index) {
        return index != 0;
    }

    static bool statsIndex(int32_t index) {
        return index > 0;
    }

    static bool histogramIndex(int32_t index) {
        return index < 0;
    }

    static std::size_t physicalIndex(int32_t index) {
        CHECK(validIndex(index));
        return index > 0 ? index - 1 : -(index + 1);
    }

    struct MetricSpec {
        StatsMethod method;
        uint32_t timeRange;
        double pct;   // Only when PCT method
    };

    static StatusOr<MetricSpec> parseMetricName(folly::StringPiece name);

    static const MetricSpec& getSpec(int32_t index) {
        const auto& sm = get();
        CHECK(validIndex(index));
        if (statsIndex(index)) {
            return sm.statsSpec_[physicalIndex(index)];
        } else if (histogramIndex(index)) {
            return sm.histogramsSpec_[physicalIndex(index)];
        }
        LOG(FATAL) << "Impossible";
    }

private:
    std::string domain_;
    HostAddr collectorAddr_{0, 0};
    int32_t interval_{0};

    // <counter_name> => index
    // when index > 0, (index - 1) is the index of stats_ list
    // when index < 0, [- (index + 1)] is the index of histograms_ list
    folly::RWSpinLock nameMapLock_;
    std::unordered_map<std::string, int32_t> nameMap_;

    // All time series stats
    std::vector<std::pair<std::unique_ptr<std::mutex>, std::unique_ptr<StatsType>>> stats_;
    std::vector<MetricSpec> statsSpec_;

    // All histogram stats
    std::vector<std::pair<std::unique_ptr<std::mutex>, std::unique_ptr<HistogramType>>> histograms_;
    std::vector<MetricSpec> histogramsSpec_;
};

}   // namespace stats
}   // namespace nebula

#include "stats/StatsManager.inl"

#endif   // COMMON_STATS_STATSMANAGER_H_
