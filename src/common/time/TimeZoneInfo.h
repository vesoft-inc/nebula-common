/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_TIME_TIMEZONEINFO_H_
#define COMMON_TIME_TIMEZONEINFO_H_

#include <boost/date_time/local_time/local_time.hpp>
#include <gflags/gflags_declare.h>

#include <exception>

#include "common/base/Base.h"
#include "common/base/Status.h"

DECLARE_string(time_zone_file);

namespace nebula {
namespace time {

class TimeZone {
public:
    TimeZone() = default;

    static MUST_USE_RESULT Status init() {
        try {
            tzdb.load_from_file(FLAGS_time_zone_file);
        } catch (const std::exception &e) {
            return Status::Error("Invalid time zone file `%s', exception: `%s'.",
                                 FLAGS_time_zone_file.c_str(),
                                 e.what());
        }
        return Status::OK();
    }

    MUST_USE_RESULT Status loadFromDb(const std::string &region) {
        zoneInfo_ = tzdb.time_zone_from_region(region);
        if (zoneInfo_ == nullptr) {
            return Status::Error("Not supported time zone `%s'.", region.c_str());
        }
        return Status::OK();
    }

    // see the posix time zone literal format in https://man7.org/linux/man-pages/man3/tzset.3.html
    MUST_USE_RESULT Status parsePosixTimeZone(const std::string &posixTimeZone) {
        try {
            zoneInfo_.reset(new ::boost::local_time::posix_time_zone(posixTimeZone));
        } catch (const std::exception &e) {
            return Status::Error("Malformed time zone format: `%s', exception: `%s'.",
                                 posixTimeZone.c_str(),
                                 e.what());
        }
        return Status::OK();
    }

    std::string stdZoneName() const {
        return DCHECK_NOTNULL(zoneInfo_)->std_zone_name();
    }

    // offset in seconds
    int32_t utcOffsetSecs() const {
        return DCHECK_NOTNULL(zoneInfo_)->base_utc_offset().total_seconds();
    }

private:
    static ::boost::local_time::tz_database tzdb;

    ::boost::shared_ptr<::boost::date_time::time_zone_base<::boost::posix_time::ptime, char>>
        zoneInfo_{nullptr};
};

}   // namespace time
}   // namespace nebula

#endif   // COMMON_TIME_TIMEZONEINFO_H_
