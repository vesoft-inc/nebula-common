/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef DATATYPES_LIST_H_
#define DATATYPES_LIST_H_

#include "base/Base.h"
#include "datatypes/Value.h"

namespace nebula {

struct List {
    std::vector<Value> values;

    List()            = default;
    List(const List&) = default;
    List(List&&)      = default;

    void clear() {
        values.clear();
    }

    List& operator=(const List& rhs) {
        if (this == &rhs) {
            return *this;
        }
        values = rhs.values;
        return *this;
    }
    List& operator=(List&& rhs) {
        if (this == &rhs) {
            return *this;
        }
        values = std::move(rhs.values);
        return *this;
    }

    bool operator==(const List& rhs) const {
        return values == rhs.values;
    }
};

}   // namespace nebula
#endif   // DATATYPES_LIST_H_
