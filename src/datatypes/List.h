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

    List() = default;
    List(const List&) = default;
    List(List&&) = default;

    // Reuse the vector constructor
    /*implicit*/ List(const std::vector<Value> &l) : values(l) {}
    /*implicit*/ List(std::vector<Value> &&l) : values(std::move(l)) {}

    std::size_t size() const {
        return values.size();
    }

    bool empty() const {
        return values.empty();
    }

    void reserve(std::size_t n) {
        values.reserve(n);
    }

    void emplace_back(const Value &v) {
        values.emplace_back(v);
    }

    void emplace_back(Value &&v) {
        values.emplace_back(std::move(v));
    }

    void clear() {
        values.clear();
    }

    List& operator=(const List& rhs) {
        if (this == &rhs) { return *this; }
        values = rhs.values;
        return *this;
    }
    List& operator=(List&& rhs) {
        if (this == &rhs) { return *this; }
        values = std::move(rhs.values);
        return *this;
    }

    bool operator==(const List& rhs) const {
        return values == rhs.values;
    }
};

}  // namespace nebula
#endif  // DATATYPES_LIST_H_

