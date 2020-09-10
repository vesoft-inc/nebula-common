/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "FunctionManager.h"
#include "common/base/Base.h"
#include "common/expression/Expression.h"
#include "common/time/WallClock.h"
#include "common/datatypes/List.h"
#include "common/datatypes/Map.h"
#include "common/datatypes/Set.h"
#include "common/datatypes/DataSet.h"

namespace nebula {

// static
FunctionManager &FunctionManager::instance() {
    static FunctionManager instance;
    return instance;
}

// static
StatusOr<Value::Type>
FunctionManager::getReturnType(const std::string &funName,
                               const std::vector<Value::Type> &argsType,
                               const std::string &scope) {
    // check existence
    auto result = instance().functions_.findSignature(scope, funName);
    NG_RETURN_IF_ERROR(result);
    const auto iter = std::move(result).value();
    for (const auto &args : iter->second) {
        if (argsType == args.argsType_) {
            return args.returnType_;
        }
    }
    return Status::Error("Parameter's type error");
}

const FunctionManager::Function FunctionManager::abs = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::abs(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::floor = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::floor(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::ceil = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::ceil(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::round = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::round(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::sqrt = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::sqrt(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::cbrt = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::cbrt(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::hypot = [](const auto &args) -> Value {
            if (args[0].isNumeric() && args[1].isNumeric()) {
                auto x = args[0].isInt() ? args[0].getInt() : args[0].getFloat();
                auto y = args[1].isInt() ? args[1].getInt() : args[1].getFloat();
                return std::hypot(x, y);
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::pow = [](const auto &args) -> Value {
            if (args[0].isNumeric() && args[1].isNumeric()) {
                auto base = args[0].isInt() ? args[0].getInt() : args[0].getFloat();
                auto exp = args[1].isInt() ? args[1].getInt() : args[1].getFloat();
                return std::pow(base, exp);
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::exp = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::exp(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::exp2 = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::exp2(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::log = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::log(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::log2 = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::log2(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::log10 = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::log10(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::sin = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::sin(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::asin = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::asin(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::cos = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::cos(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::acos = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::acos(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::tan = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::tan(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::atan = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::atan(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::rand32 = [](const auto &args) -> Value {
            if (args.empty()) {
                auto value = folly::Random::rand32();
                return static_cast<int64_t>(static_cast<int32_t>(value));
            } else if (args.size() == 1UL) {
                if (args[0].isInt()) {
                    auto max = args[0].getInt();
                    if (max < 0 || max > std::numeric_limits<uint32_t>::max()) {
                        return Value::kNullBadData;
                    }
                    auto value = folly::Random::rand32(max);
                    return static_cast<int64_t>(static_cast<int32_t>(value));
                }
                return Value::kNullBadType;
            }
            DCHECK_EQ(2UL, args.size());
            if (args[0].isInt() && args[1].isInt()) {
                auto min = args[0].getInt();
                auto max = args[1].getInt();
                if (max < 0 || min < 0 || max > std::numeric_limits<uint32_t>::max() ||
                    min > std::numeric_limits<uint32_t>::max()) {
                    return Value::kNullBadData;
                }
                if (min >= max) {
                    return Value::kNullBadData;
                }
                return static_cast<int64_t>(folly::Random::rand32(min, max));
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::rand64 = [](const auto &args) -> Value {
            if (args.empty()) {
                return static_cast<int64_t>(folly::Random::rand64());
            } else if (args.size() == 1UL) {
                if (args[0].isInt()) {
                    auto max = args[0].getInt();
                    if (max < 0) {
                        return Value::kNullBadData;
                    }
                    return static_cast<int64_t>(folly::Random::rand64(max));
                }
                return Value::kNullBadType;
            }
            DCHECK_EQ(2UL, args.size());
            if (args[0].isInt() && args[1].isInt()) {
                auto min = args[0].getInt();
                auto max = args[1].getInt();
                if (max < 0 || min < 0 || min >= max) {
                    return Value::kNullBadData;
                }
                return static_cast<int64_t>(folly::Random::rand64(min, max));
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::now = [](const auto &args) -> Value {
            UNUSED(args);
            return time::WallClock::fastNowInSec();
        };

const FunctionManager::Function FunctionManager::strcasecmp = [](const auto &args) -> Value {
            if (args[0].isStr() && args[1].isStr()) {
                return static_cast<int64_t>(
                    ::strcasecmp(args[0].getStr().c_str(), args[1].getStr().c_str()));
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::lower = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                std::string value(args[0].getStr());
                folly::toLowerAscii(value);
                return value;
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::upper = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                std::string value(args[0].getStr());
                std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
                    return std::toupper(c);
                });
                return value;
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::length = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                auto value = args[0].getStr();
                return static_cast<int64_t>(value.length());
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::trim = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                std::string value(args[0].getStr());
                return folly::trimWhitespace(value);
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::ltrim = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                std::string value(args[0].getStr());
                return folly::ltrimWhitespace(value);
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::rtrim = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                std::string value(args[0].getStr());
                return folly::rtrimWhitespace(value);
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::left = [](const auto &args) -> Value {
            if (args[0].isStr() && args[1].isInt()) {
                auto value = args[0].getStr();
                auto length = args[1].getInt();
                if (length <= 0) {
                    return std::string();
                }
                return value.substr(0, length);
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::right = [](const auto &args) -> Value {
            if (args[0].isStr() && args[1].isInt()) {
                auto value = args[0].getStr();
                auto length = args[1].getInt();
                if (length <= 0) {
                    return std::string();
                }
                if (length > static_cast<int64_t>(value.size())) {
                    length = value.size();
                }
                return value.substr(value.size() - length);
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::lpad = [](const auto &args) -> Value {
            if (args[0].isStr() && args[1].isInt() && args[2].isStr()) {
                auto value = args[0].getStr();
                auto size = args[1].getInt();
                if (size < 0) {
                    return std::string("");
                } else if (size < static_cast<int64_t>(value.size())) {
                    return value.substr(0, static_cast<int32_t>(size));
                } else {
                    auto extra = args[2].getStr();
                    size -= value.size();
                    std::stringstream stream;
                    while (size > static_cast<int64_t>(extra.size())) {
                        stream << extra;
                        size -= extra.size();
                    }
                    stream << extra.substr(0, size);
                    stream << value;
                    return stream.str();
                }
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::rpad = [](const auto &args) -> Value {
            if (args[0].isStr() && args[1].isInt() && args[2].isStr()) {
                auto value = args[0].getStr();
                size_t size = args[1].getInt();
                if (size < 0) {
                    return std::string("");
                } else if (size < value.size()) {
                    return value.substr(0, static_cast<int32_t>(size));
                } else {
                    auto extra = args[2].getStr();
                    std::stringstream stream;
                    stream << value;
                    size -= value.size();
                    while (size > extra.size()) {
                        stream << extra;
                        size -= extra.size();
                    }
                    stream << extra.substr(0, size);
                    return stream.str();
                }
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::substr = [](const auto &args) -> Value {
            if (args[0].isStr() && args[1].isInt() && args[2].isInt()) {
                auto value = args[0].getStr();
                auto start = args[1].getInt();
                auto length = args[2].getInt();
                if (static_cast<size_t>(std::abs(start)) > value.size() || length <= 0 ||
                    start == 0) {
                    return std::string("");
                }
                if (start > 0) {
                    return value.substr(start - 1, length);
                } else {
                    return value.substr(value.size() + start, length);
                }
            }
            return Value::kNullBadType;
        };

const FunctionManager::Function FunctionManager::hash = [](const auto &args) -> Value {
            switch (args[0].type()) {
                case Value::Type::NULLVALUE:
                case Value::Type::__EMPTY__:
                case Value::Type::INT:
                case Value::Type::FLOAT:
                case Value::Type::BOOL:
                case Value::Type::STRING:
                case Value::Type::DATE:
                case Value::Type::DATETIME:
                case Value::Type::VERTEX:
                case Value::Type::EDGE:
                case Value::Type::PATH:
                case Value::Type::LIST: {
                    return static_cast<int64_t>(std::hash<nebula::Value>()(args[0]));
                }
                default:
                    LOG(ERROR) << "Hash has not been implemented for " << args[0].type();
                    return Value::kNullBadType;
            }
        };

const FunctionManager::Function FunctionManager::udf_is_in = [](const auto &args) -> Value {
            return std::find(args.begin() + 1, args.end(), args[0]) != args.end();
        };

const FunctionManager::Function FunctionManager::near = [](const auto &args) -> Value {
            // auto result = geo::GeoFilter::near(args);
            // if (!result.ok()) {
            //     return std::string("");
            // } else {
            //     return std::move(result).value();
            // }
            // TODO
            UNUSED(args);
            return std::string("");
        };

const FunctionManager::Function FunctionManager::cos_similarity = [](const auto &args) -> Value {
            if (args.size() % 2 != 0) {
                LOG(ERROR) << "The number of arguments must be even.";
                // value range of cos is [-1, 1]
                // it means error when we return -2
                return static_cast<double>(-2);
            }
            // sum(xi * yi) / (sqrt(sum(pow(xi))) + sqrt(sum(pow(yi))))
            auto mid = args.size() / 2;
            double s1 = 0, s2 = 0, s3 = 0;
            for (decltype(args.size()) i = 0; i < mid; ++i) {
                if (args[i].isNumeric() && args[i + mid].isNumeric()) {
                    auto xi = args[i].isInt() ? args[i].getInt() : args[i].getFloat();
                    auto yi =
                        args[i + mid].isInt() ? args[i + mid].getInt() : args[i + mid].getFloat();
                    s1 += (xi * yi);
                    s2 += (xi * xi);
                    s3 += (yi * yi);
                } else {
                    return Value::kNullBadType;
                }
            }
            if (std::abs(s2) <= kEpsilon || std::abs(s3) <= kEpsilon) {
                return static_cast<double>(-2);
            } else {
                return s1 / (std::sqrt(s2) * std::sqrt(s3));
            }
        };

const FunctionManager::Function FunctionManager::size = [](const auto &args) -> Value {
            switch (args[0].type()) {
                case Value::Type::NULLVALUE:
                    return Value::kNullValue;
                case Value::Type::__EMPTY__:
                    return Value::kEmpty;
                case Value::Type::STRING:
                    return static_cast<int64_t>(args[0].getStr().size());
                case Value::Type::LIST:
                    return static_cast<int64_t>(args[0].getList().size());
                case Value::Type::MAP:
                    return static_cast<int64_t>(args[0].getMap().size());
                case Value::Type::SET:
                    return static_cast<int64_t>(args[0].getSet().size());
                case Value::Type::DATASET:
                    return static_cast<int64_t>(args[0].getDataSet().size());
                case Value::Type::INT:
                case Value::Type::FLOAT:
                case Value::Type::BOOL:
                case Value::Type::DATE:
                case Value::Type::DATETIME:
                case Value::Type::VERTEX:
                case Value::Type::EDGE:
                case Value::Type::PATH:
                default:
                    LOG(ERROR) << "size() has not been implemented for " << args[0].type();
                    return Value::kNullBadType;
            }
        };

FunctionManager::FunctionManager() : functions_({
    {"", {
        // absolute value
        {"abs", {1, 1, abs}, {TypeSignature({Value::Type::INT}, Value::Type::INT),
                              TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
        // to nearest integer value not greater than x
        {"floor", {1, 1, floor}, {TypeSignature({Value::Type::INT}, Value::Type::INT),
                                  TypeSignature({Value::Type::FLOAT}, Value::Type::INT)}},
        // to nearest integer value not less than x
        {"ceil", {1, 1, ceil}, {TypeSignature({Value::Type::INT}, Value::Type::INT),
                                TypeSignature({Value::Type::FLOAT}, Value::Type::INT)}},
        // to nearest integer value
        {"round", {1, 1, round}, {TypeSignature({Value::Type::INT}, Value::Type::INT),
                                  TypeSignature({Value::Type::FLOAT}, Value::Type::INT)}},
        // square root
        {"sqrt", {1, 1, sqrt}, {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
                                TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
        // cubic root
        {"cbrt", {1, 1, cbrt}, {TypeSignature({Value::Type::INT}, Value::Type::INT),
                                TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
        // sqrt(x^2 + y^2)
        {"hypot", {2, 2, hypot}, {TypeSignature({Value::Type::INT, Value::Type::INT},
                                                 Value::Type::INT),
                                  TypeSignature({Value::Type::INT, Value::Type::FLOAT},
                                                 Value::Type::FLOAT),
                                  TypeSignature({Value::Type::FLOAT, Value::Type::INT},
                                                 Value::Type::FLOAT),
                                  TypeSignature({Value::Type::FLOAT, Value::Type::FLOAT},
                                                 Value::Type::FLOAT)}},
        // base^exp
        {"pow", {2, 2, pow}, {TypeSignature({Value::Type::INT, Value::Type::INT},
                                             Value::Type::INT),
                              TypeSignature({Value::Type::INT, Value::Type::FLOAT},
                                             Value::Type::FLOAT),
                              TypeSignature({Value::Type::FLOAT, Value::Type::INT},
                                             Value::Type::FLOAT),
                              TypeSignature({Value::Type::FLOAT, Value::Type::FLOAT},
                                             Value::Type::FLOAT)}},
        // e^x
        {"exp", {1, 1, exp}, {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
                              TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
        // 2^x
        {"exp2", {1, 1, exp2}, {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
                                TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
        // e-based logarithm
        {"log", {1, 1, log}, {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
                              TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
        // 2-based logarithm
        {"log2", {1, 1, log2}, {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
                                TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
        // 10-based logarithm
        {"log10", {1, 1, log10}, {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
                                  TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
        {"sin", {1, 1, sin}, {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
                              TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
        {"asin", {1, 1, asin}, {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
                                TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
        {"cos", {1, 1, cos}, {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
                              TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
        {"acos", {1, 1, acos}, {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
                                TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
        {"tan", {1, 1, tan}, {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
                              TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
        {"atan", {1, 1, atan}, {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
                                TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
        // rand32(), rand32(max), rand32(min, max)
        {"rand32", {0, 2, rand32}, {TypeSignature({}, Value::Type::INT),
                                    TypeSignature({Value::Type::INT}, Value::Type::INT),
                                    TypeSignature({Value::Type::INT, Value::Type::INT},
                                                   Value::Type::INT)}},
        // rand64(), rand64(max), rand64(min, max)
        {"rand64", {0, 2, rand64}, {TypeSignature({}, Value::Type::INT),
                                    TypeSignature({Value::Type::INT}, Value::Type::INT),
                                    TypeSignature({Value::Type::INT, Value::Type::INT},
                                                   Value::Type::INT)}},
        {"now", {0, 0, now}, {TypeSignature({}, Value::Type::INT)}},
        {"strcasecmp", {2, 2, strcasecmp}, {TypeSignature(
                                                {Value::Type::STRING, Value::Type::STRING},
                                                 Value::Type::STRING)}},
        {"lower", {1, 1, lower}, {TypeSignature({Value::Type::STRING}, Value::Type::STRING)}},
        {"upper", {1, 1, upper}, {TypeSignature({Value::Type::STRING}, Value::Type::STRING)}},
        {"length", {1, 1, length}, {TypeSignature({Value::Type::STRING}, Value::Type::INT)}},
        {"trim", {1, 1, trim}, {TypeSignature({Value::Type::STRING}, Value::Type::STRING)}},
        {"ltrim", {1, 1, ltrim}, {TypeSignature({Value::Type::STRING}, Value::Type::STRING)}},
        {"rtrim", {1, 1, rtrim}, {TypeSignature({Value::Type::STRING}, Value::Type::STRING)}},
        {"left", {2, 2, left}, {TypeSignature({Value::Type::STRING, Value::Type::INT},
                                               Value::Type::STRING)}},
        {"right", {2, 2, right}, {TypeSignature({Value::Type::STRING, Value::Type::INT},
                                                 Value::Type::STRING)}},
        {"lpad", {3, 3, lpad}, {TypeSignature(
                                    {Value::Type::STRING, Value::Type::INT, Value::Type::STRING},
                                     Value::Type::STRING)}},
        {"rpad", {3, 3, rpad}, {TypeSignature(
                                    {Value::Type::STRING, Value::Type::INT, Value::Type::STRING},
                                     Value::Type::STRING)}},
        {"substr", {3, 3, substr}, {TypeSignature(
                                        {Value::Type::STRING, Value::Type::INT, Value::Type::INT},
                                         Value::Type::STRING)}},
        {"hash", {1, 1, hash}, {TypeSignature({Value::Type::INT}, Value::Type::INT),
                                TypeSignature({Value::Type::FLOAT}, Value::Type::INT),
                                TypeSignature({Value::Type::STRING}, Value::Type::INT),
                                TypeSignature({Value::Type::NULLVALUE}, Value::Type::INT),
                                TypeSignature({Value::Type::__EMPTY__}, Value::Type::INT),
                                TypeSignature({Value::Type::BOOL}, Value::Type::INT),
                                TypeSignature({Value::Type::DATE}, Value::Type::INT),
                                TypeSignature({Value::Type::DATETIME}, Value::Type::INT),
                                TypeSignature({Value::Type::PATH}, Value::Type::INT),
                                TypeSignature({Value::Type::VERTEX}, Value::Type::INT),
                                TypeSignature({Value::Type::EDGE}, Value::Type::INT),
                                TypeSignature({Value::Type::LIST}, Value::Type::INT)}},
        {"udf_is_in", {2, INT64_MAX, udf_is_in}, {}},
        {"near", {2, 2, near}, {}},
        {"cos_similarity", {2, INT64_MAX, cos_similarity}, {}},
        {"size", {1, 1, size}, {TypeSignature({Value::Type::STRING}, Value::Type::INT),
                                TypeSignature({Value::Type::NULLVALUE}, Value::Type::NULLVALUE),
                                TypeSignature({Value::Type::__EMPTY__}, Value::Type::__EMPTY__),
                                TypeSignature({Value::Type::LIST}, Value::Type::INT),
                                TypeSignature({Value::Type::MAP}, Value::Type::INT),
                                TypeSignature({Value::Type::SET}, Value::Type::INT),
                                TypeSignature({Value::Type::DATASET}, Value::Type::INT), }}
    }}
}) {
}

// static
StatusOr<FunctionManager::Function> FunctionManager::get(const std::string &func, size_t arity) {
    return instance().getInternal(func, arity);
}

StatusOr<FunctionManager::Function> FunctionManager::getInternal(const std::string &func,
                                                                 size_t arity,
                                                                 const std::string &scope) const {
    // check existence
    auto result = functions_.find(scope, func);
    NG_RETURN_IF_ERROR(result);
    const auto iter = std::move(result).value();
    // check arity
    auto minArity = iter->second.minArity_;
    auto maxArity = iter->second.maxArity_;
    if (arity < minArity || arity > maxArity) {
        if (minArity == maxArity) {
            return Status::Error("Arity not match for function `%s': "
                                 "provided %lu but %lu expected.",
                                 func.c_str(),
                                 arity,
                                 minArity);
        } else {
            return Status::Error("Arity not match for function `%s': "
                                 "provided %lu but %lu-%lu expected.",
                                 func.c_str(),
                                 arity,
                                 minArity,
                                 maxArity);
        }
    }
    return iter->second.body_;
}

// static
Status FunctionManager::load(const std::string &name, const std::vector<std::string> &funcs) {
    return instance().loadInternal(name, funcs);
}

Status FunctionManager::loadInternal(const std::string &, const std::vector<std::string> &) {
    return Status::Error("Dynamic function loading not supported yet");
}

// static
Status FunctionManager::unload(const std::string &name, const std::vector<std::string> &funcs) {
    return instance().loadInternal(name, funcs);
}

Status FunctionManager::unloadInternal(const std::string &, const std::vector<std::string> &) {
    return Status::Error("Dynamic function unloading not supported yet");
}

}   // namespace nebula
