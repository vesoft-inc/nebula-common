/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_ALGORITHM_HASHRANGE_H_
#define COMMON_ALGORITHM_HASHRANGE_H_

#include "common/base/Base.h"

namespace nebula {
namespace algorithm {
template <class It, class T>
inline std::size_t hashRange(It first, It last) {
    std::size_t seed = 0;

    for (; first != last; ++first) {
        seed ^= std::hash<T>()(*first) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    return seed;
}
}  // namespace algorithm
}  // namespace nebula
#endif  // COMMON_ALGORITHM_HASHRANGE_H_
