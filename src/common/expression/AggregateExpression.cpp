/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */
#include "common/expression/AggregateExpression.h"
#include "common/expression/ExprVisitor.h"

namespace nebula {
std::unordered_map<AggregateExpression::Function,
                   std::function<void(AggData*, const Value&)>>
                   AggregateExpression::aggFunMap_  = {
    {
      AggregateExpression::Function::kNone,
      [](AggData* result, const Value& val) {
          result->setRes(val);
      }
    },
    {
        AggregateExpression::Function::kCount,
        [](AggData* result, const Value& val) {
            if (val.isNull() || val.empty()) {
                return;
            }
            auto res = result->res();
            if (res.isBadNull()) {
                return;
            }

            if (res.empty()) {
                // TBD: 0 error
                result->setRes(1);
                return;
            }
            result->setRes(res + 1);
        }
    },
    {
        AggregateExpression::Function::kSum,
        [](AggData* result, const Value& val) {
            if (val.isNull() || val.empty()) {
                return;
            }
            if (UNLIKELY(!val.isNumeric())) {
                result->setRes(Value::kNullBadType);
                return;
            }
            auto res = result->res();
            if (res.isBadNull()) {
                return;
            }

            if (res.empty()) {
                result->setRes(val);
                return;
            }
            result->setRes(res + val);
        }
    },
    {
        AggregateExpression::Function::kAvg,
        [](AggData* result, const Value& val) {
            if (val.isNull() || val.empty()) {
                return;
            }
            if (UNLIKELY(!val.isNumeric())) {
                result->setRes(Value::kNullBadType);
                return;
            }
            auto res = result->res();
            if (res.isBadNull()) {
                return;
            }

            if (res.empty()) {
                result->setRes(val);
                result->setSum(val);
                result->setCnt(1);
                return;
            }
            auto sum = result->sum();
            auto cnt = result->cnt();

            result->setSum(sum + val);
            result->setCnt(cnt + 1);
            result->setRes((sum + val)/ (cnt + 1));
        }
    },
    {
        AggregateExpression::Function::kMax,
        [](AggData* result, const Value& val) {
            if (val.isNull() || val.empty()) {
                return;
            }
            if (UNLIKELY(!val.isNumeric())) {
                result->setRes(Value::kNullBadType);
                return;
            }
            auto res = result->res();
            if (res.isBadNull()) {
                return;
            }

            if (res.empty()) {
                result->setRes(val);
                return;
            }
            if (val > res) {
                result->setRes(val);
            }
        }
    },
    {
        AggregateExpression::Function::kMin,
        [](AggData* result, const Value& val) {
            if (val.isNull() || val.empty()) {
                return;
            }
            if (UNLIKELY(!val.isNumeric())) {
                result->setRes(Value::kNullBadType);
                return;
            }
            auto res = result->res();
            if (res.isBadNull()) {
                return;
            }

            if (res.empty()) {
                result->setRes(val);
                return;
            }
            if (val < res) {
                result->setRes(val);
            }
        }
    },
    {
        AggregateExpression::Function::kStdev,
        [](AggData* result, const Value& val) {
            if (val.isNull() || val.empty()) {
                return;
            }
            if (UNLIKELY(!val.isNumeric())) {
                result->setRes(Value::kNullBadType);
                return;
            }
            auto res = result->res();
            if (res.isBadNull()) {
                return;
            }

            if (res.empty()) {
                result->setRes(0.0);
                result->setCnt(1);
                result->setAvg(val);
                return;
            }

            auto cnt = result->cnt();
            result->setCnt(cnt + 1);
            auto avg = result->avg();
            auto old_deviation = result->deviation();
            auto new_deviation = (cnt - 1) / (cnt * cnt) * ((val - avg) * (val - avg))
                + (cnt - 1) / cnt * old_deviation;
            avg = avg + (val - avg) / cnt;
            auto stdev = new_deviation.isFloat() ?
                         std::sqrt(new_deviation.getFloat()) : Value::kNullBadType;
            result->setRes(stdev);
        }
    },
    {
        AggregateExpression::Function::kBitAnd,
        [](AggData* result, const Value& val) {
            if (val.isNull() || val.empty()) {
                return;
            }
            if (!val.isInt()) {
                result->setRes(Value::kNullBadType);
                return;
            }
            auto res = result->res();
            if (res.isBadNull()) {
                return;
            }

            if (res.empty()) {
                result->setRes(val);
                return;
            }
            result->setRes(res & val);
        }
    },
    {
        AggregateExpression::Function::kBitOr,
        [](AggData* result, const Value& val) {
            if (val.isNull() || val.empty()) {
                return;
            }
            if (!val.isInt()) {
                result->setRes(Value::kNullBadType);
                return;
            }
            auto res = result->res();
            if (res.isBadNull()) {
                return;
            }

            if (res.empty()) {
                result->setRes(val);
                return;
            }
            result->setRes(res | val);
        }
    },
    {
        AggregateExpression::Function::kBitXor,
        [](AggData* result, const Value& val) {
            if (val.isNull() || val.empty()) {
                return;
            }
            if (!val.isInt()) {
                result->setRes(Value::kNullBadType);
                return;
            }
            auto res = result->res();
            if (res.isBadNull()) {
                return;
            }

            if (res.empty()) {
                result->setRes(val);
                return;
            }
            result->setRes(res ^ val);
        }
    },
    {
        AggregateExpression::Function::kCollect,
        [](AggData* result, const Value& val) {
            auto& res = result->res();
            if (res.isBadNull()) {
                return;
            }
            if (res.empty()) {
                res = List();
            }
            if (val.isNull() || val.empty()) {
                return;
            }
            if (!res.isList()) {
                res = Value::kNullBadData;
                return;
            }
            auto list = res.getList();
            list.emplace_back(val);
            result->setRes(list);
        }
    },
    {
        AggregateExpression::Function::kCollectSet,
        [](AggData* result, const Value& val) {
            auto& res = result->res();
            if (res.isBadNull()) {
                return;
            }
            if (res.empty()) {
                res = Set();
            }
            if (val.isNull() || val.empty()) {
                return;
            }

            if (!res.isSet()) {
                res = Value::kNullBadData;
                return;
            }
            auto set = res.getSet();
            set.values.emplace(val);
            result->setRes(set);
        }
    }
};

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
    // TODO : verify
    // kind_
    encoder << kind_;

