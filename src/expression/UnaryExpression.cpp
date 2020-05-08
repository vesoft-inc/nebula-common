/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "expression/UnaryExpression.h"

namespace nebula {
const Value& UnaryExpression::eval() {
   switch (type_) {
       case Type::EXP_UNARY_PLUS: {
           Value val(operand_->eval());
           result_ = std::move(val);
           break;
       }
       case Type::EXP_UNARY_NEGATE: {
           result_ = -(operand_->eval());
           break;
       }
       case Type::EXP_UNARY_NOT: {
           result_ = !(operand_->eval());
           break;
       }
       default:
           LOG(FATAL) << "Unknown type: " << type_;
   }
   return result_;
}
}  // namespace nebula
