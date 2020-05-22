/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef DATATYPES_SET_H_
#define DATATYPES_SET_H_

#include "base/Base.h"
#include "datatypes/Value.h"

namespace nebula {

struct Set {
    std::unordered_set<Value> values;

    Set() = default;
    Set(const Set&) = default;
    Set(Set&&) = default;

    void clear() {
        values.clear();
    }

    Set& operator=(const Set& rhs) {
        if (this == &rhs) {
            return *this;
        }
        values = rhs.values;
        return *this;
    }
    Set& operator=(Set&& rhs) {
        if (this == &rhs) {
            return *this;
        }
        values = std::move(rhs.values);
        return *this;
    }

    bool operator==(const Set& rhs) const {
        return values == rhs.values;
    }
};

}   // namespace nebula
#endif   // DATATYPES_SET_H_
