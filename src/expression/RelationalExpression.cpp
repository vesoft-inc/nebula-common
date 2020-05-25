/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "expression/RelationalExpression.h"

namespace nebula {
Value RelationalExpression::eval() const {
    auto lhs = lhs_->eval();
    auto rhs = rhs_->eval();

    switch (kind_) {
        case Kind::kRelEQ:
            return lhs_ == rhs_;
        case Kind::kRelNE:
            return lhs_ != rhs_;
        case Kind::kRelLT:
            return lhs_ < rhs_;
        case Kind::kRelLE:
            return lhs_ <= rhs_;
        case Kind::kRelGT:
            return lhs_ > rhs_;
        case Kind::kRelGE:
            return lhs_ >= rhs_;
        default:
            break;
    }
    LOG(FATAL) << "Unknown type: " << kind_;
}
}  // namespace nebula