    // name_
    encoder << name_.get();

    // distinct_
    encoder << distinct_;

    // arg_
    if (arg_) {
        encoder << *arg_;
    }
}


void AggregateExpression::resetFrom(Decoder& decoder) {
    // TODO : verify
    // Read name_
    name_ = decoder.readStr();

    // Read arg_
    arg_ = decoder.readExpression();
}

const Value& AggregateExpression::eval(ExpressionContext& ctx) {
    auto iter = nameIdMap_.find(name_->c_str());
    if (iter == AggregateExpression::nameIdMap_.end()) {
        return Value::kNullBadData;
    }

    auto val = arg_->eval(ctx);
    auto uniques = result_->uniques();
    if (distinct_ && uniques->contains(val)) {
        return result_->res();
    }
    uniques->values.emplace(val);

    auto apply = aggFunMap_[iter->second];
    apply(result_, val);

    return result_->res();
}

std::string AggregateExpression::toString() const {
    std::string arg(arg_->toString());
    std::string isDistinct;
    if (distinct_) { isDistinct = "distinct";}
    std::stringstream out;
    out << *name_ << "(" << isDistinct << " " << arg << ")";
    return out.str();
}

void AggregateExpression::accept(ExprVisitor* visitor) {
    // TODO : impl visitor in nebula-graph
    // visitor->visit(this);
    UNUSED(visitor);
}

}  // namespace nebula
