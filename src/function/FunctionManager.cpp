/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "base/Base.h"
#include "function/FunctionManager.h"
#include "time/WallClock.h"
#include "datatypes/List.h"
#include "datatypes/DataSet.h"

namespace nebula {

// static
FunctionManager& FunctionManager::instance() {
    static FunctionManager instance;
    return instance;
}


FunctionManager::FunctionManager() {
    {
        // amalgamating multiple records or values into a single list.
        auto &attr = functions_["collect"];
        attr.minArity_ = 1;
        attr.maxArity_ = 1;
        attr.body_ = [] (const auto &args) {
            auto& val = args[0];
            if (UNLIKELY(val.type() != Value::Type::DATASET)) {
                return Value(NullType::BAD_TYPE);
            }

            auto& rows = val.getDataSet().rows;
            List list;
            list.values.reserve(rows.size());
            for (auto& row : rows) {
                auto cols = row.columns;
                List tmp(std::move(cols));
                list.values.emplace_back(std::move(tmp));
            }
            return Value(std::move(list));
        };
    }
}


// static
StatusOr<FunctionManager::Function>
FunctionManager::get(const std::string &func, size_t arity) {
    return instance().getInternal(func, arity);
}


StatusOr<FunctionManager::Function>
FunctionManager::getInternal(const std::string &func, size_t arity) const {
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
                                 func.c_str(), arity, minArity);
        } else {
            return Status::Error("Arity not match for function `%s': "
                                 "provided %lu but %lu-%lu expected.",
                                 func.c_str(), arity, minArity, maxArity);
        }
    }
    return iter->second.body_;
}

// static
Status FunctionManager::load(const std::string &name,
                             const std::vector<std::string> &funcs) {
    return instance().loadInternal(name, funcs);
}


Status FunctionManager::loadInternal(const std::string &,
                                     const std::vector<std::string> &) {
    return Status::Error("Dynamic function loading not supported yet");
}


// static
Status FunctionManager::unload(const std::string &name,
                               const std::vector<std::string> &funcs) {
    return instance().loadInternal(name, funcs);
}


Status FunctionManager::unloadInternal(const std::string &,
                                       const std::vector<std::string> &) {
    return Status::Error("Dynamic function unloading not supported yet");
}

}   // namespace nebula
