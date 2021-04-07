/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <limits>
#include "gflags/gflags.h"

#include "common/time/TimeUtils.h"

// If it's invalid timezone the service initialize will failed.
// Empty for system default configuration
DEFINE_string(timezone_name,
              "UTC+00:00:00",
              "The timezone used in current system, "
              "only used in nebula datetime compute won't "
              "affect process time (such as log time etc.).");

namespace nebula {
namespace time {

// The mainstream Linux kernel's implementation constrains this
constexpr int64_t kMaxTimestamp = std::numeric_limits<int64_t>::max() / 1000000000;

const DateTime TimeUtils::kEpoch(1970, 1, 1, 0, 0, 0, 0);
/*static*/ Timezone TimeUtils::globalTimezone;

/*static*/ Status TimeUtils::initializeGlobalTimezone() {
    // use system timezone configuration if not set.
    if (FLAGS_timezone_name.empty()) {
        auto *tz = ::getenv("TZ");
        if (tz != nullptr) {
            FLAGS_timezone_name.append(tz);
        }
    }
    if (!FLAGS_timezone_name.empty()) {
        if (FLAGS_timezone_name.front() == ':') {
            NG_RETURN_IF_ERROR(Timezone::init());
            return globalTimezone.loadFromDb(
                std::string(FLAGS_timezone_name.begin() + 1, FLAGS_timezone_name.end()));
        } else {
            return globalTimezone.parsePosixTimezone(FLAGS_timezone_name);
        }
    } else {
        return Status::Error("Don't allowed empty timezone.");
    }
}

/*static*/ StatusOr<DateTime> TimeUtils::dateTimeFromMap(const Map &m) {
    // TODO(shylock) support timezone parameter
    DateTime dt;
    for (const auto &kv : m.kvs) {
        if (!kv.second.isInt()) {
            return Status::Error("Invalid value type.");
        }
        if (kv.first == "year") {
            if (kv.second.getInt() < std::numeric_limits<int16_t>::min() ||
                kv.second.getInt() > std::numeric_limits<int16_t>::max()) {
                return Status::Error("Out of range year `%ld'.", kv.second.getInt());
            }
            dt.d.year = kv.second.getInt();
        } else if (kv.first == "month") {
            if (kv.second.getInt() <= 0 || kv.second.getInt() > 12) {
                return Status::Error("Invalid month number `%ld'.", kv.second.getInt());
            }
            dt.d.month = kv.second.getInt();
        } else if (kv.first == "day") {
            if (kv.second.getInt() <= 0 || kv.second.getInt() > 31) {
                return Status::Error("Invalid day number `%ld'.", kv.second.getInt());
            }
            dt.d.day = kv.second.getInt();
        } else if (kv.first == "hour") {
            if (kv.second.getInt() < 0 || kv.second.getInt() > 23) {
                return Status::Error("Invalid hour number `%ld'.", kv.second.getInt());
            }
            dt.t.hour = kv.second.getInt();
        } else if (kv.first == "minute") {
            if (kv.second.getInt() < 0 || kv.second.getInt() > 59) {
                return Status::Error("Invalid minute number `%ld'.", kv.second.getInt());
            }
            dt.t.minute = kv.second.getInt();
        } else if (kv.first == "second") {
            if (kv.second.getInt() < 0 || kv.second.getInt() > 59) {
                return Status::Error("Invalid second number `%ld'.", kv.second.getInt());
            }
            dt.t.sec = kv.second.getInt();
        } else if (kv.first == "microsecond") {
            if (kv.second.getInt() < 0 || kv.second.getInt() > 999999) {
                return Status::Error("Invalid microsecond number `%ld'.", kv.second.getInt());
            }
            dt.t.microsec = kv.second.getInt();
        } else {
            return Status::Error("Invlaid parameter `%s'.", kv.first.c_str());
        }
    }
    auto result = validateDate(dt.date());
    if (!result.ok()) {
        return result;
    }
    return dt;
}

/*static*/ int64_t TimeUtils::dateTimeDiffSeconds(const DateTime &dateTime0,
                                                  const DateTime &dateTime1) {
    // check the negative divide result, it's used in the negative year number
    // computing.
    static_assert(-1 / 2 == 0, "");
    // Year Base Verification
    static_assert(0 % 100 == 0, "");

    // Compute intervening leap days correctly even if year is negative.
    // take care to avoid integer overflow here.
    int a4 = shr(dateTime0.d.year, 2) + shr(0, 2) - !(dateTime0.d.year & 3);
    int b4 = shr(dateTime1.d.year, 2) + shr(0, 2) - !(dateTime1.d.year & 3);
    int a100 = (a4 + (a4 < 0)) / 25 - (a4 < 0);
    int b100 = (b4 + (b4 < 0)) / 25 - (b4 < 0);
    int a400 = shr(a100, 2);
    int b400 = shr(b100, 2);
    int intervening_leap_days = (a4 - b4) - (a100 - b100) + (a400 - b400);

    /* Compute the desired time without overflowing.  */
    int64_t years = dateTime0.d.year - dateTime1.d.year;
    int64_t days = 365 * years +
                   (isLeapYear(dateTime0.d.year) ? kLeapDaysSoFar[dateTime0.d.month - 1]
                                               : kDaysSoFar[dateTime0.d.month - 1]) -
                   (isLeapYear(dateTime1.d.year) ? kLeapDaysSoFar[dateTime1.d.month - 1]
                                               : kDaysSoFar[dateTime1.d.month - 1]) +
                   dateTime0.d.day - dateTime1.d.day + intervening_leap_days;
    int64_t hours = 24 * days + dateTime0.t.hour - dateTime1.t.hour;
    int64_t minutes = 60 * hours + dateTime0.t.minute - dateTime1.t.minute;
    int64_t seconds = 60 * minutes + dateTime0.t.sec - dateTime1.t.sec;
    return seconds;
}

/*static*/ DateTime TimeUtils::unixSecondsToDateTime(int64_t seconds) {
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
    dt.t.hour = rem / kSecondsOfHour;
    rem %= kSecondsOfHour;
    dt.t.minute = rem / kSecondsOfMinute;
    dt.t.sec = rem % kSecondsOfMinute;
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
    dt.d.year = y;
    if (dt.d.year != y) {
        // overflow
    }
    ip = (isLeapYear(y) ? kLeapDaysSoFar : kDaysSoFar);
    for (y = 11; days < ip[y]; --y) {
        continue;
    }
    days -= ip[y];
    dt.d.month = y + 1;
    dt.d.day = days + 1;
    return dt;
}

#undef DIV
#undef LEAPS_THRU_END_OF

/*static*/ StatusOr<Date> TimeUtils::dateFromMap(const Map &m) {
    Date d;
    for (const auto &kv : m.kvs) {
        if (!kv.second.isInt()) {
            return Status::Error("Invalid value type.");
        }
        if (kv.first == "year") {
            if (kv.second.getInt() < std::numeric_limits<int16_t>::min() ||
                kv.second.getInt() > std::numeric_limits<int16_t>::max()) {
                return Status::Error("Out of range year `%ld'.", kv.second.getInt());
            }
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
    auto result = validateDate(d);
    if (!result.ok()) {
        return result;
    }
    return d;
}

/*static*/ StatusOr<Time> TimeUtils::timeFromMap(const Map &m) {
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

StatusOr<Value> TimeUtils::toTimestamp(const Value &val) {
    Timestamp timestamp;
    if (val.isStr()) {
        auto status = parseDateTime(val.getStr());
        if (!status.ok()) {
            return status.status();
        }
        auto dateTime = std::move(status).value();
        if (dateTime.t.microsec != 0) {
            return Status::Error("The timestamp  only supports seconds unit.");
        }
        timestamp = time::TimeUtils::dateTimeToUnixSeconds(dateTime);
    } else if (val.isInt()) {
        timestamp = val.getInt();
    } else {
        return Status::Error("Incorrect timestamp type: `%s'", val.toString().c_str());
    }

    if (timestamp < 0 || (timestamp > kMaxTimestamp)) {
        return Status::Error("Incorrect timestamp value: `%s'", val.toString().c_str());
    }
    return timestamp;
}
}   // namespace time
}   // namespace nebula
