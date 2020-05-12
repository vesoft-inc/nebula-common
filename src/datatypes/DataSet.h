/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef DATATYPES_DATASET_H_
#define DATATYPES_DATASET_H_

#include "base/Base.h"
#include "datatypes/Value.h"

namespace nebula {

struct Row {
    std::vector<Value> columns;

    Row() = default;
    Row(const Row& r) noexcept {
        columns = r.columns;
    }
    Row(Row&& r) noexcept {
        columns = std::move(r.columns);
    }

    Row& operator=(const Row& r) noexcept {
        columns = r.columns;
        return *this;
    }
    Row& operator=(Row&& r) noexcept {
        columns = std::move(r.columns);
        return *this;
    }

    void emplace_back(Value&& v) {
        columns.emplace_back(std::move(v));
    }

    void emplace_back(Value& v) {
        columns.emplace_back(v);
    }

    void clear() {
        columns.clear();
    }

    bool operator==(const Row& rhs) const {
        return columns == rhs.columns;
    }
};

struct DataSet {
    std::vector<std::string> colNames;
    std::vector<Row> rows;

    DataSet() = default;
    explicit DataSet(std::vector<std::string>&& colNames_) : colNames(std::move(colNames_)) {}
    DataSet(const DataSet& ds) noexcept {
        colNames = ds.colNames;
        rows = ds.rows;
    }
    DataSet(DataSet&& ds) noexcept {
        colNames = std::move(ds.colNames);
        rows = std::move(ds.rows);
    }
    DataSet& operator=(const DataSet& ds) noexcept {
        colNames = ds.colNames;
        rows = ds.rows;
        return *this;
    }
    DataSet& operator=(DataSet&& ds) noexcept {
        colNames = std::move(ds.colNames);
        rows = std::move(ds.rows);
        return *this;
    }

    // append the DataSet to one
    bool append(DataSet&& o) {
        if (colNames != o.colNames) {
            return false;
        }
        rows.reserve(o.rows.size());
        rows.insert(rows.end(),
                    std::make_move_iterator(o.rows.begin()),
                    std::make_move_iterator(o.rows.end()));
        return true;
    }

    void clear() {
        colNames.clear();
        rows.clear();
    }

    bool operator==(const DataSet& rhs) const {
        return colNames == rhs.colNames && rows == rhs.rows;
    }
};

}   // namespace nebula
#endif   // DATATYPES_DATASET_H_
