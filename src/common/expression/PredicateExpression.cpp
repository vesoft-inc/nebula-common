/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/PredicateExpression.h"
#include "common/expression/ExprVisitor.h"

namespace nebula {

const Value& PredicateExpression::eval(ExpressionContext& ctx) {
    auto& name = *name_;
    auto& listVal = collection_->eval(ctx);
    if (listVal.isNull()) {
        result_ = Value::kNullValue;
        return result_;
    }
    if (!listVal.isList()) {
        result_ = Value::kNullBadType;
        return result_;
    }
    auto& list = listVal.getList();

    if (name == "all") {
        result_ = true;
        for (size_t i = 0; i < list.size(); ++i) {
            auto& v = list[i];
            ctx.setVar(*innerVar_, v);
            auto& filterVal = filter_->eval(ctx);
            if (!filterVal.empty() && !filterVal.isNull() && !filterVal.isBool()) {
                return Value::kNullBadType;
            }
            if (filterVal.empty() || filterVal.isNull() || !filterVal.getBool()) {
                result_ = false;
                return result_;
            }
        }
        return result_;
    } else if (name == "any") {
        result_ = false;
        for (size_t i = 0; i < list.size(); ++i) {
            auto& v = list[i];
            ctx.setVar(*innerVar_, v);
            auto& filterVal = filter_->eval(ctx);
            if (!filterVal.empty() && !filterVal.isNull() && !filterVal.isBool()) {
                return Value::kNullBadType;
            }
            if (filterVal.isBool() && filterVal.getBool()) {
                result_ = true;
                return result_;
            }
        }
        return result_;
    } else if (name == "single") {
        result_ = false;
        for (size_t i = 0; i < list.size(); ++i) {
            auto& v = list[i];
            ctx.setVar(*innerVar_, v);
            auto& filterVal = filter_->eval(ctx);
            if (!filterVal.empty() && !filterVal.isNull() && !filterVal.isBool()) {
                return Value::kNullBadType;
            }
            if (filterVal.isBool() && filterVal.getBool()) {
                if (result_ == false) {
                    result_ = true;
                } else {
                    result_ = false;
                    return result_;
                }
            }
        }
        return result_;
    } else if (name == "none") {
        result_ = true;
        for (size_t i = 0; i < list.size(); ++i) {
            auto& v = list[i];
            ctx.setVar(*innerVar_, v);
            auto& filterVal = filter_->eval(ctx);
            if (!filterVal.empty() && !filterVal.isNull() && !filterVal.isBool()) {
                return Value::kNullBadType;
            }
            if (filterVal.isBool() && filterVal.getBool()) {
                result_ = false;
                return result_;
            }
        }
        return result_;
    } else {
        result_ = Value::kNullBadType;
        return result_;
    }
}

bool PredicateExpression::operator==(const Expression& rhs) const {
    if (kind() != rhs.kind()) {
        return false;
    }

    const auto& expr = static_cast<const PredicateExpression&>(rhs);

    if (*name_ != *expr.name_) {
        return false;
    }

    if (*innerVar_ != *expr.innerVar_) {
        return false;
    }

    if (*collection_ != *expr.collection_) {
        return false;
    }
    if (*filter_ != *expr.filter_) {
        return false;
    }

    return true;
}

void PredicateExpression::writeTo(Encoder& encoder) const {
    encoder << kind_;
    encoder << Value(hasOriginString());

    encoder << name_.get();
    encoder << innerVar_.get();
    encoder << *collection_;
    encoder << *filter_;
    if (hasOriginString()) {
        encoder << originString_.get();
    }
}

void PredicateExpression::resetFrom(Decoder& decoder) {
    bool hasFilter = decoder.readValue().getBool();
    bool hasString = decoder.readValue().getBool();

    name_ = decoder.readStr();
    innerVar_ = decoder.readStr();
    collection_ = decoder.readExpression();
    if (hasFilter) {
        filter_ = decoder.readExpression();
    }
    if (hasString) {
        originString_ = decoder.readStr();
    }
}

std::string PredicateExpression::toString() const {
    if (originString_ != nullptr) {
        return *originString_;
    } else {
        return makeString();
    }
}

std::string PredicateExpression::makeString() const {
    std::string buf;
    buf.reserve(256);

    buf += *name_;
    buf += "(";
    buf += *innerVar_;
    buf += " IN ";
    buf += collection_->toString();
    buf += " WHERE ";
    buf += filter_->toString();
    buf += ")";

    return buf;
}

void PredicateExpression::accept(ExprVisitor* visitor) {
    visitor->visit(this);
}

}   // namespace nebula
