/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_TIME_TIMEZONE_H_
#define COMMON_TIME_TIMEZONE_H_

#include <gflags/gflags_declare.h>
#include <iomanip>
#include <limits>

#include "common/base/Status.h"
#include "common/base/StatusOr.h"
#include "common/datatypes/Date.h"
#include "common/datatypes/Map.h"

DECLARE_string(timezone_name);

namespace nebula {
namespace time {

// In nebula only store UTC time, and the interpretion of time value based on the
// timezone configuration in current system.

// TODO(shylock) Get Timzone info(I.E. GMT offset) directly from IANA tzdb
static inline Status initializeGlobalTimezone() {
    if (::setenv("TZ", FLAGS_timezone_name.c_str(), true) != 0) {
        return Status::Error("Set timezone failed: %s", ::strerror(errno));
    }
    ::tzset();
    return Status::OK();
}

static inline StatusOr<std::tm*> localTM() {
    time_t unixTime = std::time(NULL);
    if (unixTime == -1) {
        return Status::Error("Get unix time failed: %s.", std::strerror(errno));
    }
    // TODO(shylock) support accept timezone to convert to localtime if require nonglobal timezone
    // configuration
    struct std::tm *ptm = std::localtime(&unixTime);
    if (ptm == nullptr) {
        return Status::Error("Get local time failed: %s.", std::strerror(errno));
    }
    return ptm;
}

static inline StatusOr<DateTime> parseDateTime(const std::string &str) {
    std::tm tm;
    std::istringstream ss(str);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
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
    return dt;
}

static inline StatusOr<DateTime> dateTimeFromMap(const Map &m) {
    DateTime dt;
    for (const auto &kv : m.kvs) {
        if (!kv.second.isInt()) {
            return Status::Error("Invalid value type.");
        }
        if (kv.first == "year") {
            dt.year = kv.second.getInt();
        } else if (kv.first == "month") {
            if (kv.second.getInt() <= 0 || kv.second.getInt() > 12) {
                return Status::Error("Invalid month number `%ld'.", kv.second.getInt());
            }
            dt.month = kv.second.getInt();
        } else if (kv.first == "day") {
            if (kv.second.getInt() <= 0 || kv.second.getInt() > 31) {
                return Status::Error("Invalid day number `%ld'.", kv.second.getInt());
            }
            dt.day = kv.second.getInt();
        } else if (kv.first == "hour") {
            if (kv.second.getInt() < 0 || kv.second.getInt() > 23) {
                return Status::Error("Invalid hour number `%ld'.", kv.second.getInt());
            }
            dt.hour = kv.second.getInt();
        } else if (kv.first == "minute") {
            if (kv.second.getInt() < 0 || kv.second.getInt() > 59) {
                return Status::Error("Invalid minute number `%ld'.", kv.second.getInt());
            }
            dt.minute = kv.second.getInt();
        } else if (kv.first == "second") {
            if (kv.second.getInt() < 0 || kv.second.getInt() > 59) {
                return Status::Error("Invalid second number `%ld'.", kv.second.getInt());
            }
            dt.sec = kv.second.getInt();
        } else if (kv.first == "microsecond") {
            if (kv.second.getInt() < 0 || kv.second.getInt() > 999999) {
                return Status::Error("Invalid microsecond number `%ld'.", kv.second.getInt());
            }
            dt.microsec = kv.second.getInt();
        } else {
            return Status::Error("Invlaid parameter `%s'.", kv.first.c_str());
        }
    }
    return dt;
}

// https://en.wikipedia.org/wiki/Leap_year#Leap_day
static inline bool isLeapYear(int16_t year) {
    if (year % 4 != 0) {
        return false;
    } else if (year % 100 != 0) {
        return true;
    } else if (year % 400 != 0) {
        return false;
    } else {
        return true;
    }
}

static constexpr int kDayOfLeapYear = 366;
static constexpr int kDayOfCommonYear = 365;

// unix time
static inline int64_t dateTimeToSeconds(const DateTime &dateTime) {
    int64_t seconds = dateTime.sec;
    seconds += (dateTime.minute * 60);
    seconds += (dateTime.hour * 60 * 60);
    seconds += ((dateTime.day -1) * 60 * 60 * 24);
    if (isLeapYear(dateTime.year)) {
        seconds += (60 * 60 * 24 * leapDaysSoFar[dateTime.month - 1]);
    } else {
        seconds += (60 * 60 * 24 * daysSoFar[dateTime.month - 1]);
    }
    for (int64_t i = dateTime.year - 1; i >= 1970; --i) {
        if (isLeapYear(i)) {
            seconds += (60 * 60 * 24 * kDayOfLeapYear);
        } else {
            seconds += (60 * 60 * 24 * kDayOfCommonYear);
        }
    }
    return seconds;
}

static inline DateTime secondsToDateTime(int64_t seconds) {
    DateTime dt;
    int16_t year = 1970;
    for (; year < std::numeric_limits<int16_t>::max(); ++year) {
        if (isLeapYear(year)) {
            if (seconds < (60 * 60 * 24 * kDayOfLeapYear)) {
                break;
            }
            seconds -= (60 * 60 * 24 * kDayOfLeapYear);
        } else {
            if (seconds < (60 * 60 * 24 * kDayOfCommonYear)) {
                break;
            }
            seconds -= (60 * 60 * 24 * kDayOfCommonYear);
        }
    }
    dt.year = year;
    int8_t month = 1;
    for (; month <= 12; ++month) {
        if (isLeapYear(dt.year)) {
            if (seconds < (60 * 60 * 24 * leapDaysSoFar[month])) {
                seconds -= (60 * 60 * 24 * leapDaysSoFar[month - 1]);
                break;
            }
        } else {
            if (seconds < (60 * 60 * 24 * daysSoFar[month])) {
                seconds -= (60 * 60 * 24 * daysSoFar[month - 1]);
                break;
            }
        }
    }
    dt.month = month;
    dt.day = seconds / (60 * 60 * 24) + 1;
    seconds -= ((dt.day - 1) * 60 * 60 * 24);
    dt.hour = seconds / (60 * 60);
    seconds -= (dt.hour * 60 * 60);
    dt.minute = seconds / 60;
    seconds -= (dt.minute * 60);
    dt.sec = seconds;
    return dt;
}

// Shift the DateTime in timezone space
static inline DateTime dateTimeShift(const DateTime &dateTime, int64_t offsetSeconds) {
    if (offsetSeconds == 0) {
        return dateTime;
    }
    return secondsToDateTime(dateTimeToSeconds(dateTime) + offsetSeconds);
}

static inline DateTime dateTimeToUTC(const DateTime &dateTime) {
    return dateTimeShift(dateTime, timezone);
}

static inline DateTime utcToDateTime(const DateTime &dateTime) {
    return dateTimeShift(dateTime, -timezone);
}

static inline StatusOr<DateTime> localDateTime() {
    DateTime dt;
    time_t unixTime = std::time(NULL);
    if (unixTime == -1) {
        return Status::Error("Get unix time failed: %s.", std::strerror(errno));
    }
    return secondsToDateTime(unixTime - timezone);
}

static inline StatusOr<Date> dateFromMap(const Map &m) {
    Date d;
    for (const auto &kv : m.kvs) {
        if (!kv.second.isInt()) {
            return Status::Error("Invalid value type.");
        }
        if (kv.first == "year") {
            d.year = kv.second.getInt();
        } else if (kv.first == "month") {
            if (kv.second.getInt() <= 0 || kv.second.getInt() > 12) {
                return Status::Error("Invalid month number `%ld'.", kv.second.getInt());
            }
            d.month = kv.second.getInt();
        } else if (kv.first == "day") {
            if (kv.second.getInt() <= 0 || kv.second.getInt() > 31) {
                return Status::Error("Invalid day number `%ld'.", kv.second.getInt());
            }
            d.day = kv.second.getInt();
        } else {
            return Status::Error("Invlaid parameter `%s'.", kv.first.c_str());
        }
    }
    return d;
}

static inline StatusOr<Date> parseDate(const std::string &str) {
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
    return d;
}

// unix time
static inline int64_t dateToSeconds(const Date &date) {
    int64_t seconds = ((date.day -1) * 60 * 60 * 24);
    if (isLeapYear(date.year)) {
        seconds += (60 * 60 * 24 * leapDaysSoFar[date.month - 1]);
    } else {
        seconds += (60 * 60 * 24 * daysSoFar[date.month - 1]);
    }
    for (int64_t i = date.year - 1; i >= 1970; --i) {
        if (isLeapYear(i)) {
            seconds += (60 * 60 * 24 * kDayOfLeapYear);
        } else {
            seconds += (60 * 60 * 24 * kDayOfCommonYear);
        }
    }
    return seconds;
}

static inline Date secondsToDate(int64_t seconds) {
    Date d;
    int16_t year = 1970;
    for (; year < std::numeric_limits<int16_t>::max(); ++year) {
        if (isLeapYear(year)) {
            if (seconds < (60 * 60 * 24 * kDayOfLeapYear)) {
                break;
            }
            seconds -= (60 * 60 * 24 * kDayOfLeapYear);
        } else {
            if (seconds < (60 * 60 * 24 * kDayOfCommonYear)) {
                break;
            }
            seconds -= (60 * 60 * 24 * kDayOfCommonYear);
        }
    }
    d.year = year;
    int8_t month = 1;
    for (; month <= 12; ++month) {
        if (isLeapYear(d.year)) {
            if (seconds < (60 * 60 * 24 * leapDaysSoFar[month])) {
                seconds -= (60 * 60 * 24 * leapDaysSoFar[month - 1]);
                break;
            }
        } else {
            if (seconds < (60 * 60 * 24 * daysSoFar[month])) {
                seconds -= (60 * 60 * 24 * daysSoFar[month - 1]);
                break;
            }
        }
    }
    d.month = month;
    d.day = seconds / (60 * 60 * 24) + 1;
    return d;
}

// Shift the DateTime in timezone space
static inline Date dateShift(const Date &date, int64_t offsetSeconds) {
    if (offsetSeconds == 0) {
        return date;
    }
    return secondsToDate(dateToSeconds(date) + offsetSeconds);
}

static inline Date dateToUTC(const Date &date) {
    return dateShift(date, timezone);
}

static inline Date utcToDate(const Date &date) {
    return dateShift(date, -timezone);
}

static inline StatusOr<Date> localDate() {
    Date d;
    time_t unixTime = std::time(NULL);
    if (unixTime == -1) {
        return Status::Error("Get unix time failed: %s.", std::strerror(errno));
    }
    return secondsToDate(unixTime - timezone);
}

static inline StatusOr<Time> timeFromMap(const Map &m) {
    Time t;
    for (const auto &kv : m.kvs) {
        if (!kv.second.isInt()) {
            return Status::Error("Invalid value type.");
        }
        if (kv.first == "hour") {
            if (kv.second.getInt() < 0 || kv.second.getInt() > 23) {
                return Status::Error("Invalid hour number `%ld'.", kv.second.getInt());
            }
            t.hour = kv.second.getInt();
        } else if (kv.first == "minute") {
            if (kv.second.getInt() < 0 || kv.second.getInt() > 59) {
                return Status::Error("Invalid minute number `%ld'.", kv.second.getInt());
            }
            t.minute = kv.second.getInt();
        } else if (kv.first == "second") {
            if (kv.second.getInt() < 0 || kv.second.getInt() > 59) {
                return Status::Error("Invalid second number `%ld'.", kv.second.getInt());
            }
            t.sec = kv.second.getInt();
        } else if (kv.first == "microsecond") {
            if (kv.second.getInt() < 0 || kv.second.getInt() > 999999) {
                return Status::Error("Invalid microsecond number `%ld'.", kv.second.getInt());
            }
            t.microsec = kv.second.getInt();
        } else {
            return Status::Error("Invlaid parameter `%s'.", kv.first.c_str());
        }
    }
    return t;
}

static inline StatusOr<Time> parseTime(const std::string &str) {
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

// unix time
static inline int64_t timeToSeconds(const Time &time) {
    int64_t seconds = time.sec;
    seconds += (time.minute * 60);
    seconds += (time.hour * 60 * 60);
    return seconds;
}

static inline Time secondsToTime(int64_t seconds) {
    Time t;
    auto dt = secondsToDateTime(seconds);
    t.hour = dt.hour;
    t.minute = dt.minute;
    t.sec = dt.sec;
    return t;
}

// Shift the Time in timezone space
static inline Time timeShift(const Time &time, int64_t offsetSeconds) {
    if (offsetSeconds == 0) {
        return time;
    }
    return secondsToTime(timeToSeconds(time) + offsetSeconds);
}

static inline Time timeToUTC(const Time &time) {
    return timeShift(time, timezone);
}

static inline Time utcToTime(const Time &time) {
    return timeShift(time, -timezone);
}

static inline StatusOr<Time> localTime() {
    Time dt;
    time_t unixTime = std::time(NULL);
    if (unixTime == -1) {
        return Status::Error("Get unix time failed: %s.", std::strerror(errno));
    }
    return secondsToTime(unixTime - timezone);
}

}  // namespace time
}  // namespace nebula

#endif  // COMMON_TIME_TIMEZONE_H_
