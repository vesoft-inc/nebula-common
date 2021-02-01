/* Copyright (c) 2019 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/SubscriptExpression.h"
#include "common/datatypes/Map.h"
#include "common/datatypes/List.h"
#include "common/expression/ExprVisitor.h"

namespace nebula {

const Value& SubscriptExpression::eval(ExpressionContext &ctx) {
    auto &lvalue = left()->eval(ctx);
    auto &rvalue = right()->eval(ctx);

    result_ = Value::kNullValue;
    do {
        if (lvalue.isList()) {
            if (!rvalue.isInt()) {
                result_ = Value::kNullBadType;
                break;
            }
            auto size = static_cast<int64_t>(lvalue.getList().size());
            auto index = rvalue.getInt();
            if (index >= size || index < -size) {
                result_ = Value::kNullOutOfRange;
                break;
            }
            // list[0] === list[-size], list[-1] === list[size-1]
            if (index < 0) {
                index += size;
            }
            result_ = lvalue.getList()[index];
            break;
        }
        if (lvalue.isMap()) {
            if (!rvalue.isStr()) {
                break;
            }
            result_ = lvalue.getMap().at(rvalue.getStr());
            break;
        }
        if (lvalue.isDataSet()) {
            if (!rvalue.isInt()) {
                result_ = Value::kNullBadType;
                break;
            }
            auto size = static_cast<int64_t>(lvalue.getDataSet().rowSize());
            auto rowIndex = rvalue.getInt();
            if (rowIndex >= size || rowIndex < 0) {
                result_ = Value::kNullOutOfRange;
                break;
            }
            result_ = lvalue.getDataSet().rows[rowIndex];
            break;
        }
        if (lvalue.isVertex()) {
            if (!rvalue.isStr()) {
                break;
            }
            if (rvalue.getStr() == kVid) {
                result_ = lvalue.getVertex().vid;
                return result_;
            }
            for (auto &tag : lvalue.getVertex().tags) {
                auto iter = tag.props.find(rvalue.getStr());
                if (iter != tag.props.end()) {
                    return iter->second;
                }
            }
            return Value::kNullValue;
        }
        if (lvalue.isEdge()) {
            if (!rvalue.isStr()) {
                break;
            }
            DCHECK(!rvalue.getStr().empty());
            if (rvalue.getStr()[0] == '_') {
                if (rvalue.getStr() == kSrc) {
                    result_ = lvalue.getEdge().src;
                } else if (rvalue.getStr() == kDst) {
                    result_ = lvalue.getEdge().dst;
                } else if (rvalue.getStr() == kRank) {
                    result_ = lvalue.getEdge().ranking;
                } else if (rvalue.getStr() == kType) {
                    result_ = lvalue.getEdge().name;
                }
                return result_;
            }
            auto iter = lvalue.getEdge().props.find(rvalue.getStr());
            if (iter == lvalue.getEdge().props.end()) {
                return Value::kNullValue;
            }
            return iter->second;
        }
        result_ = Value::kNullBadType;
    } while (false);

    return result_;
}


std::string SubscriptExpression::toString() const {
    std::string buf;
    buf.reserve(256);
    buf += left()->toString();
    buf += '[';
    buf += right()->toString();
    buf += ']';
    return buf;
}

void SubscriptExpression::accept(ExprVisitor *visitor) {
    visitor->visit(this);
}

}   // namespace nebula
