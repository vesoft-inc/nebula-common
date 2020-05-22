/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/ArithmeticExpression.h"

namespace nebula {

const Value& ArithmeticExpression::eval() {
    auto& lhs = lhs_->eval();
    auto& rhs = rhs_->eval();

    switch (kind_) {
        case Kind::kAdd:
            result_ = lhs + rhs;
            break;
        case Kind::kMinus:
            result_ = lhs - rhs;
            break;
        case Kind::kMultiply:
            result_ = lhs * rhs;
            break;
        case Kind::kDivision:
            result_ = lhs / rhs;
            break;
        case Kind::kMod:
            result_ = lhs % rhs;
            break;
        default:
            LOG(FATAL) << "Unknown type: " << kind_;
    }
    return result_;
}

std::string ArithmeticExpression::encode() const {
    return "";
}

}   // namespace nebula
