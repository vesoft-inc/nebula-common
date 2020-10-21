/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_TIME_TIMEZONEINFO_H_
#define COMMON_TIME_TIMEZONEINFO_H_

#include <boost/date_time/local_time/local_time.hpp>

#include "common/base/Base.h"
#include "common/base/Status.h"

namespace nebula {
namespace time {

class Timezone {
public:
    explicit Timezone(const std::string &name) : name_(name) {}

    static MUST_USE_RESULT Status init() {
        try {
            tzdb.load_from_file(kTzDbFile);
        } catch (const std::exception &e) {
            return Status::Error("%s", e.what());
        }
        return Status::OK();
    }

    MUST_USE_RESULT Status load() {
        zoneInfo_ = tzdb.time_zone_from_region(name_);
        if (zoneInfo_ == nullptr) {
            return Status::Error("Not supported timezone `%s'.", name_.c_str());
        }
        return Status::OK();
    }

    // offset in seconds
    uint32_t utcOffset() const {
        return zoneInfo_->base_utc_offset().total_seconds();
    }

private:
    static constexpr char kTzDbFile[] = "share/resources/date_time_zonespec.csv";
    static ::boost::local_time::tz_database tzdb;

    const std::string                                          name_;
    ::boost::shared_ptr<::boost::local_time::custom_time_zone> zoneInfo_{nullptr};
};

}   // namespace time
}   // namespace nebula

#endif   // COMMON_TIME_TIMEZONEINFO_H_
