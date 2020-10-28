/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/CaseExpression.h"
#include "common/expression/ConstantExpression.h"
#include "common/datatypes/List.h"
#include "common/datatypes/Map.h"
#include "common/datatypes/Set.h"
#include "common/expression/ExprVisitor.h"

namespace nebula {

bool CaseExpression::operator==(const Expression& rhs) const {
    if (kind() != rhs.kind()) {
        return false;
    }

    auto& expr = static_cast<const CaseExpression&>(rhs);
    if (hasCondition() != expr.hasCondition()) {
        return false;
    }
    if (hasCondition()) {
        if (*condition_ != *expr.condition_) {
            return false;
        }
    }
    if (hasDefault() != expr.hasDefault()) {
        return false;
    }
    if (hasDefault()) {
        if (*default_ != *expr.default_) {
            return false;
        }
    }
    if (numCases() != expr.numCases()) {
        return false;
    }
    for (auto i = 0u; i < numCases(); i++) {
        if (*cases_[i].when != *expr.cases_[i].when) {
            return false;
        }
        if (*cases_[i].then != *expr.cases_[i].then) {
            return false;
        }
    }

    return true;
}

const Value& CaseExpression::eval(ExpressionContext& ctx) {
    switch (kind_) {
        case Kind::kSimpleCase: {
            auto cond = condition_->eval(ctx);
            for (const auto& whenThen : cases_) {
                auto when = whenThen.when->eval(ctx);
                auto then = whenThen.then->eval(ctx);
                if (cond == when) {
                    result_ = then;
                    return result_;
                }
            }
            if (default_) {
                result_ = default_->eval(ctx);
            } else {
                result_ = Value::kNullValue;
            }
            break;
        }
        case Kind::kGenericCase:
        case Kind::kConditionalCase: {
            for (const auto& whenThen : cases_) {
                auto when = whenThen.when->eval(ctx);
                auto then = whenThen.then->eval(ctx);
                if (!when.isBool()) {
                    return Value::kNullBadType;
                }
                if (when.getBool()) {
                    result_ = then;
                    return result_;
                }
            }
            if (default_) {
                result_ = default_->eval(ctx);
            } else {
                result_ = Value::kNullValue;
            }
            break;
        }
        default: {
            LOG(FATAL) << "Unknown type: " << kind_;
        }
    }
    return result_;
}

std::string CaseExpression::toString() const {
    std::string buf;
    buf.reserve(256);

    if (kind_ == Kind::kSimpleCase || kind_ == Kind::kGenericCase) {
        buf += "CASE";
        if (hasCondition()) {
            buf += " ";
            buf += condition_->toString();
        }
        for (const auto& whenThen : cases_) {
            buf += " WHEN ";
            buf += whenThen.when->toString();
            buf += " THEN ";
            buf += whenThen.then->toString();
        }
        if (hasDefault()) {
            buf += " ELSE ";
            buf += default_->toString();
        }
        buf += " END";
    } else {
        buf += "(";
        buf += cases_.front().when->toString();
        buf += " ? ";
        buf += cases_.front().then->toString();
        buf += " : ";
        buf += default_->toString();
        buf += ")";
    }

    return buf;
}

void CaseExpression::writeTo(Encoder& encoder) const {
    encoder << kind_;
    encoder << Value(hasCondition());
    encoder << Value(hasDefault());
    encoder << numCases();
    if (hasCondition()) {
        encoder << *condition_;
    }
    if (hasDefault()) {
        encoder << *default_;
    }
    for (const auto& whenThen : cases_) {
        encoder << *whenThen.when;
        encoder << *whenThen.then;
    }
}

void CaseExpression::resetFrom(Decoder& decoder) {
    bool hasCondition = decoder.readValue().getBool();
    bool hasDefault = decoder.readValue().getBool();
    auto num = decoder.readSize();
    if (hasCondition) {
        condition_ = decoder.readExpression();
        CHECK(!!condition_);
    }
    if (hasDefault) {
        default_ = decoder.readExpression();
        CHECK(!!default_);
    }
    cases_.reserve(num);
    for (auto i = 0u; i < num; i++) {
        auto when = decoder.readExpression();
        CHECK(!!when);
        auto then = decoder.readExpression();
        CHECK(!!then);
        cases_.emplace_back(when.release(), then.release());
    }
}

void CaseExpression::accept(ExprVisitor* visitor) {
    visitor->visit(this);
}

}   // namespace nebula
