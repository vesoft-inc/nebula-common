/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/time/TimeParser.h"
#include <sstream>
#include "common/time/TimeUtils.h"
#include "common/time/TimezoneInfo.h"

namespace nebula {
namespace time {

/*static*/ const std::vector<TimeParser::Component> TimeParser::dateTimeStates = {
    // State, ShiftMap
    {kInitial,
     [](Token, Token, DateTime&, ExpectType type, TokenType&, int32_t&) -> StatusOr<State> {
         if (type == ExpectType::kDateTime) {
             return kDateYear;
         } else if (type == ExpectType::kDate) {
             return kDateYear;
         } else if (type == ExpectType::kTime) {
             return kTimeHour;
         }
         return Status::Error("Unknown time type.");
     }},
    {kDateYear,
     [](Token t, Token n, DateTime& val, ExpectType type, TokenType&, int32_t&) -> StatusOr<State> {
         if (t.type == TokenType::kNumber) {
             if (t.val < std::numeric_limits<decltype(val.year)>::min() ||
                 t.val > std::numeric_limits<decltype(val.year)>::max()) {
                 return Status::Error("The year number `%d' exceed the number limit.", t.val);
             }
             val.year = t.val;
             switch (n.type) {
                 case TokenType::kMinus:
                     return kDateMonth;
                 case TokenType::kTimePrefix:
                     if (type == ExpectType::kDate) {
                         return Status::Error("Unexpected read-ahead token `%s'.",
                                              toString(n.type));
                     }
                     return kTimeHour;
                 case TokenType::kPlaceHolder:
                     return kEnd;
                 default:
                     return Status::Error("Unexpected read-ahead token `%s'.", toString(n.type));
             }
         } else {
             return Status::Error("Unexpected token `%s'.", toString(n.type));
         }
     }},
    {kDateMonth,
     [](Token t, Token n, DateTime& val, ExpectType type, TokenType&, int32_t&) -> StatusOr<State> {
         if (t.type == TokenType::kMinus) {
             return kDateMonth;
         }
         if (t.type == TokenType::kNumber) {
             if (t.val < 1 || t.val > 12) {
                 return Status::Error("The month number `%d' exceed the number limit.", t.val);
             }
             val.month = t.val;
             switch (n.type) {
                 case TokenType::kMinus:
                     return kDateDay;
                 case TokenType::kTimePrefix:
                     if (type == ExpectType::kDate) {
                         return Status::Error("Unexpected read-ahead token `%s'.",
                                              toString(n.type));
                     }
                     return kTimeHour;
                 case TokenType::kPlaceHolder:
                     return kEnd;
                 default:
                     return Status::Error("Unexpected read-ahead token `%s'.", toString(n.type));
             }
         } else {
             return Status::Error("Unexpected token `%s'.", toString(n.type));
         }
     }},
    {kDateDay,
     [](Token t, Token n, DateTime& val, ExpectType type, TokenType&, int32_t&) -> StatusOr<State> {
         if (t.type == TokenType::kMinus) {
             return kDateDay;
         }
         if (t.type == TokenType::kNumber) {
             if (t.val < 1 || t.val > 31) {
                 return Status::Error("The day number `%d' exceed the number limit.", t.val);
             }
             val.day = t.val;
             switch (n.type) {
                 case TokenType::kTimePrefix:
                     if (type == ExpectType::kDate) {
                         return Status::Error("Unexpected read-ahead token `%s'.",
                                              toString(n.type));
                     }
                     return kTimeHour;
                 case TokenType::kPlaceHolder:
                     return kEnd;
                 default:
                     return Status::Error("Unexpected read-ahead token `%s'.", toString(n.type));
             }
         } else {
             return Status::Error("Unexpected token `%s'.", toString(n.type));
         }
     }},
    {kTimeHour,
     [](Token t, Token n, DateTime& val, ExpectType, TokenType&, int32_t&) -> StatusOr<State> {
         if (t.type == TokenType::kTimePrefix) {
             return kTimeHour;
         }
         if (t.type == TokenType::kNumber) {
             if (t.val < 0 || t.val > 23) {
                 return Status::Error("The hour number `%d' exceed the number limit.", t.val);
             }
             val.hour = t.val;
             switch (n.type) {
                 case TokenType::kTimeDelimiter:
                     return kTimeMinute;
                 case TokenType::kPlus:
                 case TokenType::kMinus:
                 case TokenType::kTimeZoneName:
                     return kUtcOffset;
                 case TokenType::kPlaceHolder:
                     return kEnd;
                 default:
                     return Status::Error("Unexpected read-ahead token `%s'.", toString(n.type));
             }
         } else {
             return Status::Error("Unexpected token `%s'.", toString(n.type));
         }
         return Status::OK();
     }},
    {kTimeMinute,
     [](Token t, Token n, DateTime& val, ExpectType, TokenType&, int32_t&) -> StatusOr<State> {
         if (t.type == TokenType::kTimeDelimiter) {
             return kTimeMinute;
         }
         if (t.type == TokenType::kNumber) {
             if (t.val < 0 || t.val > 59) {
                 return Status::Error("The minute number `%d' exceed the number limit.", t.val);
             }
             val.minute = t.val;
             switch (n.type) {
                 case TokenType::kTimeDelimiter:
                     return kTimeSecond;
                 case TokenType::kPlus:
                 case TokenType::kMinus:
                 case TokenType::kTimeZoneName:
                     return kUtcOffset;
                 case TokenType::kPlaceHolder:
                     return kEnd;
                 default:
                     return Status::Error("Unexpected read-ahead token `%s'.", toString(n.type));
             }
         } else {
             return Status::Error("Unexpected token `%s'.", toString(n.type));
         }
         return Status::OK();
     }},
    {kTimeSecond,
     [](Token t, Token n, DateTime& val, ExpectType, TokenType&, int32_t&) -> StatusOr<State> {
         if (t.type == TokenType::kTimeDelimiter) {
             return kTimeSecond;
         }
         if (t.type == TokenType::kNumber) {
             if (t.val < 0 || t.val > 59) {
                 return Status::Error("The second number `%d' exceed the number limit.", t.val);
             }
             val.sec = t.val;
             switch (n.type) {
                 case TokenType::kMicroSecondPrefix:
                     return kTimeMicroSecond;
                 case TokenType::kPlus:
                 case TokenType::kMinus:
                 case TokenType::kTimeZoneName:
                     return kUtcOffset;
                 case TokenType::kPlaceHolder:
                     return kEnd;
                 default:
                     return Status::Error("Unexpected read-ahead token `%s'.", toString(n.type));
             }
         } else {
             return Status::Error("Unexpected token `%s'.", toString(n.type));
         }
         return Status::OK();
     }},
    {kTimeMicroSecond,
     [](Token t, Token n, DateTime& val, ExpectType, TokenType&, int32_t&) -> StatusOr<State> {
         if (t.type == TokenType::kMicroSecondPrefix) {
             return kTimeMicroSecond;
         }
         if (t.type == TokenType::kNumber) {
             if (t.val < 0 || t.val > 999999) {
                 return Status::Error("The microsecond number `%d' exceed the number limit.",
                                      t.val);
             }
             val.microsec = t.val;
             switch (n.type) {
                 case TokenType::kMicroSecondPrefix:
                     return kTimeMicroSecond;
                 case TokenType::kPlus:
                 case TokenType::kMinus:
                 case TokenType::kTimeZoneName:
                     return kUtcOffset;
                 case TokenType::kPlaceHolder:
                     return kEnd;
                 default:
                     return Status::Error("Unexpected read-ahead token `%s'.", toString(n.type));
             }
         } else {
             return Status::Error("Unexpected token `%s'.", toString(n.type));
         }
         return Status::OK();
     }},
    {kUtcOffset,
     [](Token t, Token, DateTime& val, ExpectType, TokenType& utcSign, int32_t&)
         -> StatusOr<State> {
         switch (t.type) {
             case TokenType::kPlus:
             case TokenType::kMinus:
                 if (utcSign != TokenType::kUnknown) {
                     return Status::Error("Unexpected token `%s'.", toString(t.type));
                 }
                 utcSign = t.type;
                 return kUtcOffsetHour;
             case TokenType::kTimeZoneName: {
                 Timezone tz;
                 auto result = tz.loadFromDb(t.str);
                 NG_RETURN_IF_ERROR(result);
                 val = TimeUtils::dateTimeShift(val, tz.utcOffsetSecs());
                 return kEnd;
             }
             default:
                 return Status::Error("Unexpected token `%s'.", toString(t.type));
         }
     }},
    {kUtcOffsetHour,
     [](Token t, Token, DateTime&, ExpectType, TokenType& utcSign, int32_t& utcOffsetSecs)
         -> StatusOr<State> {
         switch (t.type) {
             case TokenType::kNumber:
                 if (t.val > 23) {
                     return Status::Error("Unexpected utc offset hours number `%d'.", t.val);
                 }
                 utcOffsetSecs = (utcSign == TokenType::kPlus ? t.val * 60 * 60 : -t.val * 60 * 60);
                 return kUtcOffsetMinute;
             default:
                 return Status::Error("Unexpected token `%s'.", toString(t.type));
         }
     }},
    {kUtcOffsetMinute,
     [](Token t, Token n, DateTime& val, ExpectType, TokenType& utcSign, int32_t& utcOffsetSecs)
         -> StatusOr<State> {
         switch (t.type) {
             case TokenType::kTimeDelimiter:
                 if (n.type != TokenType::kNumber) {
                     return Status::Error("Unexpected read-ahead token `%s'.", toString(n.type));
                 }
                 return kUtcOffsetMinute;
             case TokenType::kNumber:
                 if (t.val > 59) {
                     return Status::Error("Unexpected utc offset minutes number `%d'.", t.val);
                 }
                 utcOffsetSecs += (utcSign == TokenType::kPlus ? t.val * 60 : -t.val * 60);
                 val = TimeUtils::dateTimeShift(val, utcOffsetSecs);
                 return kEnd;
             default:
                 return Status::Error("Unexpected token `%s'.", toString(t.type));
         }
     }},
    {kEnd,
     [](Token, Token, DateTime&, ExpectType, TokenType&, int32_t&) -> StatusOr<State> {
         return Status::OK();
     }},
};

/*static*/ const char* TimeParser::toString(TokenType t) {
    switch (t) {
        case TokenType::kUnknown:
            return "Unknown";
        case TokenType::kPlaceHolder:
            return "PlaceHolder";
        case TokenType::kNumber:
            return "Number";
        case TokenType::kPlus:
            return "+";
        case TokenType::kMinus:
            return "-";
        case TokenType::kTimeDelimiter:
            return "TimeDelimiter";
        case TokenType::kTimePrefix:
            return "TimePrefix";
        case TokenType::kMicroSecondPrefix:
            return "MicroSecondPrefix";
        case TokenType::kTimeZoneName:
            return "TimeZoneName";
    }
    LOG(FATAL) << "Unknown token " << static_cast<int>(t);
}

/*static*/ std::string TimeParser::toString(const Token& t) {
    std::stringstream ss;
    ss << toString(t.type);
    ss << "(";
    if (t.type == TokenType::kNumber) {
        ss << t.val;
    }
    if (t.type == TokenType::kTimeZoneName) {
        ss << t.str;
    }
    ss << ")";
    return ss.str();
}

Status TimeParser::lex(folly::StringPiece str) {
    tokens_.reserve(8);
    std::string digits;
    digits.reserve(8);
    auto c = str.start();
    while (*c != '\0') {
        if (std::isdigit(*c)) {
            digits.push_back(*c);
            if (!std::isdigit(*(c + 1))) {
                Token t;
                try {
                    t.val = folly::to<int32_t>(digits);
                } catch (std::exception& e) {
                    return Status::Error("%s", e.what());
                }
                t.type = TokenType::kNumber;
                tokens_.emplace_back(t);
                digits.clear();
            }
        } else if (kTimeDelimiter == *c) {
            tokens_.emplace_back(Token{TokenType::kTimeDelimiter, 0, ""});
        } else if (kTimePrefix == *c) {
            tokens_.emplace_back(Token{TokenType::kTimePrefix, 0, ""});
        } else if (kMicroSecondPrefix == *c) {
            tokens_.emplace_back(Token{TokenType::kMicroSecondPrefix, 0, ""});
        } else if (kPlus == *c) {
            tokens_.emplace_back(Token{TokenType::kPlus, 0, ""});
        } else if (kMinus == *c) {
            tokens_.emplace_back(Token{TokenType::kMinus, 0, ""});
        } else if (kLeftBracket == *c) {
            std::string s;
            while (*(++c) != kRightBracket) {
                if (*c == '\0') {
                    return Status::Error("Unterminated bracket.");
                }
                s.push_back(*c);
            }
            tokens_.emplace_back(Token{TokenType::kTimeZoneName, 0, std::move(s)});
        } else {
            return Status::Error("Illegal character `%c'.", *c);
        }
        ++c;
    }
    // Only for read-ahead placeholder
    tokens_.emplace_back(Token{TokenType::kPlaceHolder, 0, ""});
    return Status::OK();
}

Status TimeParser::parse() {
    auto result = dateTimeStates[kInitial].next({}, {}, result_, type_, utcSign_, utcOffsetSecs_);
    NG_RETURN_IF_ERROR(result);
    auto current = result.value();
    for (std::size_t i = 0; i < tokens_.size() - 1; ++i) {
        result = dateTimeStates[current].next(
            tokens_[i], tokens_[i + 1], result_, type_, utcSign_, utcOffsetSecs_);
        NG_RETURN_IF_ERROR(result);
        current = result.value();
        if (current == kEnd) {
            break;
        }
    }
    if (current != kEnd) {
        return Status::Error("Not end parse.");
    }
    return Status::OK();
}

}   // namespace time
}   // namespace nebula
