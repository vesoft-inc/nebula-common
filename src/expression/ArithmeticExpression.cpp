/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "expression/ArithmeticExpression.h"

namespace nebula {

const Value& ArithmeticExpression::eval() {
    auto& lhs = lhs_->eval();
    auto& rhs = rhs_->eval();

    switch (type_) {
        case Type::EXP_ADD:
            result_ = lhs + rhs;
            break;
        case Type::EXP_MINUS:
            result_ = lhs - rhs;
            break;
        case Type::EXP_MULTIPLY:
            result_ = lhs * rhs;
            break;
        case Type::EXP_DIVIDE:
            result_ = lhs / rhs;
            break;
        case Type::EXP_MOD:
            result_ = lhs % rhs;
            break;
        default:
            LOG(FATAL) << "Unknown type: " << type_;
    }
    return result_;
}



std::string ArithmeticExpression::encode() const {
    return "";
}

}  // namespace nebula


