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

class AggFun {
public:
    enum class Function : uint8_t {
        kNone,
        kCount,
        kCountDist,
        kSum,
        kAvg,
        kMax,
        kMin,
        kStdev,
        kBitAnd,
        kBitOr,
        kBitXor,
        kCollect
    };
    AggFun() {}
    virtual ~AggFun() {}

public:
    virtual void apply(const Value &val) = 0;
    virtual Value getResult() = 0;

    static std::unordered_map<Function, std::function<std::unique_ptr<AggFun>()>> aggFunMap_;
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
        if (val.isNull() || val.empty()) {
            return;
        }
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
        if (val.isNull() || val.empty()) {
            return;
        }
        if (UNLIKELY(!val.isNumeric())) {
            sum_ = Value(NullType::BAD_TYPE);
            return;
        }

        if (sum_.isNull()) {
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
    Value   sum_{NullType::__NULL__};
};


class Avg final : public AggFun {
public:
    void apply(const Value &val) override {
        if (val.isNull() || val.empty()) {
            return;
        }
        if (UNLIKELY(!val.isNumeric())) {
            avg_ = Value(NullType::BAD_TYPE);
            return;
        }
        cnt_ = cnt_ + 1;
        avg_ = avg_ + (val - avg_) / cnt_;
    }

    Value getResult() override {
        return cnt_ > 0 ? avg_ : Value::kNullValue;
    }

private:
    Value               avg_{0.0};
    Value               cnt_{0.0};
};


class CountDistinct final : public AggFun {
public:
    void apply(const Value &val) override {
        if (val.isNull() || val.empty()) {
            return;
        }
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
        if (val.isNull() || val.empty()) {
            return;
        }
        if (max_.isNull() || val > max_) {
            max_ = val;
        }
    }

    Value getResult() override {
        return max_;
    }

private:
    Value     max_{NullType::__NULL__};
};


class Min final : public AggFun {
public:
    void apply(const Value &val) override {
        if (val.isNull() || val.empty()) {
            return;
        }
        if (min_.isNull() || val < min_) {
            min_ = val;
        }
    }

    Value getResult() override {
        return min_;
    }

private:
    Value       min_{NullType::__NULL__};
};


class Stdev final : public AggFun {
public:
    void apply(const Value &val) override {
        if (val.isNull() || val.empty()) {
            return;
        }
        if (UNLIKELY(!val.isNumeric())) {
            avg_ = Value(NullType::BAD_TYPE);
            var_ = Value(NullType::BAD_TYPE);
            return;
        }
        cnt_ = cnt_ + 1;
        var_ = (cnt_ - 1) / (cnt_ * cnt_) * ((val - avg_) * (val - avg_))
            + (cnt_ - 1) / cnt_ * var_;
        avg_ = avg_ + (val - avg_) / cnt_;
    }

    Value getResult() override {
        if (cnt_ <= 0) {
            return Value::kNullValue;
        } else if (var_.empty() || var_.isNull()) {
            return var_;
        } else {
            return std::sqrt(var_.getFloat());
        }
    }

private:
    Value   avg_{0.0};
    Value   cnt_{0.0};
    Value   var_{0.0};
};


class BitAnd final : public AggFun {
public:
    void apply(const Value &val) override {
        if (val.isNull() || val.empty()) {
            return;
        }
        if (result_.isNull()) {
            result_ = val;
        } else {
            result_  = result_ & val;
        }
    }

    Value getResult() override {
        return result_;
    }

private:
    Value result_{NullType::__NULL__};
};


class BitOr final : public AggFun {
public:
    void apply(const Value &val) override {
        if (val.isNull() || val.empty()) {
            return;
        }
        if (result_.isNull()) {
            result_ = val;
        } else {
            result_  = result_ | val;
        }
    }

    Value getResult() override {
        return result_;
    }

private:
    Value result_{NullType::__NULL__};
};


class BitXor final : public AggFun {
public:
    void apply(const Value &val) override {
        if (val.isNull() || val.empty()) {
            return;
        }
        if (result_.isNull()) {
            result_ = val;
        } else {
            result_  = result_ ^ val;
        }
    }

    Value getResult() override {
        return result_;
    }

private:
    Value result_{NullType::__NULL__};
};

class Collect final : public AggFun {
public:
    void apply(const Value &val) override {
        if (val.isNull() || val.empty()) {
            return;
        }
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
