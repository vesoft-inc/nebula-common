/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/function/AggregateFunction.h"

namespace nebula {
std::unordered_map<std::string, std::function<std::shared_ptr<AggFun>()>> AggFun::aggFunMap_  = {
    { "", []() -> auto { return std::make_shared<Group>();} },
    { kCount, []() -> auto { return std::make_shared<Count>();} },
    { kCountDist, []() -> auto { return std::make_shared<CountDistinct>();} },
    { kSum, []() -> auto { return std::make_shared<Sum>();} },
    { kAvg, []() -> auto { return std::make_shared<Avg>();} },
    { kMax, []() -> auto { return std::make_shared<Max>();} },
    { kMin, []() -> auto { return std::make_shared<Min>();} },
    { kStd, []() -> auto { return std::make_shared<Stdev>();} },
    { kBitAnd, []() -> auto { return std::make_shared<BitAnd>();} },
    { kBitOr, []() -> auto { return std::make_shared<BitOr>();} },
    { kBitXor, []() -> auto { return std::make_shared<BitXor>();} },
    { kCollect, []() -> auto { return std::make_shared<Collect>();} },
};

}  // namespace nebula
