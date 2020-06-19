/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "FunctionManager.h"
#include "common/base/Base.h"
#include "common/expression/Expression.h"
#include "common/time/WallClock.h"
// #include "filter/geo/GeoFilter.h"

namespace nebula {

// static
FunctionManager &FunctionManager::instance() {
    static FunctionManager instance;
    return instance;
}

FunctionManager::FunctionManager() {
    {
        // absolute value
        auto &attr = functions_["abs"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return abs(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        // to nearest integer value not greater than x
        auto &attr = functions_["floor"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::floor(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        // to nearest integer value not less than x
        auto &attr = functions_["ceil"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::ceil(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        // to nearest integer value
        auto &attr = functions_["round"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::round(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        // square root
        auto &attr = functions_["sqrt"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::sqrt(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        // cubic root
        auto &attr = functions_["cbrt"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::cbrt(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        // sqrt(x^2 + y^2)
        auto &attr = functions_["hypot"];
        attr.minArity_ = 2;
        attr.maxArity_ = 2;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric() && args[1].isNumeric()) {
                auto x = args[0].isInt() ? args[0].getInt() : args[0].getFloat();
                auto y = args[1].isInt() ? args[1].getInt() : args[1].getFloat();
                return std::hypot(x, y);
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        // base^exp
        auto &attr = functions_["pow"];
        attr.minArity_ = 2;
        attr.maxArity_ = 2;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric() && args[1].isNumeric()) {
                auto base = args[0].isInt() ? args[0].getInt() : args[0].getFloat();
                auto exp = args[1].isInt() ? args[1].getInt() : args[1].getFloat();
                return std::pow(base, exp);
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        // e^x
        auto &attr = functions_["exp"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::exp(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        // 2^x
        auto &attr = functions_["exp2"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::exp2(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        // e-based logarithm
        auto &attr = functions_["log"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::log(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        // 2-based logarithm
        auto &attr = functions_["log2"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::log2(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        // 10-based logarithm
        auto &attr = functions_["log10"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::log10(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["sin"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::sin(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["asin"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::asin(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["cos"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::cos(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["acos"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::acos(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["tan"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::tan(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["atan"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::atan(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        // rand32(), rand32(max), rand32(min, max)
        auto &attr = functions_["rand32"];
        attr.minArity_ = 0;
        attr.maxArity_ = 2;
        attr.body_ = [](const auto &args) -> Value {
            if (args.empty()) {
                auto value = folly::Random::rand32();
                return static_cast<int64_t>(static_cast<int32_t>(value));
            } else if (args.size() == 1UL) {
                if (args[0].isInt()) {
                    auto max = args[0].getInt();
                    if (max < 0 || max > std::numeric_limits<uint32_t>::max()) {
                        return Value(NullType::BAD_TYPE);
                    }
                    auto value = folly::Random::rand32(max);
                    return static_cast<int64_t>(static_cast<int32_t>(value));
                }
                return Value(NullType::BAD_TYPE);
            }
            DCHECK_EQ(2UL, args.size());
            if (args[0].isInt() && args[1].isInt()) {
                auto min = args[0].getInt();
                auto max = args[1].getInt();
                if (max < 0 || min < 0 || max > std::numeric_limits<uint32_t>::max() ||
                    min > std::numeric_limits<uint32_t>::max()) {
                    return Value(NullType::BAD_TYPE);
                }
                if (min >= max) {
                    return Value(NullType::BAD_TYPE);
                }
                return static_cast<int64_t>(folly::Random::rand32(min, max));
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        // rand64(), rand64(max), rand64(min, max)
        auto &attr = functions_["rand64"];
        attr.minArity_ = 0;
        attr.maxArity_ = 2;
        attr.body_ = [](const auto &args) -> Value {
            if (args.empty()) {
                return static_cast<int64_t>(folly::Random::rand64());
            } else if (args.size() == 1UL) {
                if (args[0].isInt()) {
                    auto max = args[0].getInt();
                    if (max < 0) {
                        return Value(NullType::BAD_TYPE);
                    }
                    return static_cast<int64_t>(folly::Random::rand64(max));
                }
                return Value(NullType::BAD_TYPE);
            }
            DCHECK_EQ(2UL, args.size());
            if (args[0].isInt() && args[1].isInt()) {
                auto min = args[0].getInt();
                auto max = args[1].getInt();
                if (max < 0 || min < 0 || min >= max) {
                    return Value(NullType::BAD_TYPE);
                }
                return static_cast<int64_t>(folly::Random::rand64(min, max));
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        // unix timestamp
        auto &attr = functions_["now"];
        attr.minArity_ = 0;
        attr.maxArity_ = 0;
        attr.body_ = [](const auto &args) -> Value {
            UNUSED(args);
            return time::WallClock::fastNowInSec();
        };
    }
    {
        auto &attr = functions_["strcasecmp"];
        attr.minArity_ = 2;
        attr.maxArity_ = 2;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr() && args[1].isStr()) {
                return static_cast<int64_t>(
                    ::strcasecmp(args[0].getStr().c_str(), args[1].getStr().c_str()));
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["lower"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                auto value = const_cast<std::string &>(args[0].getStr());
                std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
                    return std::tolower(c);
                });
                return value;
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["upper"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                auto value = const_cast<std::string &>(args[0].getStr());
                std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
                    return std::toupper(c);
                });
                return value;
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["length"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                auto value = args[0].getStr();
                return static_cast<int64_t>(value.length());
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["trim"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                auto value = const_cast<std::string &>(args[0].getStr());
                value.erase(0, value.find_first_not_of(" "));
                value.erase(value.find_last_not_of(" ") + 1);
                return value;
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["ltrim"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                auto value = const_cast<std::string &>(args[0].getStr());
                value.erase(0, value.find_first_not_of(" "));
                return value;
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["rtrim"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                auto value = const_cast<std::string &>(args[0].getStr());
                value.erase(value.find_last_not_of(" ") + 1);
                return value;
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["left"];
        attr.minArity_ = 2;
        attr.maxArity_ = 2;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr() && args[1].isInt()) {
                auto value = args[0].getStr();
                auto length = args[1].getInt();
                if (length <= 0) {
                    return std::string();
                }
                return value.substr(0, length);
            }
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["right"];
        attr.minArity_ = 2;
        attr.maxArity_ = 2;
        attr.body_ = [](const auto &args) -> Value {
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
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["lpad"];
        attr.minArity_ = 3;
        attr.maxArity_ = 3;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr() && args[1].isInt()) {
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
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["rpad"];
        attr.minArity_ = 3;
        attr.maxArity_ = 3;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr() && args[1].isInt()) {
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
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        auto &attr = functions_["substr"];
        attr.minArity_ = 3;
        attr.maxArity_ = 3;
        attr.body_ = [](const auto &args) -> Value {
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
            return Value(NullType::BAD_TYPE);
        };
    }
    {
        // 64bit signed hash value
        auto &attr = functions_["hash"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            switch (args[0].type()) {
                case Value::Type::INT: {
                    auto v = args[0].getInt();
                    return static_cast<int64_t>(std::hash<int64_t>()(v));
                }
                case Value::Type::FLOAT: {
                    auto v = args[0].getFloat();
                    return static_cast<int64_t>(std::hash<double>()(v));
                }
                case Value::Type::BOOL: {
                    auto v = args[0].getBool();
                    return static_cast<int64_t>(std::hash<bool>()(v));
                }
                case Value::Type::STRING: {
                    auto &v = args[0].getStr();
                    return static_cast<int64_t>(std::hash<std::string>()(v));
                }
                default:
                    LOG(ERROR) << "UnKown type: " << args[0].type();
                    return Value(NullType::BAD_TYPE);
            }
        };
    }
    {
        auto &attr = functions_["udf_is_in"];
        attr.minArity_ = 2;
        attr.maxArity_ = INT64_MAX;
        attr.body_ = [](const auto &args) -> Value {
            switch (args[0].type()) {
                case Value::Type::INT: {
                    auto v = args[0].getInt();
                    std::unordered_set<uint64_t> vals;
                    for (auto iter = (args.begin() + 1); iter < args.end(); ++iter) {
                        vals.emplace((*iter).getInt());
                    }
                    auto ret = vals.emplace(v);
                    return !ret.second;
                }
                case Value::Type::FLOAT: {
                    auto v = args[0].getFloat();
                    std::unordered_set<double> vals;
                    for (auto iter = (args.begin() + 1); iter < args.end(); ++iter) {
                        vals.emplace((*iter).getFloat());
                    }
                    auto ret = vals.emplace(v);
                    return !ret.second;
                }
                case Value::Type::BOOL: {
                    auto v = args[0].getBool();
                    std::unordered_set<bool> vals;
                    for (auto iter = (args.begin() + 1); iter < args.end(); ++iter) {
                        vals.emplace((*iter).getBool());
                    }
                    auto ret = vals.emplace(v);
                    return !ret.second;
                }
                case Value::Type::STRING: {
                    auto v = args[0].getStr();
                    std::unordered_set<std::string> vals;
                    for (auto iter = (args.begin() + 1); iter < args.end(); ++iter) {
                        vals.emplace((*iter).getStr());
                    }
                    auto ret = vals.emplace(v);
                    return !ret.second;
                }
                default:
                    LOG(ERROR) << "UnKown type: " << args[0].type();
                    return Value(NullType::BAD_TYPE);
            }
        };
    }
    {
        auto &attr = functions_["near"];
        attr.minArity_ = 2;
        attr.maxArity_ = 2;
        attr.body_ = [](const auto &args) -> Value {
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
    }
    {
        auto &attr = functions_["cos_similarity"];
        attr.minArity_ = 2;
        attr.maxArity_ = INT64_MAX;
        attr.body_ = [](const auto &args) -> Value {
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
                auto xi = args[i].getFloat();
                auto yi = args[i + mid].getFloat();
                s1 += (xi * yi);
                s2 += (xi * xi);
                s3 += (yi * yi);
            }
            if (s2 == 0 || s3 == 0) {
                return static_cast<double>(-2);
            } else {
                return s1 / (std::sqrt(s2) * std::sqrt(s3));
            }
        };
    }
}   // NOLINT

// static
StatusOr<FunctionManager::Function> FunctionManager::get(const std::string &func, size_t arity) {
    return instance().getInternal(func, arity);
}

StatusOr<FunctionManager::Function> FunctionManager::getInternal(const std::string &func,
                                                                 size_t arity) const {
    auto status = Status::OK();
    folly::RWSpinLock::ReadHolder holder(lock_);
    // check existence
    auto iter = functions_.find(func);
    if (iter == functions_.end()) {
        return Status::Error("Function `%s' not defined", func.c_str());
    }
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
