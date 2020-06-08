/* Copyright (c) 2019 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_FUNCTION_AGGREGATEFUNCTION_H_
#define COMMON_FUNCTION_AGGREGATEFUNCTION_H_

#include "common/base/Base.h"
#include "common/datatypes/List.h"

namespace nebula {

constexpr char kCount[] = "COUNT";
constexpr char kCountDist[] = "COUNT_DISTINCT";
constexpr char kSum[] = "SUM";
constexpr char kAvg[] = "AVG";
constexpr char kMax[] = "MAX";
constexpr char kMin[] = "MIN";
constexpr char kStd[] = "STD";
constexpr char kBitAnd[] = "BIT_AND";
constexpr char kBitOr[] = "BIT_OR";
constexpr char kBitXor[] = "BIT_XOR";
constexpr char kCollect[] = "COLLECT";

class AggFun {
public:
    AggFun() {}
    virtual ~AggFun() {}

public:
    virtual void apply(const Value &val) = 0;
    virtual Value getResult() = 0;

    static std::unordered_map<std::string, std::function<std::shared_ptr<AggFun>()>> aggFunMap_;
};


class Group final : public AggFun {
public:
    void apply(const Value &val) override {
        col_ = val;
    }

    Value getResult() override {
        return col_;
    }

private:
    Value   col_;
};


class Count final : public AggFun {
public:
    void apply(const Value &val) override {
        UNUSED(val);
        cnt_ = cnt_ + 1;
    }

    Value getResult() override {
        return cnt_;
    }

private:
    Value   cnt_{0};
};


class Sum final : public AggFun {
public:
    void apply(const Value &val) override {
        if (sum_.type() == Value::Type::__EMPTY__) {
            sum_ = val;
        } else {
            // TODO: Support += for value.
            sum_ = sum_ + val;
        }
    }

    Value getResult() override {
        return sum_;
    }

private:
    Value   sum_;
};


class Avg final : public AggFun {
public:
    void apply(const Value &val) override {
        cnt_ = cnt_ + 1;
        avg_ = avg_ + (val - avg_) / cnt_;
    }

    Value getResult() override {
        return avg_;
    }

private:
    Value               avg_{0.0};
    Value               cnt_{0.0};
};


class CountDistinct final : public AggFun {
public:
    void apply(const Value &val) override {
        valueSet_.emplace(val);
    }

    Value getResult() override {
        int64_t count = static_cast<int64_t>(valueSet_.size());
        return Value(count);
    }

private:
    std::unordered_set<Value> valueSet_;
};


class Max final : public AggFun {
public:
    void apply(const Value &val) override {
        if (max_.type() == Value::Type::__EMPTY__ || val > max_) {
            max_ = val;
        }
    }

    Value getResult() override {
        return max_;
    }

private:
    Value     max_;
};


class Min final : public AggFun {
public:
    void apply(const Value &val) override {
        if (min_.type() == Value::Type::__EMPTY__ || val < min_) {
            min_ = val;
        }
    }

    Value getResult() override {
        return min_;
    }

private:
    Value       min_;
};


class Stdev final : public AggFun {
public:
    void apply(const Value &val) override {
        cnt_ = cnt_ + 1;
        var_ = (cnt_ - 1) / (cnt_ * cnt_) * ((val - avg_) * (val - avg_))
            + (cnt_ - 1) / cnt_ * var_;
        avg_ = avg_ + (val - avg_) / cnt_;
    }

    Value getResult() override {
        return std::sqrt(var_.getFloat());
    }

private:
    Value   avg_{0.0};
    Value   cnt_{0.0};
    Value   var_{0.0};
};


class BitAnd final : public AggFun {
public:
    void apply(const Value &val) override {
        UNUSED(val);
        // TODO : implement the bit and for value;
    }

    Value getResult() override {
        // TODO
        return kEmpty;
    }
};


class BitOr final : public AggFun {
public:
    void apply(const Value &val) override {
        UNUSED(val);
        // TODO : implement the bit or for value;
    }

    Value getResult() override {
        // TODO
        return kEmpty;
    }
};


class BitXor final : public AggFun {
public:
    void apply(const Value &val) override {
        UNUSED(val);
        // TODO : implement the bit xor for value;
    }

    Value getResult() override {
        // TODO
        return kEmpty;
    }
};

class Collect final : public AggFun {
public:
    void apply(const Value &val) override {
        list_.values.emplace_back(val);
    }

    Value getResult() override {
        return Value(std::move(list_));
    }

private:
    List    list_;
};


}  // namespace nebula

#endif  // COMMON_FUNCTION_AGGREGATEFUNCTION_H_
