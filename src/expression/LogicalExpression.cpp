/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "expression/LogicalExpression.h"

namespace nebula {
const Value& LogicalExpression::eval() {
    auto lhs = lhs_->eval();
    auto rhs = rhs_->eval();

    switch (type_) {
        case Type::EXP_LOGICAL_AND:
            result_ = lhs && rhs;
            break;
        case Type::EXP_LOGICAL_OR:
            result_ = lhs || rhs;
            break;
        case Type::EXP_LOGICAL_XOR:
            result_ = (lhs && !rhs) || (!lhs && rhs);
            break;
        default:
            LOG(FATAL) << "Unknown type: " << type_;
    }
    return result_;
}
}  // namespace nebula
