/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "stats/StatsManager.h"
#include <folly/stats/MultiLevelTimeSeries-defs.h>
#include <folly/stats/TimeseriesHistogram-defs.h>
#include "base/Base.h"

namespace nebula {
namespace stats {

DEFINE_bool(enable_monitor, true, "Enable collect the metrics");

// static
StatsManager& StatsManager::get() {
    static StatsManager sm;
    return sm;
}

// static
void StatsManager::setDomain(folly::StringPiece domain) {
    get().domain_ = domain.toString();
}

// static
void StatsManager::setReportInfo(HostAddr addr, int32_t interval) {
    auto& sm = get();
    sm.collectorAddr_ = addr;
    sm.interval_ = interval;
}

// static
int32_t StatsManager::registerStats(folly::StringPiece counterName) {
    if (!FLAGS_enable_monitor) {
        return kInvalidIndex;
    }
    auto retName = parseMetricName(counterName);
    if (!retName.ok()) {
        return kInvalidIndex;
    }
    auto spec = std::move(retName).value();
    if (spec.method == StatsMethod::PCT) {
        // Not supported method PCT for stats
        return kInvalidIndex;
    }
    auto name = counterName.toString();

    using std::chrono::seconds;

    auto& sm = get();

    folly::RWSpinLock::WriteHolder wh(sm.nameMapLock_);
    auto it = sm.nameMap_.find(name);
    if (it != sm.nameMap_.end()) {
        VLOG(2) << "The counter \"" << name << "\" already exists";
        return it->second;
    }

    // Insert the Stats
    sm.stats_.emplace_back(std::make_pair(
        std::make_unique<std::mutex>(),
        std::make_unique<StatsType>(
            60, std::initializer_list<StatsType::Duration>({seconds(spec.timeRange)}))));
    sm.statsSpec_.emplace_back(spec);
    CHECK_EQ(sm.stats_.size(), sm.statsSpec_.size());
    int32_t index = sm.stats_.size();
    auto ret = sm.nameMap_.emplace(std::move(name), index);
    if (LIKELY(ret.second)) {
        return index;
    } else {
        return kInvalidIndex;
    }
}

// static
int32_t StatsManager::registerHisto(folly::StringPiece counterName,
                                    StatsManager::VT bucketSize,
                                    StatsManager::VT min,
                                    StatsManager::VT max) {
    if (!FLAGS_enable_monitor) {
        return kInvalidIndex;
    }
    auto retName = parseMetricName(counterName);
    if (!retName.ok()) {
        return kInvalidIndex;
    }
    auto spec = std::move(retName).value();
    auto name = counterName.toString();

    using std::chrono::seconds;

    auto& sm = get();
    folly::RWSpinLock::WriteHolder wh(sm.nameMapLock_);
    auto it = sm.nameMap_.find(name);
    if (it != sm.nameMap_.end()) {
        VLOG(2) << "The counter \"" << name << "\" already exists";
        return it->second;
    }

    // Insert the Histogram
    sm.histograms_.emplace_back(
        std::make_pair(std::make_unique<std::mutex>(),
                       std::make_unique<HistogramType>(
                           bucketSize, min, max, StatsType(60, {seconds(spec.timeRange)}))));
    sm.histogramsSpec_.emplace_back(spec);
    CHECK_EQ(sm.histograms_.size(), sm.histogramsSpec_.size());
    int32_t index = -sm.histograms_.size();
    auto ret = sm.nameMap_.emplace(std::move(name), index);
    if (LIKELY(ret.second)) {
        VLOG(2) << "registerHisto, bucketSize: " << bucketSize << ", min: " << min
                << ", max: " << max;
        return index;
    } else {
        return kInvalidIndex;
    }
}

// static
void StatsManager::addValue(int32_t index, VT value) {
    if (!FLAGS_enable_monitor) {
        return;
    }
    using std::chrono::seconds;
    CHECK(validIndex(index));

    auto& sm = get();
    if (statsIndex(index)) {
        // Stats
        auto rIndex = physicalIndex(index);
        DCHECK_LT(rIndex, sm.stats_.size());
        std::lock_guard<std::mutex> g(*(sm.stats_[rIndex].first));
        sm.stats_[rIndex].second->addValue(seconds(time::WallClock::fastNowInSec()), value);
    } else if (histogramIndex(index)) {
        // Histogram
        auto rIndex = physicalIndex(index);
        DCHECK_LT(rIndex, sm.histograms_.size());
        std::lock_guard<std::mutex> g(*(sm.histograms_[rIndex].first));
        sm.histograms_[rIndex].second->addValue(seconds(time::WallClock::fastNowInSec()), value);
    }
}

// static
StatusOr<StatsManager::VT> StatsManager::readValue(folly::StringPiece metricName) {
    if (!FLAGS_enable_monitor) {
        return Status::NotSupported("Disabled");
    }
    int32_t index = 0;
    {
        auto& sm = get();
        folly::RWSpinLock::ReadHolder r(sm.nameMapLock_);
        const auto fd = sm.nameMap_.find(metricName.toString());
        if (fd == sm.nameMap_.end()) {
            return Status::Error("%s isn't registered", metricName.data());
        }
        index = fd->second;
    }

    const auto& spec = getSpec(index);
    switch (spec.method) {
        case StatsMethod::PCT:
            /* code */
            return readValue(index, spec.pct);
        case StatsMethod::SUM:
            // fallthrough
        case StatsMethod::AVG:
            // fallthrough
        case StatsMethod::RATE:
            // fallthrough
        case StatsMethod::COUNT:
            return readValue(index, spec.method);
    }
}

/*static*/ StatusOr<StatsManager::VT> StatsManager::readValue(int32_t index, StatsMethod method) {
    if (!FLAGS_enable_monitor) {
        return Status::NotSupported("Disabled");
    }
    using std::chrono::seconds;
    auto& sm = get();

    if (UNLIKELY(!validIndex(index))) {
        return Status::Error("Invalid index %d", index);
    } else if (statsIndex(index)) {
        // stats
        auto rIndex = physicalIndex(index);
        DCHECK_LT(rIndex, sm.stats_.size());
        if (rIndex >= sm.stats_.size()) {
            return Status::Error("Out of range index %d", index);
        }
        std::lock_guard<std::mutex> g(*(sm.stats_[rIndex].first));
        sm.stats_[rIndex].second->update(seconds(time::WallClock::fastNowInSec()));
        return readValue(*(sm.stats_[rIndex].second), 0, method);
    } else if (histogramIndex(index)) {
        // histograms_
        auto rIndex = physicalIndex(index);
        DCHECK_LT(rIndex, sm.histograms_.size());
        if (rIndex >= sm.histograms_.size()) {
            return Status::Error("Out of range index %d", index);
        }
        std::lock_guard<std::mutex> g(*(sm.histograms_[rIndex].first));
        sm.histograms_[rIndex].second->update(seconds(time::WallClock::fastNowInSec()));
        return readValue(*(sm.histograms_[rIndex].second), 0, method);
    }
    LOG(FATAL) << "Impossible";
}

/*static*/ StatusOr<StatsManager::VT> StatsManager::readValue(int32_t index, double pct) {
    if (!FLAGS_enable_monitor) {
        return Status::NotSupported("Disabled");
    }
    using std::chrono::seconds;
    auto& sm = get();
    if (!histogramIndex(index)) {
        return Status::NotSupported("Not a histogram");
    }
    auto rIndex = physicalIndex(index);
    if (rIndex >= sm.histograms_.size()) {
        return Status::Error("Out of range");
    }

    std::lock_guard<std::mutex> g(*(sm.histograms_[rIndex].first));
    sm.histograms_[rIndex].second->update(seconds(time::WallClock::fastNowInSec()));
    return sm.histograms_[rIndex].second->getPercentileEstimate(pct, 0);
}

// static
folly::dynamic StatsManager::readAllValue() {
    auto& sm = get();
    folly::dynamic vals = folly::dynamic::object;
    if (!FLAGS_enable_monitor) {
        vals["metrics"] = "Disabled";
        return vals;
    }

    folly::RWSpinLock::ReadHolder rh(sm.nameMapLock_);
    for (const auto& statsName : sm.nameMap_) {
        const auto& spec = getSpec(statsName.second);
        StatusOr<StatsManager::VT> metricValue;
        switch (spec.method) {
            case StatsMethod::PCT:
                /* code */
                metricValue = readValue(statsName.second, spec.pct);
                break;
            case StatsMethod::SUM:
                // fallthrough
            case StatsMethod::AVG:
                // fallthrough
            case StatsMethod::RATE:
                // fallthrough
            case StatsMethod::COUNT:
                metricValue = readValue(statsName.second, spec.method);
                break;
            default:
                LOG(FATAL) << "Impossible";
        }
        if (metricValue.ok()) {
            vals[statsName.first] = metricValue.value();
        } else {
            vals[statsName.first] = metricValue.status().toString();
        }
    }
    return vals;
}

/*static*/ StatusOr<StatsManager::MetricSpec> StatsManager::parseMetricName(
    folly::StringPiece name) {
    std::vector<std::string> parts;
    folly::split(".", name, parts, true);
    if (parts.size() != 3) {
        return Status::Error("Invalid metric name format");
    }

    StatsMethod method;
    double pct = 0;
    // Now check the statistic method
    static int32_t dividors[] = {1, 1, 10, 100, 1000, 10000};
    folly::toLowerAscii(parts[1]);
    if (parts[1] == "sum") {
        method = StatsMethod::SUM;
    } else if (parts[1] == "count") {
        method = StatsMethod::COUNT;
    } else if (parts[1] == "avg") {
        method = StatsMethod::AVG;
    } else if (parts[1] == "rate") {
        method = StatsMethod::RATE;
    } else if (parts[1][0] == 'p') {
        method = StatsMethod::PCT;
        // Percentile
        try {
            size_t len = parts[1].size() - 1;
            if (len > 0 && len <= 6) {
                auto digits = folly::StringPiece(&(parts[1][1]), len);
                pct = folly::to<double>(digits) / dividors[len - 1];
            } else {
                return Status::Error("Invalid percentage %s", parts[1].c_str());
            }
        } catch (const std::exception& ex) {
            return Status::Error("Invalid percentage %s", parts[1].c_str());
        }
    } else {
        return Status::Error("Invalid method %s", parts[1].c_str());
    }

    uint32_t timeRange = 0;
    try {
        timeRange = folly::to<uint32_t>(parts[2]);
    } catch (std::exception&) {
        return Status::Error("Invalid time range %s", parts[2].c_str());
    }
    return MetricSpec{method, timeRange, pct};
}

}   // namespace stats
}   // namespace nebula
