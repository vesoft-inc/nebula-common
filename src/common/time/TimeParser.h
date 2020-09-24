/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_TIME_TIMEPARSER_H_
#define COMMON_TIME_TIMEPARSER_H_

#include "common/base/Base.h"
#include "common/base/Status.h"
#include "common/base/StatusOr.h"
#include "common/datatypes/Date.h"

namespace nebula {
namespace time {

// parser the date/time/datetime literal
class TimeParser {
public:
    TimeParser() = default;

    StatusOr<Date> parseDate(folly::StringPiece str) {
        type_ = ExpectType::kDate;
        NG_RETURN_IF_ERROR(lex(str));
        NG_RETURN_IF_ERROR(parse());
        return Date{result_.year, result_.month, result_.day};
    }

    StatusOr<Time> parseTime(folly::StringPiece str) {
        type_ = ExpectType::kTime;
        NG_RETURN_IF_ERROR(lex(str));
        NG_RETURN_IF_ERROR(parse());
        return Time{result_.hour, result_.minute, result_.sec, result_.microsec};
    }

    StatusOr<DateTime> parseDateTime(folly::StringPiece str) {
        type_ = ExpectType::kDateTime;
        NG_RETURN_IF_ERROR(lex(str));
        NG_RETURN_IF_ERROR(parse());
        return result_;
    }

private:
    static constexpr char kDateDelimiter = '-';
    static constexpr char kTimeDelimiter = ':';

    static constexpr char kTimePrefix = 'T';
    static constexpr char kMicroSecondPrefix = '.';
    static constexpr char kUTCoffsetPrefixPlus = '+';
    static constexpr char kUTCoffsetPrefixMinus = '-';

    enum class ExpectType {
        kDate,
        kTime,
        kDateTime,
    };

    enum class TokenType {
        kUnknown,
        kPlaceHolder,   // Only for read-ahead placeholder
        kNumber,
        kDateDelimiter,
        kTimeDelimiter,
        kTimePrefix,
        kMicroSecondPrefix,
        kUTCoffsetPrefixPlus,
        kUTCoffsetPrefixMinus,
    };

    static const char* toString(TokenType t);

    struct Token {
        TokenType type;
        int32_t val;   // only used for number token
    };

    // TODO(shylock) support weeks/days format when support the convertion
    // The state of current parser
    enum State {
        kInitial,           // 0
        kDateYear,          // 1
        kDateMonth,         // 2
        kDateDay,           // 3
        kTimeHour,          // 4
        kTimeMinute,        // 5
        kTimeSecond,        // 6
        kTimeMicroSecond,   // 7
        kUtcOffset,         // 8
        kEnd,               // 9
        kSize,              // Just for count
    };

    struct Component {
        const State state;   // current state
        // State shift function
        std::function<StatusOr<State>(Token, Token, DateTime&, ExpectType)> next;
    };

    // All states of datetime
    static const std::vector<Component> dateTimeStates;

    Status lex(folly::StringPiece str);

    Status parse();

private:
    std::vector<Token> tokens_;
    DateTime result_;
    ExpectType type_;
};

}   // namespace time
}   // namespace nebula

#endif   // COMMON_TIME_TIMEPARSER_H_
