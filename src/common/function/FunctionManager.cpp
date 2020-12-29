/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "FunctionManager.h"

#include "common/base/Base.h"
#include "common/datatypes/DataSet.h"
#include "common/datatypes/Edge.h"
#include "common/datatypes/List.h"
#include "common/datatypes/Map.h"
#include "common/datatypes/Path.h"
#include "common/datatypes/Set.h"
#include "common/datatypes/Vertex.h"
#include "common/expression/Expression.h"
#include "common/thrift/ThriftTypes.h"
#include "common/time/TimeUtils.h"
#include "common/time/WallClock.h"

namespace nebula {

// static
FunctionManager &FunctionManager::instance() {
    static FunctionManager instance;
    return instance;
}

std::unordered_map<std::string, std::vector<TypeSignature>> FunctionManager::typeSignature_ = {
    {"abs",
     {TypeSignature({Value::Type::INT}, Value::Type::INT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
    {"floor",
     {TypeSignature({Value::Type::INT}, Value::Type::INT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::INT)}},
    {"ceil",
     {TypeSignature({Value::Type::INT}, Value::Type::INT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::INT)}},
    {"round",
     {TypeSignature({Value::Type::INT}, Value::Type::INT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::INT)}},
    {"sqrt",
     {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
    {"cbrt",
     {TypeSignature({Value::Type::INT}, Value::Type::INT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
    {"hypot",
     {TypeSignature({Value::Type::INT, Value::Type::INT}, Value::Type::INT),
      TypeSignature({Value::Type::INT, Value::Type::FLOAT}, Value::Type::FLOAT),
      TypeSignature({Value::Type::FLOAT, Value::Type::INT}, Value::Type::FLOAT),
      TypeSignature({Value::Type::FLOAT, Value::Type::FLOAT}, Value::Type::FLOAT)}},
    {"pow",
     {TypeSignature({Value::Type::INT, Value::Type::INT}, Value::Type::INT),
      TypeSignature({Value::Type::INT, Value::Type::FLOAT}, Value::Type::FLOAT),
      TypeSignature({Value::Type::FLOAT, Value::Type::INT}, Value::Type::FLOAT),
      TypeSignature({Value::Type::FLOAT, Value::Type::FLOAT}, Value::Type::FLOAT)}},
    {"exp",
     {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
    {"exp2",
     {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
    {"log",
     {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
    {"log2",
     {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
    {"log10",
     {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
    {"sin",
     {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
    {"cos",
     {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
    {"asin",
     {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
    {"acos",
     {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
    {"tan",
     {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
    {"atan",
     {TypeSignature({Value::Type::INT}, Value::Type::FLOAT),
      TypeSignature({Value::Type::FLOAT}, Value::Type::FLOAT)}},
    {"rand32",
     {TypeSignature({}, Value::Type::INT),
      TypeSignature({Value::Type::INT}, Value::Type::INT),
      TypeSignature({Value::Type::INT, Value::Type::INT}, Value::Type::INT)}},
    {"rand64",
     {TypeSignature({}, Value::Type::INT),
      TypeSignature({Value::Type::INT}, Value::Type::INT),
      TypeSignature({Value::Type::INT, Value::Type::INT}, Value::Type::INT)}},
    {"now", {TypeSignature({}, Value::Type::INT)}},
    {"strcasecmp",
     {TypeSignature({Value::Type::STRING, Value::Type::STRING}, Value::Type::STRING)}},
    {"lower", {TypeSignature({Value::Type::STRING}, Value::Type::STRING)}},
    {"toLower", {TypeSignature({Value::Type::STRING}, Value::Type::STRING)}},
    {"upper", {TypeSignature({Value::Type::STRING}, Value::Type::STRING)}},
    {"toUpper", {TypeSignature({Value::Type::STRING}, Value::Type::STRING)}},
    {"length", {TypeSignature({Value::Type::STRING}, Value::Type::INT),
                TypeSignature({Value::Type::PATH}, Value::Type::INT), }},
    {"trim", {TypeSignature({Value::Type::STRING}, Value::Type::STRING)}},
    {"ltrim", {TypeSignature({Value::Type::STRING}, Value::Type::STRING)}},
    {"rtrim", {TypeSignature({Value::Type::STRING}, Value::Type::STRING)}},
    {"left", {TypeSignature({Value::Type::STRING, Value::Type::INT}, Value::Type::STRING)}},
    {"right", {TypeSignature({Value::Type::STRING, Value::Type::INT}, Value::Type::STRING)}},
    {"replace", {TypeSignature({Value::Type::STRING,
            Value::Type::STRING, Value::Type::STRING}, Value::Type::STRING)}},
    {"reverse", {TypeSignature({Value::Type::STRING}, Value::Type::STRING),
                 TypeSignature({Value::Type::LIST}, Value::Type::LIST)}},
    {"split", {TypeSignature({Value::Type::STRING, Value::Type::STRING}, Value::Type::LIST)}},
    {"lpad",
     {TypeSignature({Value::Type::STRING, Value::Type::INT, Value::Type::STRING},
                    Value::Type::STRING)}},
    {"rpad",
     {TypeSignature({Value::Type::STRING, Value::Type::INT, Value::Type::STRING},
                    Value::Type::STRING)}},
    {"substr",
     {TypeSignature({Value::Type::STRING, Value::Type::INT, Value::Type::INT}, Value::Type::STRING),
            TypeSignature({Value::Type::STRING, Value::Type::INT}, Value::Type::STRING)}},
    {"substring",
     {TypeSignature({Value::Type::STRING, Value::Type::INT, Value::Type::INT}, Value::Type::STRING),
            TypeSignature({Value::Type::STRING, Value::Type::INT}, Value::Type::STRING)}},
    {"toString", {TypeSignature({Value::Type::INT}, Value::Type::STRING),
                  TypeSignature({Value::Type::FLOAT}, Value::Type::STRING),
                  TypeSignature({Value::Type::STRING}, Value::Type::STRING),
                  TypeSignature({Value::Type::BOOL}, Value::Type::STRING),
                  TypeSignature({Value::Type::DATE}, Value::Type::STRING),
                  TypeSignature({Value::Type::TIME}, Value::Type::STRING),
                  TypeSignature({Value::Type::DATETIME}, Value::Type::STRING)
                }},
    {"hash", {TypeSignature({Value::Type::INT}, Value::Type::INT),
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
    {"size", {TypeSignature({Value::Type::STRING}, Value::Type::INT),
              TypeSignature({Value::Type::NULLVALUE}, Value::Type::NULLVALUE),
              TypeSignature({Value::Type::__EMPTY__}, Value::Type::__EMPTY__),
              TypeSignature({Value::Type::LIST}, Value::Type::INT),
              TypeSignature({Value::Type::MAP}, Value::Type::INT),
              TypeSignature({Value::Type::SET}, Value::Type::INT),
              TypeSignature({Value::Type::DATASET}, Value::Type::INT),
             }},
    {"time", {TypeSignature({}, Value::Type::TIME),
              TypeSignature({Value::Type::STRING}, Value::Type::TIME),
              TypeSignature({Value::Type::MAP}, Value::Type::TIME)}},
    {"date", {TypeSignature({}, Value::Type::DATE),
              TypeSignature({Value::Type::STRING}, Value::Type::DATE),
              TypeSignature({Value::Type::MAP}, Value::Type::DATE)}},
    {"datetime", {TypeSignature({}, Value::Type::DATETIME),
              TypeSignature({Value::Type::STRING}, Value::Type::DATETIME),
              TypeSignature({Value::Type::MAP}, Value::Type::DATETIME)}},
    {"timestamp", {TypeSignature({Value::Type::STRING}, Value::Type::INT),
                   TypeSignature({Value::Type::INT}, Value::Type::INT)}},
    {"id", {TypeSignature({Value::Type::VERTEX}, Value::Type::STRING),
             }},
    {"tags", {TypeSignature({Value::Type::VERTEX}, Value::Type::LIST),
             }},
    {"labels", {TypeSignature({Value::Type::VERTEX}, Value::Type::LIST),
             }},
    {"properties", {TypeSignature({Value::Type::VERTEX}, Value::Type::MAP),
                    TypeSignature({Value::Type::EDGE}, Value::Type::MAP),
             }},
    {"type", {TypeSignature({Value::Type::EDGE}, Value::Type::STRING),
             }},
    {"rank", {TypeSignature({Value::Type::EDGE}, Value::Type::INT),
             }},
    {"startNode", {TypeSignature({Value::Type::EDGE}, Value::Type::VERTEX),
                   TypeSignature({Value::Type::PATH}, Value::Type::VERTEX), }},
    {"endNode", {TypeSignature({Value::Type::EDGE}, Value::Type::VERTEX),
                 TypeSignature({Value::Type::PATH}, Value::Type::VERTEX), }},
    {"keys", {TypeSignature({Value::Type::VERTEX}, Value::Type::LIST),
              TypeSignature({Value::Type::EDGE}, Value::Type::LIST),
              TypeSignature({Value::Type::MAP}, Value::Type::LIST), }},
    {"nodes", {TypeSignature({Value::Type::PATH}, Value::Type::LIST), }},
    {"tail", {TypeSignature({Value::Type::LIST}, Value::Type::LIST), }},
    {"relationships", {TypeSignature({Value::Type::PATH}, Value::Type::LIST), }},
    {"head", {TypeSignature({Value::Type::LIST}, Value::Type::__EMPTY__), }},
    {"last", { TypeSignature({Value::Type::LIST}, Value::Type::__EMPTY__), }},
    {"coalesce", { TypeSignature({Value::Type::LIST}, Value::Type::__EMPTY__), }},
    {"range",
     {TypeSignature({Value::Type::INT, Value::Type::INT}, Value::Type::LIST),
      TypeSignature({Value::Type::INT, Value::Type::INT, Value::Type::INT}, Value::Type::LIST)}},
    {"hasSameEdgeInPath", { TypeSignature({Value::Type::PATH}, Value::Type::BOOL), }},
    {"hasSameVertexInPath", {TypeSignature({Value::Type::PATH}, Value::Type::BOOL), }},
    {"reversePath", {TypeSignature({Value::Type::PATH}, Value::Type::PATH), }},
};

// static
StatusOr<Value::Type>
FunctionManager::getReturnType(const std::string &funName,
                               const std::vector<Value::Type> &argsType) {
    auto iter = typeSignature_.find(funName);
    if (iter == typeSignature_.end()) {
        return Status::Error("Function `%s' not defined", funName.c_str());
    }

    for (const auto &args : iter->second) {
        if (argsType == args.argsType_) {
            return args.returnType_;
        }
    }

    for (auto &argType : argsType) {
        // Most functions do not accept NULL or EMPTY
        // but if the parameters are given by NULL or EMPTY ,
        // then we will tell that it returns NULL or EMPTY
        if (argType == Value::Type::__EMPTY__ || argType == Value::Type::NULLVALUE) {
            return argType;
        }
    }

    return Status::Error("Parameter's type error");
}

FunctionManager::FunctionManager() {
    {
        // absolute value
        auto &attr = functions_["abs"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                if (args[0].isInt()) {
                    return std::abs(args[0].getInt());
                } else {
                    return std::abs(args[0].getFloat());
                }
            }
            return Value::kNullBadType;
        };
    }
    {
        // to nearest integer value not greater than x
        auto &attr = functions_["floor"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                if (args[0].isInt()) {
                    return std::floor(args[0].getInt());
                } else {
                    return std::floor(args[0].getFloat());
                }
            }
            return Value::kNullBadType;
        };
    }
    {
        // to nearest integer value not less than x
        auto &attr = functions_["ceil"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                if (args[0].isInt()) {
                    return std::ceil(args[0].getInt());
                } else {
                    return std::ceil(args[0].getFloat());
                }
            }
            return Value::kNullBadType;
        };
    }
    {
        // to nearest integer value
        auto &attr = functions_["round"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                if (args[0].isInt()) {
                    return std::round(args[0].getInt());
                } else {
                    return std::round(args[0].getFloat());
                }
            }
            return Value::kNullBadType;
        };
    }
    {
        // square root
        auto &attr = functions_["sqrt"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                if (args[0].isInt()) {
                    return std::sqrt(args[0].getInt());
                } else {
                    return std::sqrt(args[0].getFloat());
                }
            }
            return Value::kNullBadType;
        };
    }
    {
        // cubic root
        auto &attr = functions_["cbrt"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::cbrt(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };
    }
    {
        // sqrt(x^2 + y^2)
        auto &attr = functions_["hypot"];
        attr.minArity_ = 2;
        attr.maxArity_ = 2;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric() && args[1].isNumeric()) {
                auto x = args[0].isInt() ? args[0].getInt() : args[0].getFloat();
                auto y = args[1].isInt() ? args[1].getInt() : args[1].getFloat();
                return std::hypot(x, y);
            }
            return Value::kNullBadType;
        };
    }
    {
        // base^exp
        auto &attr = functions_["pow"];
        attr.minArity_ = 2;
        attr.maxArity_ = 2;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric() && args[1].isNumeric()) {
                auto base = args[0].isInt() ? args[0].getInt() : args[0].getFloat();
                auto exp = args[1].isInt() ? args[1].getInt() : args[1].getFloat();
                auto val = std::pow(base, exp);
                if (args[0].isInt() && args[1].isInt()) {
                    return static_cast<int64_t>(val);
                } else {
                    return val;
                }
            }
            return Value::kNullBadType;
        };
    }
    {
        // e^x
        auto &attr = functions_["exp"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                if (args[0].isInt()) {
                    return std::exp(args[0].getInt());
                } else {
                    return std::exp(args[0].getFloat());
                }
            }
            return Value::kNullBadType;
        };
    }
    {
        // 2^x
        auto &attr = functions_["exp2"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                if (args[0].isInt()) {
                    return std::exp2(args[0].getInt());
                } else {
                    return std::exp2(args[0].getFloat());
                }
            }
            return Value::kNullBadType;
        };
    }
    {
        // e-based logarithm
        auto &attr = functions_["log"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                if (args[0].isInt()) {
                    return std::log(args[0].getInt());
                } else {
                    return std::log(args[0].getFloat());
                }
            }
            return Value::kNullBadType;
        };
    }
    {
        // 2-based logarithm
        auto &attr = functions_["log2"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                if (args[0].isInt()) {
                    return std::log2(args[0].getInt());
                } else {
                    return std::log2(args[0].getFloat());
                }
            }
            return Value::kNullBadType;
        };
    }
    {
        // 10-based logarithm
        auto &attr = functions_["log10"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::log10(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["sin"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::sin(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["asin"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::asin(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["cos"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::cos(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["acos"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::acos(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["tan"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::tan(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["atan"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isNumeric()) {
                return std::atan(args[0].isInt() ? args[0].getInt() : args[0].getFloat());
            }
            return Value::kNullBadType;
        };
    }
    {
        // rand32(), rand32(max), rand32(min, max)
        auto &attr = functions_["rand32"];
        attr.minArity_ = 0;
        attr.maxArity_ = 2;
        attr.isPure_ = false;
        attr.body_ = [](const auto &args) -> Value {
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
    }
    {
        // rand64(), rand64(max), rand64(min, max)
        auto &attr = functions_["rand64"];
        attr.minArity_ = 0;
        attr.maxArity_ = 2;
        attr.isPure_ = false;
        attr.body_ = [](const auto &args) -> Value {
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
    }
    {
        // unix timestamp
        auto &attr = functions_["now"];
        attr.minArity_ = 0;
        attr.maxArity_ = 0;
        attr.isPure_ = false;
        attr.body_ = [](const auto &args) -> Value {
            UNUSED(args);
            return time::WallClock::fastNowInSec();
        };
    }
    {
        auto &attr = functions_["strcasecmp"];
        attr.minArity_ = 2;
        attr.maxArity_ = 2;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr() && args[1].isStr()) {
                return static_cast<int64_t>(
                    ::strcasecmp(args[0].getStr().c_str(), args[1].getStr().c_str()));
            }
            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["lower"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                std::string value(args[0].getStr());
                folly::toLowerAscii(value);
                return value;
            }
            return Value::kNullBadType;
        };
        functions_["toLower"] = attr;
    }
    {
        auto &attr = functions_["upper"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                std::string value(args[0].getStr());
                std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
                    return std::toupper(c);
                });
                return value;
            }
            return Value::kNullBadType;
        };
        functions_["toUpper"] = attr;
    }
    {
        auto &attr = functions_["length"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                auto value = args[0].getStr();
                return static_cast<int64_t>(value.length());
            }
            if (args[0].isPath()) {
                auto path = args[0].getPath();
                return static_cast<int64_t>(path.steps.size());
            }
            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["trim"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                std::string value(args[0].getStr());
                return folly::trimWhitespace(value).toString();
            }
            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["ltrim"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                std::string value(args[0].getStr());
                return folly::ltrimWhitespace(value).toString();
            }
            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["rtrim"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                std::string value(args[0].getStr());
                return folly::rtrimWhitespace(value).toString();
            }
            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["left"];
        attr.minArity_ = 2;
        attr.maxArity_ = 2;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
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
    }
    {
        auto &attr = functions_["right"];
        attr.minArity_ = 2;
        attr.maxArity_ = 2;
        attr.isPure_ = true;
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
            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["replace"];
        attr.minArity_ = 3;
        attr.maxArity_ = 3;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr() && args[1].isStr() && args[2].isStr()) {
                std::string origStr(args[0].getStr());
                std::string search(args[1].getStr());
                std::string newStr(args[2].getStr());
                return boost::replace_all_copy(origStr, search, newStr);
            }
            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["reverse"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr()) {
                std::string origStr(args[0].getStr());
                std::reverse(origStr.begin(), origStr.end());
                return origStr;
            } else if (args[0].isList()) {
                auto& list = args[0].getList();
                List result(list.values);
                std::reverse(result.values.begin(), result.values.end());
                return result;
            }

            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["split"];
        attr.minArity_ = 2;
        attr.maxArity_ = 2;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isStr() && args[1].isStr()) {
                std::string origStr(args[0].getStr());
                std::string delim(args[1].getStr());
                List res;
                std::vector<folly::StringPiece> substrings;
                folly::split<folly::StringPiece>(delim, origStr, substrings);
                for (auto str : substrings) {
                    res.emplace_back(str.toString());
                }
                return res;
            }
            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["toString"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
             switch (args[0].type()) {
                case Value::Type::NULLVALUE:
                    return Value::kNullBadType;
                case Value::Type::INT: {
                    return folly::to<std::string>(args[0].getInt());
                }
                case Value::Type::FLOAT: {
                    auto str = folly::to<std::string>(args[0].getFloat());
                    std::size_t found = str.find('.');
                    if (found == std::string::npos) {
                        str += ".0";
                    }
                    return str;
                }
                case Value::Type::BOOL: {
                    return args[0].getBool() ? "true" : "false";
                }
                case Value::Type::STRING: {
                    return args[0].getStr();
                }
                case Value::Type::DATE: {
                    return args[0].getDate().toString();
                }
                case Value::Type::TIME: {
                    return args[0].getTime().toString();
                }
                case Value::Type::DATETIME: {
                    return args[0].getDateTime().toString();
                }
                default:
                    LOG(ERROR) << "toString has not been implemented for " << args[0].type();
                    return Value::kNullBadType;
            }
        };
    }
    {
        auto &attr = functions_["lpad"];
        attr.minArity_ = 3;
        attr.maxArity_ = 3;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
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
    }
    {
        auto &attr = functions_["rpad"];
        attr.minArity_ = 3;
        attr.maxArity_ = 3;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
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
    }
    {
        auto &attr = functions_["substr"];
        attr.minArity_ = 2;
        attr.maxArity_ = 3;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            auto argSize = args.size();
            if (argSize < 2 || argSize >3) {
                LOG(ERROR) <<  "Unexpected arguments count " << args.size();
                return Value::kNullBadData;
            }
            auto value = args[0].getStr();
            auto start = args[1].getInt();
            auto length =  (args.size() == 2) ? value.size() - start : args[2].getInt();
            if (args[0].isStr() && args[1].isInt()) {
                if (argSize == 3) {
                    if (!args[2].isInt()) {
                        return Value::kNullBadType;
                    }
                }
                if (static_cast<size_t>(std::abs(start)) > value.size() || length == 0) {
                    return std::string("");
                }
                if (start < 0) {
                    LOG(ERROR) << "Invalid Start index " << start;
                    return Value::kNullBadData;
                }
                if (start == 0) {
                    return value;
                }
                return value.substr(start, length);
            }
            return Value::kNullBadType;
        };
        functions_["substring"] = attr;
    }
    {
        // 64bit signed hash value
        auto &attr = functions_["hash"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
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
    }
    {
        auto &attr = functions_["udf_is_in"];
        attr.minArity_ = 2;
        attr.maxArity_ = INT64_MAX;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            return std::find(args.begin() + 1, args.end(), args[0]) != args.end();
        };
    }
    {
        auto &attr = functions_["near"];
        attr.minArity_ = 2;
        attr.maxArity_ = 2;
        attr.isPure_ = true;
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
        attr.isPure_ = true;
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
    }
    {
        auto &attr = functions_["size"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
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
    }
    {
        auto &attr = functions_["date"];
        // 0 for corrent time
        // 1 for string or map
        attr.minArity_ = 0;
        attr.maxArity_ = 1;
        attr.isPure_ = false;
        attr.body_ = [](const auto &args) -> Value {
            switch (args.size()) {
                case 0: {
                    auto result = time::TimeUtils::utcDate();
                    if (!result.ok()) {
                        return Value::kNullBadData;
                    }
                    return Value(std::move(result).value());
                }
                case 1: {
                    if (args[0].isStr()) {
                        auto result = time::TimeUtils::parseDate(args[0].getStr());
                        if (!result.ok()) {
                            return Value::kNullBadData;
                        }
                        return time::TimeUtils::dateToUTC(result.value());
                    } else if (args[0].isMap()) {
                        auto result = time::TimeUtils::dateFromMap(args[0].getMap());
                        if (!result.ok()) {
                            return Value::kNullBadData;
                        }
                        return time::TimeUtils::dateToUTC(result.value());
                    } else {
                        return Value::kNullBadType;
                    }
                }
                default:
                    LOG(FATAL) << "Unexpected arguments count " << args.size();
            }
        };
    }
    {
        auto &attr = functions_["time"];
        // 0 for corrent time
        // 1 for string or map
        attr.minArity_ = 0;
        attr.maxArity_ = 1;
        attr.isPure_ = false;
        attr.body_ = [](const auto &args) -> Value {
            switch (args.size()) {
                case 0: {
                    auto result = time::TimeUtils::utcTime();
                    if (!result.ok()) {
                        return Value::kNullBadData;
                    }
                    return Value(std::move(result).value());
                }
                case 1: {
                    if (args[0].isStr()) {
                        auto result = time::TimeUtils::parseTime(args[0].getStr());
                        if (!result.ok()) {
                            return Value::kNullBadData;
                        }
                        return time::TimeUtils::timeToUTC(result.value());
                    } else if (args[0].isMap()) {
                        auto result = time::TimeUtils::timeFromMap(args[0].getMap());
                        if (!result.ok()) {
                            return Value::kNullBadData;
                        }
                        return time::TimeUtils::timeToUTC(result.value());
                    } else {
                        return Value::kNullBadType;
                    }
                }
                default:
                    LOG(FATAL) << "Unexpected arguments count " << args.size();
            }
        };
    }
    {
        auto &attr = functions_["datetime"];
        // 0 for corrent time
        // 1 for string or map
        attr.minArity_ = 0;
        attr.maxArity_ = 1;
        attr.isPure_ = false;
        attr.body_ = [](const auto &args) -> Value {
            switch (args.size()) {
                case 0: {
                    auto result = time::TimeUtils::utcDateTime();
                    if (!result.ok()) {
                        return Value::kNullBadData;
                    }
                    return Value(std::move(result).value());
                }
                case 1: {
                    if (args[0].isStr()) {
                        auto result = time::TimeUtils::parseDateTime(args[0].getStr());
                        if (!result.ok()) {
                            return Value::kNullBadData;
                        }
                        return time::TimeUtils::dateTimeToUTC(result.value());
                    } else if (args[0].isMap()) {
                        auto result = time::TimeUtils::dateTimeFromMap(args[0].getMap());
                        if (!result.ok()) {
                            return Value::kNullBadData;
                        }
                        return time::TimeUtils::dateTimeToUTC(result.value());
                    } else {
                        return Value::kNullBadData;
                    }
                }
                default:
                    LOG(FATAL) << "Unexpected arguments count " << args.size();
            }
        };
    }
    {
        auto &attr = functions_["timestamp"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [](const auto &args) -> Value {
            auto status = time::TimeUtils::toTimestamp(args[0]);
            if (!status.ok()) {
                return Value::kNullBadData;
            }
            return status.value();
        };
    }
    {
        auto &attr = functions_["range"];
        attr.minArity_ = 2;
        attr.maxArity_ = 3;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (!args[0].isInt() || !args[1].isInt()) {
                return Value::kNullBadType;
            }

            int64_t start = args[0].getInt();
            int64_t end = args[1].getInt();
            int64_t step = 1;
            if (args.size() == 3) {
                if (!args[2].isInt()) {
                    return Value::kNullBadType;
                }
                step = args[2].getInt();
            }
            if (step == 0) {
                return Value::kNullBadData;
            }

            List res;
            for (auto i = start; step > 0? i <= end : i >= end; i = i + step) {
                res.emplace_back(i);
            }
            return Value(res);
        };
    }
    {
        auto &attr = functions_["id"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (!args[0].isVertex()) {
                return Value::kNullBadType;
            }
            return args[0].getVertex().vid;
        };
    }
    {
        auto &attr = functions_["tags"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (!args[0].isVertex()) {
                return Value::kNullBadType;
            }
            List tags;
            for (auto &tag : args[0].getVertex().tags) {
                tags.emplace_back(tag.name);
            }
            return tags;
        };
        functions_["labels"] = attr;
    }
    {
        auto &attr = functions_["properties"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isVertex()) {
                Map props;
                for (auto &tag : args[0].getVertex().tags) {
                    props.kvs.insert(tag.props.cbegin(), tag.props.cend());
                }
                return Value(std::move(props));
            } else if (args[0].isEdge()) {
                Map props;
                props.kvs = args[0].getEdge().props;
                return Value(std::move(props));
            } else {
                return Value::kNullBadType;
            }
        };
    }
    {
        auto &attr = functions_["type"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (!args[0].isEdge()) {
                return Value::kNullBadType;
            }
            return args[0].getEdge().name;
        };
    }
    {
        auto &attr = functions_["src"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (!args[0].isEdge()) {
                return Value::kNullBadType;
            }
            return args[0].getEdge().src;
        };
    }
    {
        auto &attr = functions_["dst"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (!args[0].isEdge()) {
                return Value::kNullBadType;
            }
            return args[0].getEdge().dst;
        };
    }
    {
        auto &attr = functions_["rank"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (!args[0].isEdge()) {
                return Value::kNullBadType;
            }
            return args[0].getEdge().ranking;
        };
    }
    {
        auto &attr = functions_["startNode"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isEdge()) {
                return Vertex(args[0].getEdge().src, {});
            }
            if (args[0].isPath()) {
                return args[0].getPath().src;
            }
            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["endNode"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (args[0].isEdge()) {
                return Vertex(args[0].getEdge().dst, {});
            }
            if (args[0].isPath()) {
                auto &path = args[0].getPath();
                if (path.steps.empty()) {
                    return path.src;
                }
                return path.steps.back().dst;
            }
            return Value::kNullBadType;
        };
    }
    {
        auto &attr = functions_["head"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (!args[0].isList()) {
                return Value::kNullBadType;
            }
            return args[0].getList().values.front();
        };
    }
    {
        auto &attr = functions_["last"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (!args[0].isList()) {
                return Value::kNullBadType;
            }
            return args[0].getList().values.back();
        };
    }
    {
        auto &attr = functions_["coalesce"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (!args[0].isList()) {
                return Value::kNullBadType;
            }
            auto& list = args[0].getList();
            if (list.values.empty()) {
                return Value::kNullValue;
            }
            for (auto& i : list.values) {
                if (i != Value::kNullValue) {
                    return i;
                }
            }
            return Value::kNullValue;
        };
    }
    {
        auto &attr = functions_["keys"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            std::set<std::string> tmp;
            if (args[0].isVertex()) {
                for (auto& tag : args[0].getVertex().tags) {
                    for (auto& prop : tag.props) {
                        tmp.emplace(prop.first);
                    }
                }
            } else if (args[0].isEdge()) {
                for (auto& prop : args[0].getEdge().props) {
                    tmp.emplace(prop.first);
                }
            } else if (args[0].isMap()) {
                for (auto& kv : args[0].getMap().kvs) {
                    tmp.emplace(kv.first);
                }
            } else {
                return Value::kNullBadType;
            }
            List result;
            result.values.assign(tmp.cbegin(), tmp.cend());
            return result;
        };
    }
    {
        auto &attr = functions_["nodes"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (!args[0].isPath()) {
                return Value::kNullBadType;
            }
            auto& path = args[0].getPath();
            List result;
            result.emplace_back(path.src);
            for (auto& step : path.steps) {
                result.emplace_back(step.dst);
            }
            return result;
        };
    }
    {
        auto &attr = functions_["tail"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (!args[0].isList()) {
                return Value::kNullBadType;
            }
            auto& list = args[0].getList();
            if (list.empty()) {
                return List();
            }
            List result(std::vector<Value>(list.values.begin()+1, list.values.end()));
            return result;
        };
    }
    {
        auto &attr = functions_["relationships"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (!args[0].isPath()) {
                return Value::kNullBadType;
            }
            auto& path = args[0].getPath();
            List result;
            auto src = path.src.vid;
            for (size_t i = 0; i < path.steps.size(); ++i) {
                Edge edge;
                edge.src = src;
                edge.dst = path.steps[i].dst.vid;
                edge.type = path.steps[i].type;
                edge.name = path.steps[i].name;
                edge.ranking = path.steps[i].ranking;
                edge.props = path.steps[i].props;

                src = edge.dst;
                result.values.emplace_back(std::move(edge));
            }
            return result;
        };
    }
    {
        auto &attr = functions_["hasSameEdgeInPath"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (!args[0].isPath()) {
                return Value::kNullBadType;
            }
            auto &path = args[0].getPath();
            return path.hasDuplicateEdges();
        };
    }
    {
        auto &attr = functions_["hasSameVertexInPath"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (!args[0].isPath()) {
                return Value::kNullBadType;
            }
            auto &path = args[0].getPath();
            return path.hasDuplicateVertices();
        };
    }
    {
        auto &attr = functions_["reversePath"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.isPure_ = true;
        attr.body_ = [](const auto &args) -> Value {
            if (!args[0].isPath()) {
                return Value::kNullBadType;
            }
            auto path = args[0].getPath();
            path.reverse();
            return path;
        };
    }
}   // NOLINT

// static
StatusOr<FunctionManager::Function> FunctionManager::get(const std::string &func, size_t arity) {
    auto result = instance().getInternal(func, arity);
    NG_RETURN_IF_ERROR(result);
    return result.value().body_;
}

/*static*/ StatusOr<bool> FunctionManager::getIsPure(const std::string &func, size_t arity) {
    auto result = instance().getInternal(func, arity);
    NG_RETURN_IF_ERROR(result);
    return result.value().isPure_;
}

/*static*/ StatusOr<const FunctionManager::FunctionAttributes>
FunctionManager::getInternal(const std::string &func,
                             size_t arity) const {
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
    return iter->second;
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
