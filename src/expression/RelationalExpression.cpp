/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "expression/RelationalExpression.h"

namespace nebula {
const Value& RelationalExpression::eval() {
    auto lhs = lhs_->eval();
    auto rhs = rhs_->eval();

    switch (type_) {
        case Type::EXP_REL_EQ:
            result_ = lhs_ == rhs_;
            break;
        case Type::EXP_REL_NE:
            result_ = lhs_ != rhs_;
            break;
        case Type::EXP_REL_LT:
            result_ = lhs_ < rhs_;
            break;
        case Type::EXP_REL_LE:
            result_ = lhs_ <= rhs_;
            break;
        case Type::EXP_REL_GT:
            result_ = lhs_ > rhs_;
            break;
        case Type::EXP_REL_GE:
            result_ = lhs_ >= rhs_;
            break;
        default:
            LOG(FATAL) << "Unknown type: " << type_;
    }
    return result_;
}
}  // namespace nebula
