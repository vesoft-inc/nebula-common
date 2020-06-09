/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/function/AggregateFunction.h"

namespace nebula {
std::unordered_map<AggFun::Function,
                   std::function<std::unique_ptr<AggFun>()>> AggFun::aggFunMap_  = {
    { AggFun::Function::kNone, []() -> auto { return std::make_unique<Group>();} },
    { AggFun::Function::kCount, []() -> auto { return std::make_unique<Count>();} },
    { AggFun::Function::kCountDist, []() -> auto { return std::make_unique<CountDistinct>();} },
    { AggFun::Function::kSum, []() -> auto { return std::make_unique<Sum>();} },
    { AggFun::Function::kAvg, []() -> auto { return std::make_unique<Avg>();} },
    { AggFun::Function::kMax, []() -> auto { return std::make_unique<Max>();} },
    { AggFun::Function::kMin, []() -> auto { return std::make_unique<Min>();} },
    { AggFun::Function::kStdev, []() -> auto { return std::make_unique<Stdev>();} },
    { AggFun::Function::kBitAnd, []() -> auto { return std::make_unique<BitAnd>();} },
    { AggFun::Function::kBitOr, []() -> auto { return std::make_unique<BitOr>();} },
    { AggFun::Function::kBitXor, []() -> auto { return std::make_unique<BitXor>();} },
    { AggFun::Function::kCollect, []() -> auto { return std::make_unique<Collect>();} },
};

}  // namespace nebula
