/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/RelationalExpression.h"
#include "common/datatypes/List.h"

namespace nebula {
const Value& RelationalExpression::eval() {
    auto& lhs = lhs_->eval();
    auto& rhs = rhs_->eval();

    switch (type_) {
        case Type::EXP_REL_EQ:
            result_ = lhs == rhs;
            break;
        case Type::EXP_REL_NE:
            result_ = lhs != rhs;
            break;
        case Type::EXP_REL_LT:
            result_ = lhs < rhs;
            break;
        case Type::EXP_REL_LE:
            result_ = lhs <= rhs;
            break;
        case Type::EXP_REL_GT:
            result_ = lhs > rhs;
            break;
        case Type::EXP_REL_GE:
            result_ = lhs >= rhs;
            break;
        case Type::EXP_REL_IN: {
            if (UNLIKELY(rhs.type() != Value::Type::LIST)) {
                result_ = Value(NullType::BAD_TYPE);
                break;
            }
            auto& list = rhs.getList().values;
            auto found = std::find(list.begin(), list.end(), lhs);
            if (found == list.end()) {
                result_ = false;
            } else {
                result_ = true;
            }
            break;
        }
        default:
            LOG(FATAL) << "Unknown type: " << type_;
    }
    return result_;
}
}   // namespace nebula
