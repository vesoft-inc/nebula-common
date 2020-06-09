/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/function/AggregateFunction.h"

namespace nebula {
std::unordered_map<AggFun::Function,
                   std::function<std::shared_ptr<AggFun>()>> AggFun::aggFunMap_  = {
    { AggFun::Function::kNone, []() -> auto { return std::make_shared<Group>();} },
    { AggFun::Function::kCount, []() -> auto { return std::make_shared<Count>();} },
    { AggFun::Function::kCountDist, []() -> auto { return std::make_shared<CountDistinct>();} },
    { AggFun::Function::kSum, []() -> auto { return std::make_shared<Sum>();} },
    { AggFun::Function::kAvg, []() -> auto { return std::make_shared<Avg>();} },
    { AggFun::Function::kMax, []() -> auto { return std::make_shared<Max>();} },
    { AggFun::Function::kMin, []() -> auto { return std::make_shared<Min>();} },
    { AggFun::Function::kStdev, []() -> auto { return std::make_shared<Stdev>();} },
    { AggFun::Function::kBitAnd, []() -> auto { return std::make_shared<BitAnd>();} },
    { AggFun::Function::kBitOr, []() -> auto { return std::make_shared<BitOr>();} },
    { AggFun::Function::kBitXor, []() -> auto { return std::make_shared<BitXor>();} },
    { AggFun::Function::kCollect, []() -> auto { return std::make_shared<Collect>();} },
};

}  // namespace nebula
