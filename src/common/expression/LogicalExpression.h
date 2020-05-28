/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_LOGICALEXPRESSION_H_
#define COMMON_EXPRESSION_LOGICALEXPRESSION_H_

#include "common/expression/BinaryExpression.h"

namespace nebula {
class LogicalExpression final : public BinaryExpression {
public:
    LogicalExpression(Kind kind,
                      Expression* lhs = nullptr,
                      Expression* rhs = nullptr)
        : BinaryExpression(kind, lhs, rhs) {}

    const Value& eval(ExpressionContext& ctx) override;

    std::string toString() const override {
       std::string buf;
        buf.reserve(256);
        buf += '(';
        buf.append(lhs_->toString());
        switch (kind_) {
            case Kind::kLogicalAnd:
                buf += "&&";
                break;
            case Kind::kLogicalOr:
                buf += "||";
                break;
            case Kind::kLogicalXor:
                buf += "XOR";
                break;
            default:
                LOG(FATAL) << "Uknown type!";
                break;
        }
        buf.append(rhs_->toString());
        buf += ')';
        return buf;
    }

    llvm::Value* codegen(ExprCodegenContext& ctx) const override;

private:
    Value                                       result_;
};

}   // namespace nebula
#endif  // COMMON_EXPRESSION_LOGICALEXPRESSION_H_
