/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "expression/ArithmeticExpression.h"

namespace nebula {

Value ArithmeticExpression::eval() const {
    switch (kind_) {
    case Kind::kAdd:
        return lhs_->eval() + rhs_->eval();
    case Kind::kMinus:
        return lhs_->eval() - rhs_->eval();
    case Kind::kMultiply:
        return lhs_->eval() * rhs_->eval();
    case Kind::kDivision:
        return lhs_->eval() / rhs_->eval();
    case Kind::kMod:
        return lhs_->eval() % rhs_->eval();
    default:
        break;
    }
    LOG(FATAL) << "Unknown type: " << kind_;
}

std::string ArithmeticExpression::encode() const {
    return "";
}

}  // namespace nebula
