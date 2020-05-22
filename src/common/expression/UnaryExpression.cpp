/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/UnaryExpression.h"
#include "common/expression/VariableExpression.h"

namespace nebula {
const Value& UnaryExpression::eval() {
   switch (kind_) {
        case Kind::kUnaryPlus: {
            Value val(operand_->eval());
            result_ = std::move(val);
            break;
        }
        case Kind::kUnaryNegate: {
            result_ = -(operand_->eval());
            break;
        }
        case Kind::kUnaryNot: {
            result_ = !(operand_->eval());
            break;
        }
        case Kind::kUnaryIncr: {
            if (UNLIKELY(operand_->kind() != Kind::kVar)) {
                result_ = Value(NullType::BAD_TYPE);
                break;
            }
            result_ = operand_->eval() + 1;
            auto* varExpr = static_cast<VariableExpression*>(operand_.get());
            ectx_->setVar(varExpr->var(), result_);
            break;
        }
        case Kind::kUnaryDecr: {
            if (UNLIKELY(operand_->kind() != Kind::kVar)) {
                result_ = Value(NullType::BAD_TYPE);
                break;
            }
            result_ = operand_->eval() - 1;
            auto* varExpr = static_cast<VariableExpression*>(operand_.get());
            ectx_->setVar(varExpr->var(), result_);
            break;
        }
       default:
           LOG(FATAL) << "Unknown type: " << kind_;
   }
   return result_;
}
}   // namespace nebula
