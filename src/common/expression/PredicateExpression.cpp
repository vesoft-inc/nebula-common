/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/PredicateExpression.h"
#include "common/expression/ExprVisitor.h"

namespace nebula {

std::unordered_map<std::string, PredicateExpression::Type> PredicateExpression::typeMap_ = {
    {"all", Type::ALL},
    {"any", Type::ANY},
    {"single", Type::SINGLE},
    {"none", Type::NONE},
};

const Value& PredicateExpression::evalExists(ExpressionContext& ctx) {
    if (!result_.empty()) {
        return result_;
    }
    auto& container = collection_->eval(ctx);
    auto& key = filter_->eval(ctx);
    switch (container.type()) {
        case Value::Type::VERTEX: {
            result_ = container.getVertex().contains(key);
            break;
        }
        case Value::Type::EDGE: {
            result_ = container.getEdge().contains(key);
            break;
        }
        case Value::Type::MAP: {
            result_ = container.getMap().contains(key);
            break;
        }
        case Value::Type::NULLVALUE: {
            result_ = Value::kNullValue;
            break;
        }
        default: {
            result_ = Value::kNullBadType;
        }
    }
    return result_;
}

const Value& PredicateExpression::eval(ExpressionContext& ctx) {
    if (*name_ == "exists") {
        return evalExists(ctx);
    }
    Type type;
    auto iter = typeMap_.find(*name_);
    if (iter != typeMap_.end()) {
        type = iter->second;
    } else {
        result_ = Value::kNullBadType;
        return result_;
    }

    auto& listVal = collection_->eval(ctx);
    if (listVal.isNull() || listVal.empty()) {
        result_ = listVal;
        return result_;
    }
    if (!listVal.isList()) {
        result_ = Value::kNullBadType;
        return result_;
    }
    auto& list = listVal.getList();

    switch (type) {
        case Type::ALL: {
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
        }
        case Type::ANY: {
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
        }
        case Type::SINGLE: {
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
        }
        case Type::NONE: {
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
        }
        // no default so the compiler will warning when lack
    }

    result_ = Value::kNullBadType;
    return result_;
}

bool PredicateExpression::operator==(const Expression& rhs) const {
    if (kind() != rhs.kind()) {
        return false;
    }

    const auto& expr = static_cast<const PredicateExpression&>(rhs);

    if (*name_ != *expr.name_) {
        return false;
    }

    if (hasInnerVar() != expr.hasInnerVar()) {
        return false;
    }

    if (hasInnerVar()) {
        if (*innerVar_ != *expr.innerVar_) {
            return false;
        }
    }

    if (*collection_ != *expr.collection_) {
        return false;
    }

    if (hasFilter() != expr.hasFilter()) {
        return false;
    }

    if (hasFilter()) {
        if (*filter_ != *expr.filter_) {
            return false;
        }
    }

    return true;
}

std::unique_ptr<Expression> PredicateExpression::clone() const {
    auto expr = std::make_unique<PredicateExpression>(
        new std::string(*name_),
        innerVar_ == nullptr ? nullptr : new std::string(*innerVar_),
        collection_->clone().release(),
        filter_ != nullptr ? filter_->clone().release() : nullptr);
    if (originString_ != nullptr) {
        expr->setOriginString(new std::string(*originString_));
    }
    return expr;
}

void PredicateExpression::writeTo(Encoder& encoder) const {
    encoder << kind_;
    encoder << Value(hasInnerVar());
    encoder << Value(hasFilter());
    encoder << Value(hasOriginString());

    encoder << name_.get();
    if (hasInnerVar()) {
        encoder << innerVar_.get();
    }
    encoder << *collection_;
    if (hasFilter()) {
        encoder << *filter_;
    }
    if (hasOriginString()) {
        encoder << originString_.get();
    }
}

void PredicateExpression::resetFrom(Decoder& decoder) {
    bool hasInnerVar = decoder.readValue().getBool();
    bool hasFilter = decoder.readValue().getBool();
    bool hasString = decoder.readValue().getBool();

    name_ = decoder.readStr();
    if (hasInnerVar) {
        innerVar_ = decoder.readStr();
    }
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
    }
    return makeString();
}

std::string PredicateExpression::makeString() const {
    std::string buf;
    buf.reserve(256);

    buf += *name_;
    buf += "(";
    if (innerVar_ != nullptr) {
        buf += *innerVar_;
        buf += " IN ";
        buf += collection_->toString();
        buf += " WHERE ";
        buf += filter_->toString();
    } else {
        buf += collection_->toString();
    }
    buf += ")";

    return buf;
}

void PredicateExpression::accept(ExprVisitor* visitor) {
    visitor->visit(this);
}

}   // namespace nebula
