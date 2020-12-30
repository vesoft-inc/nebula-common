/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */
#include "common/expression/AggregateExpression.h"
#include "common/expression/ExprVisitor.h"

namespace nebula {

std::unordered_map<std::string, AggregateExpression::Function> AggregateExpression::nameIdMap_ = {
    {"", AggregateExpression::Function::kNone},
    {"COUNT", AggregateExpression::Function::kCount},
    {"SUM", AggregateExpression::Function::kSum},
    {"AVG", AggregateExpression::Function::kAvg},
    {"MAX", AggregateExpression::Function::kMax},
    {"MIN", AggregateExpression::Function::kMin},
    {"STD", AggregateExpression::Function::kStdev},
    {"BIT_AND", AggregateExpression::Function::kBitAnd},
    {"BIT_OR", AggregateExpression::Function::kBitOr},
    {"BIT_XOR", AggregateExpression::Function::kBitXor},
    {"COLLECT", AggregateExpression::Function::kCollect},
    {"COLLECT_SET", AggregateExpression::Function::kCollectSet}
};

bool AggregateExpression::operator==(const Expression& rhs) const {
    if (kind_ != rhs.kind()) {
        return false;
    }

    const auto& r = static_cast<const AggregateExpression&>(rhs);
    return *name_ == *(r.name_) && *arg_ == *(r.arg_);
}

void AggregateExpression::writeTo(Encoder& encoder) const {
    encoder << kind_;
    encoder << name_.get();
    encoder << Value(distinct_);
    if (arg_) {
        encoder << *arg_;
    }
}

void AggregateExpression::resetFrom(Decoder& decoder) {
    name_ = decoder.readStr();
    distinct_ = decoder.readValue().getBool();
    arg_ = decoder.readExpression();
}

const Value& AggregateExpression::eval(ExpressionContext& ctx) {
    DCHECK(!!aggData_);
    auto val = arg_->eval(ctx);
    auto uniques = aggData_->uniques();
    if (distinct_) {
        if (uniques->contains(val)) {
            return aggData_->result();
        }
        uniques->values.emplace(val);
    }

    apply_(aggData_, val);

    return aggData_->result();
}

std::string AggregateExpression::toString() const {
    std::string arg(arg_->toString());
    std::string isDistinct;
    if (distinct_) {
        isDistinct = "distinct ";
    }
    std::stringstream out;
    out << *name_ << "(" << isDistinct << arg << ")";
    return out.str();
}

void AggregateExpression::accept(ExprVisitor* visitor) {
    visitor->visit(this);
}

std::unordered_map<AggregateExpression::Function,
                   std::function<void(AggData*, const Value&)>>
    AggregateExpression::aggFunMap_  = {
    {
        AggregateExpression::Function::kNone,
        [](AggData* result, const Value& val) {
            result->setResult(val);
        }
    },
    {
        AggregateExpression::Function::kCount,
        [](AggData* result, const Value& val) {
            auto& res = result->result();
            if (res.isBadNull()) {
                return;
            }
            if (res.isNull()) {
                res = 0;
            }
            if (val.isNull() || val.empty()) {
                return;
            }

            res = res + 1;
        }
    },
    {
        AggregateExpression::Function::kSum,
        [](AggData* result, const Value& val) {
            auto& res = result->result();
            if (res.isBadNull()) {
                return;
            }
            if (UNLIKELY(!val.isNull() && !val.isNumeric())) {
                res = Value::kNullBadType;
                return;
            }

            if (val.isNull() || val.empty()) {
                return;
            }
            if (res.isNull()) {
                res = val;
                return;
            }
            res = res + val;
        }
    },
    {
        AggregateExpression::Function::kAvg,
        [](AggData* result, const Value& val) {
            auto& res = result->result();
            if (res.isBadNull()) {
                return;
            }
            if (UNLIKELY(!val.isNull() && !val.isNumeric())) {
                res = Value::kNullBadType;
                return;
            }

            auto& sum = result->sum();
            auto& cnt = result->cnt();
            if (val.isNull() || val.empty()) {
                return;
            }
            if (res.isNull()) {
                res = 0.0;
                sum = 0.0;
                cnt = 0.0;
            }

            sum = sum + val;
            cnt = cnt + 1;
            res = sum / cnt;
        }
    },
    {
        AggregateExpression::Function::kMax,
        [](AggData* result, const Value& val) {
            auto& res = result->result();
            if (res.isBadNull()) {
                return;
            }
            if (UNLIKELY(!val.isNull() && !val.isNumeric())) {
                res = Value::kNullBadType;
                return;
            }
            if (val.isNull() || val.empty()) {
                return;
            }
            if (res.isNull()) {
                res = val;
                return;
            }

            if (val > res) {
                res = val;
            }
        }
    },
    {
        AggregateExpression::Function::kMin,
        [](AggData* result, const Value& val) {
            auto& res = result->result();
            if (res.isBadNull()) {
                return;
            }
            if (UNLIKELY(!val.isNull() && !val.isNumeric())) {
                res = Value::kNullBadType;
                return;
            }
            if (val.isNull() || val.empty()) {
                return;
            }
            if (res.isNull()) {
                res = val;
                return;
            }
            if (val < res) {
                res = val;
            }
        }
    },
    {
        AggregateExpression::Function::kStdev,
        [](AggData* result, const Value& val) {
            auto& res = result->result();
            if (res.isBadNull()) {
                return;
            }
            if (UNLIKELY(!val.isNull() && !val.isNumeric())) {
                res = Value::kNullBadType;
                return;
            }
            if (val.isNull() || val.empty()) {
                return;
            }

            auto& cnt = result->cnt();
            auto& avg = result->avg();
            auto& deviation = result->deviation();
            if (res.isNull()) {
                res = 0.0;
                cnt = 0.0;
                avg = 0.0;
                deviation = 0.0;
            }

            cnt = cnt + 1;
            deviation = (cnt - 1) / (cnt * cnt) * ((val - avg) * (val - avg))
                + (cnt - 1) / cnt * deviation;
            avg = avg + (val - avg) / cnt;
            auto stdev = deviation.isFloat() ?
                         std::sqrt(deviation.getFloat()) : Value::kNullBadType;
            res = stdev;
        }
    },
    {
        AggregateExpression::Function::kBitAnd,
        [](AggData* result, const Value& val) {
            auto& res = result->result();
            if (res.isBadNull()) {
                return;
            }
            if (UNLIKELY(!val.isNull() && !val.isInt())) {
                res = Value::kNullBadType;
                return;
            }
            if (val.isNull() || val.empty()) {
                return;
            }
            if (res.isNull() && val.isInt()) {
                res = val;
                return;
            }

            res = res & val;
        }
    },
    {
        AggregateExpression::Function::kBitOr,
        [](AggData* result, const Value& val) {
            auto& res = result->result();
            if (res.isBadNull()) {
                return;
            }
            if (UNLIKELY(!val.isNull() && !val.isNumeric())) {
                res = Value::kNullBadType;
                return;
            }
            if (val.isNull() || val.empty()) {
                return;
            }
            if (res.isNull() && val.isInt()) {
                res = val;
                return;
            }

            res = res | val;
        }
    },
    {
        AggregateExpression::Function::kBitXor,
        [](AggData* result, const Value& val) {
            auto& res = result->result();
            if (res.isBadNull()) {
                return;
            }
            if (UNLIKELY(!val.isNull() && !val.isNumeric())) {
                res = Value::kNullBadType;
                return;
            }
            if (val.isNull() || val.empty()) {
                return;
            }
            if (res.isNull() && val.isInt()) {
                res = val;
                return;
            }

            res = res ^ val;
        }
    },
    {
        AggregateExpression::Function::kCollect,
        [](AggData* result, const Value& val) {
            auto& res = result->result();
            if (res.isBadNull()) {
                return;
            }
            if (res.isNull()) {
                res = List();
            }
            if (val.isNull() || val.empty()) {
                return;
            }
            if (!res.isList()) {
                res = Value::kNullBadData;
                return;
            }
            auto &list = res.mutableList();
            list.emplace_back(val);
        }
    },
    {
        AggregateExpression::Function::kCollectSet,
        [](AggData* result, const Value& val) {
            auto& res = result->result();
            if (res.isBadNull()) {
                return;
            }
            if (res.isNull()) {
                res = Set();
            }
            if (val.isNull() || val.empty()) {
                return;
            }

            if (!res.isSet()) {
                res = Value::kNullBadData;
                return;
            }
            auto& set = res.mutableSet();
            set.values.emplace(val);
        }
    }
};

}  // namespace nebula
