/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_TIME_TIME_H_
#define COMMON_TIME_TIME_H_

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
// to support non-global timezone configuration
static inline Status initializeGlobalTimezone() {
    if (::setenv("TZ", FLAGS_timezone_name.c_str(), true) != 0) {
        return Status::Error("Set timezone failed: %s", ::strerror(errno));
    }
    ::tzset();
    return Status::OK();
}

// TODO(shylock) support more format
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

static constexpr int64_t kSecondsOfMinute = 60;
static constexpr int64_t kSecondsOfHour = 60 * kSecondsOfMinute;
static constexpr int64_t kSecondsOfDay = 24 * kSecondsOfHour;

static const DateTime kEpoch(1970, 1, 1, 0, 0, 0, 0);

static inline int64_t shr(int64_t a, int b) {
    int64_t one = 1;
    return (-one >> 1 == -1 ? a >> b : (a + (a < 0)) / (one << b) - (a < 0));
}

static inline int64_t dateTimeDiffSeconds(const DateTime &dateTime0, const DateTime &dateTime1) {
    static_assert(-1 / 2 == 0, "");
    // Year Base Verification
    static_assert(0 % 100 == 0, "");

    // Compute intervening leap days correctly even if year is negative.
    // take care to avoid integer overflow here.
    int a4 = shr(dateTime0.year, 2) + shr(0, 2) - !(dateTime0.year & 3);
    int b4 = shr(dateTime1.year, 2) + shr(0, 2) - !(dateTime1.year & 3);
    int a100 = (a4 + (a4 < 0)) / 25 - (a4 < 0);
    int b100 = (b4 + (b4 < 0)) / 25 - (b4 < 0);
    int a400 = shr(a100, 2);
    int b400 = shr(b100, 2);
    int intervening_leap_days = (a4 - b4) - (a100 - b100) + (a400 - b400);

    /* Compute the desired time without overflowing.  */
    int64_t years = dateTime0.year - dateTime1.year;
    int64_t days = 365 * years +
                   (isLeapYear(dateTime0.year) ? leapDaysSoFar[dateTime0.month - 1]
                                               : daysSoFar[dateTime0.month - 1]) -
                   (isLeapYear(dateTime1.year) ? leapDaysSoFar[dateTime1.month - 1]
                                               : daysSoFar[dateTime1.month - 1]) +
                   dateTime0.day - dateTime1.day + intervening_leap_days;
    int64_t hours = 24 * days + dateTime0.hour - dateTime1.hour;
    int64_t minutes = 60 * hours + dateTime0.minute - dateTime1.minute;
    int64_t seconds = 60 * minutes + dateTime0.sec - dateTime1.sec;
    return seconds;
}

// unix time
static inline int64_t dateTimeToSeconds(const DateTime &dateTime) {
    return dateTimeDiffSeconds(dateTime, kEpoch);
}

static inline DateTime secondsToDateTime(int64_t seconds) {
    DateTime dt;
    int64_t days, rem, y;
    const int64_t *ip;

    days = seconds / kSecondsOfDay;
    rem = seconds % kSecondsOfDay;
    while (rem < 0) {
        rem += kSecondsOfDay;
        --days;
    }
    while (rem >= kSecondsOfDay) {
        rem -= kSecondsOfDay;
        ++days;
    }
    dt.hour = rem / kSecondsOfHour;
    rem %= kSecondsOfHour;
    dt.minute = rem / kSecondsOfMinute;
    dt.sec = rem % kSecondsOfMinute;
    y = 1970;

#define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV(y, 4) - DIV(y, 100) + DIV(y, 400))

    while (days < 0 || days >= (isLeapYear(y) ? kDayOfLeapYear : kDayOfCommonYear)) {
        /* Guess a corrected year, assuming 365 days per year.  */
        int64_t yg = y + days / kDayOfCommonYear - (days % kDayOfCommonYear < 0);

        /* Adjust DAYS and Y to match the guessed year.  */
        days -=
            ((yg - y) * kDayOfCommonYear + LEAPS_THRU_END_OF(yg - 1) - LEAPS_THRU_END_OF(y - 1));
        y = yg;
    }
    dt.year = y;
    if (dt.year != y) {
        // overflow
    }
    ip = (isLeapYear(y) ? leapDaysSoFar : daysSoFar);
    for (y = 11; days < ip[y]; --y) {
        continue;
    }
    days -= ip[y];
    dt.month = y + 1;
    dt.day = days + 1;
    return dt;
}

#undef DIV
#undef LEAPS_THRU_END_OF

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

// TODO(shylock) support more format
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
    return dateTimeDiffSeconds(DateTime(date), kEpoch);
}

static inline Date secondsToDate(int64_t seconds) {
    auto dateTime = secondsToDateTime(seconds);
    return Date(dateTime.year, dateTime.month, dateTime.day);
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

// TODO(shylock) support more format
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
    seconds += (time.minute * kSecondsOfMinute);
    seconds += (time.hour * kSecondsOfHour);
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

}   // namespace time
}   // namespace nebula

#endif   // COMMON_TIME_TIME_H_
