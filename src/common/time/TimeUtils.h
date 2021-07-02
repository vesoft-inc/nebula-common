/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_TIME_TIME_H_
#define COMMON_TIME_TIME_H_

#include <iomanip>
#include <limits>
#include <vector>

#include "common/base/Status.h"
#include "common/base/StatusOr.h"
#include "common/datatypes/Date.h"
#include "common/datatypes/Map.h"
#include "common/fs/FileUtils.h"
#include "common/time/TimeConversion.h"
#include "common/time/TimezoneInfo.h"

namespace nebula {
namespace time {

// In nebula only store UTC time, and the interpretion of time value based on the
// timezone configuration in current system.

class TimeUtils {
public:
    explicit TimeUtils(...) = delete;

    // check the validation of date
    // not check range limit in here
    // I.E. 2019-02-31
    template <typename D,
              typename = std::enable_if_t<std::is_same<D, Date>::value ||
                                          std::is_same<D, DateTime>::value>>
    static Status validateDate(const D &date) {
        const int64_t *p = TimeConversion::isLeapYear(date.year) ? kLeapDaysSoFar : kDaysSoFar;
        if ((p[date.month] - p[date.month - 1]) < date.day) {
            return Status::Error("`%s' is not a valid date.", date.toString().c_str());
        }
        return Status::OK();
    }

    // TODO(shylock) support more format
    static StatusOr<DateTime> parseDateTime(const std::string &str) {
        std::tm tm;
        std::istringstream ss(str);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        if (ss.fail()) {
            return Status::Error();
        }
        DateTime dt;
        dt.year = tm.tm_year + 1900;
        dt.month = tm.tm_mon + 1;
        dt.day = tm.tm_mday;
        dt.hour = tm.tm_hour;
        dt.minute = tm.tm_min;
        dt.sec = tm.tm_sec;
        dt.microsec = 0;
        NG_RETURN_IF_ERROR(validateDate(dt));
        return dt;
    }

    static StatusOr<DateTime> dateTimeFromMap(const Map &m);

    // utc + offset = local
    static DateTime dateTimeToUTC(const DateTime &dateTime) {
        return TimeConversion::dateTimeShift(dateTime,
                                             -Timezone::getGlobalTimezone().utcOffsetSecs());
    }

    static DateTime utcToDateTime(const DateTime &dateTime) {
        return TimeConversion::dateTimeShift(dateTime,
                                             Timezone::getGlobalTimezone().utcOffsetSecs());
    }

    static StatusOr<DateTime> localDateTime() {
        DateTime dt;
        time_t unixTime = std::time(NULL);
        if (unixTime == -1) {
            return Status::Error("Get unix time failed: %s.", std::strerror(errno));
        }
        return TimeConversion::unixSecondsToDateTime(unixTime -
                                                     Timezone::getGlobalTimezone().utcOffsetSecs());
    }

    static StatusOr<DateTime> utcDateTime() {
        DateTime dt;
        time_t unixTime = std::time(NULL);
        if (unixTime == -1) {
            return Status::Error("Get unix time failed: %s.", std::strerror(errno));
        }
        return TimeConversion::unixSecondsToDateTime(unixTime);
    }

    static StatusOr<Date> dateFromMap(const Map &m);

    // TODO(shylock) support more format
    static StatusOr<Date> parseDate(const std::string &str) {
        std::tm tm;
        std::istringstream ss(str);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        if (ss.fail()) {
            return Status::Error();
        }
        Date d;
        d.year = tm.tm_year + 1900;
        d.month = tm.tm_mon + 1;
        d.day = tm.tm_mday;
        NG_RETURN_IF_ERROR(validateDate(d));
        return d;
    }

    static StatusOr<Date> localDate() {
        Date d;
        time_t unixTime = std::time(NULL);
        if (unixTime == -1) {
            return Status::Error("Get unix time failed: %s.", std::strerror(errno));
        }
        return TimeConversion::unixSecondsToDate(unixTime -
                                                 Timezone::getGlobalTimezone().utcOffsetSecs());
    }

    static StatusOr<Date> utcDate() {
        Date d;
        time_t unixTime = std::time(NULL);
        if (unixTime == -1) {
            return Status::Error("Get unix time failed: %s.", std::strerror(errno));
        }
        return TimeConversion::unixSecondsToDate(unixTime);
    }

    static StatusOr<Time> timeFromMap(const Map &m);

    // TODO(shylock) support more format
    static StatusOr<Time> parseTime(const std::string &str) {
        std::tm tm;
        std::istringstream ss(str);
        ss >> std::get_time(&tm, "%H:%M:%S");
        if (ss.fail()) {
            return Status::Error();
        }
        Time t;
        t.hour = tm.tm_hour;
        t.minute = tm.tm_min;
        t.sec = tm.tm_sec;
        t.microsec = 0;
        return t;
    }

    // utc + offset = local
    static Time timeToUTC(const Time &time) {
        return TimeConversion::timeShift(time, -Timezone::getGlobalTimezone().utcOffsetSecs());
    }

    static Time utcToTime(const Time &time) {
        return TimeConversion::timeShift(time, Timezone::getGlobalTimezone().utcOffsetSecs());
    }

    static StatusOr<Time> localTime() {
        Time dt;
        time_t unixTime = std::time(NULL);
        if (unixTime == -1) {
            return Status::Error("Get unix time failed: %s.", std::strerror(errno));
        }
        return TimeConversion::unixSecondsToTime(unixTime -
                                                 Timezone::getGlobalTimezone().utcOffsetSecs());
    }

    static StatusOr<Time> utcTime() {
        Time dt;
        time_t unixTime = std::time(NULL);
        if (unixTime == -1) {
            return Status::Error("Get unix time failed: %s.", std::strerror(errno));
        }
        return TimeConversion::unixSecondsToTime(unixTime);
    }

    static StatusOr<Value> toTimestamp(const Value &val);
};   // class TimeUtils

}   // namespace time
}   // namespace nebula

#endif   // COMMON_TIME_TIME_H_
