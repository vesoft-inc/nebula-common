/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/ListComprehensionExpression.h"
#include "common/expression/ExprVisitor.h"

namespace nebula {

const Value& ListComprehensionExpression::eval(ExpressionContext& ctx) {
    List ret;

    auto realVar = needRewrite_ ? newInnerVar_.get() : innerVar_.get();
    auto realFilter = needRewrite_ ? newFilter_.get() : filter_.get();
    auto realMapping = needRewrite_ ? newMapping_.get() : mapping_.get();

    auto& listVal = collection_->eval(ctx);
    if (!listVal.isList()) {
        return Value::kNullBadType;
    }
    auto& list = listVal.getList();

    for (size_t i = 0; i < list.size(); ++i) {
        auto v = list[i];
        if (realFilter != nullptr || realMapping != nullptr) {
            DCHECK(!!realVar);
            ctx.setVar(*realVar, v);
        }
        if (realFilter != nullptr) {
            auto& filterVal = realFilter->eval(ctx);
            if (!filterVal.empty() && !filterVal.isNull() && !filterVal.isBool()) {
                return Value::kNullBadType;
            }
            if (filterVal.empty() || filterVal.isNull() || !filterVal.getBool()) {
                continue;
            }
        }

        if (realMapping != nullptr) {
            v = realMapping->eval(ctx);
        }

        ret.emplace_back(std::move(v));
    }

    result_ = ret;
    return result_;
}

bool ListComprehensionExpression::operator==(const Expression& rhs) const {
    if (kind() != rhs.kind()) {
        return false;
    }

    const auto& expr = static_cast<const ListComprehensionExpression&>(rhs);

    if (needRewrite_ != expr.needRewrite_) {
        return false;
    }

    if (*innerVar_ != *expr.innerVar_) {
        return false;
        if (needRewrite_) {
            if (*newInnerVar_ != *expr.newInnerVar_) {
                return false;
            }
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
        if (needRewrite_) {
            if (*newFilter_ != *expr.newFilter_) {
                return false;
            }
        }
    }

    if (hasMapping() != expr.hasMapping()) {
        return false;
    }
    if (hasMapping()) {
        if (*mapping_ != *expr.mapping_) {
            return false;
        }
        if (needRewrite_) {
            if (*newMapping_ != *expr.newMapping_) {
                return false;
            }
        }
    }

    return true;
}

void ListComprehensionExpression::writeTo(Encoder& encoder) const {
    encoder << kind_;
    encoder << Value(needRewrite_);
    encoder << Value(hasFilter());
    encoder << Value(hasMapping());

    encoder << innerVar_.get();
    if (needRewrite_) {
        encoder << newInnerVar_.get();
    }
    encoder << *collection_;
    if (hasFilter()) {
        encoder << *filter_;
        if (needRewrite_) {
            encoder << *newFilter_;
        }
    }
    if (hasMapping()) {
        encoder << *mapping_;
        if (needRewrite_) {
            encoder << *newMapping_;
        }
    }
}

void ListComprehensionExpression::resetFrom(Decoder& decoder) {
    needRewrite_ = decoder.readValue().getBool();
    bool hasFilter = decoder.readValue().getBool();
    bool hasMapping = decoder.readValue().getBool();

    innerVar_ = decoder.readStr();
    if (needRewrite_) {
        newInnerVar_ = decoder.readStr();
    }
    collection_ = decoder.readExpression();
    if (hasFilter) {
        filter_ = decoder.readExpression();
        if (needRewrite_) {
            newFilter_ = decoder.readExpression();
        }
    }
    if (hasMapping) {
        mapping_ = decoder.readExpression();
        if (needRewrite_) {
            newMapping_ = decoder.readExpression();
        }
    }
}

std::string ListComprehensionExpression::toString() const {
    std::string buf;
    buf.reserve(256);

    buf += "[";
    buf += *innerVar_;
    buf += " IN ";
    buf += collection_->toString();
    if (hasFilter()) {
        buf += " WHERE ";
        buf += filter_->toString();
    }
    if (hasMapping()) {
        buf += " | ";
        buf += mapping_->toString();
    }
    buf += "]";

    return buf;
}

void ListComprehensionExpression::accept(ExprVisitor* visitor) {
    visitor->visit(this);
}

}   // namespace nebula
