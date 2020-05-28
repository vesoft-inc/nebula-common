/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_ARITHMETICEXPRESSION_H_
#define COMMON_EXPRESSION_ARITHMETICEXPRESSION_H_

#include "common/expression/BinaryExpression.h"

namespace nebula {

class ArithmeticExpression final : public BinaryExpression {
public:
    ArithmeticExpression(Kind kind,
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
            case Kind::kAdd:
                buf += '+';
                break;
            case Kind::kMinus:
                buf += '-';
                break;
            case Kind::kMultiply:
                buf += '*';
                break;
            case Kind::kDivision:
                buf += '/';
                break;
            case Kind::kMod:
                buf += '%';
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
    Value                       result_;

private:
    llvm::Value* genAdd(llvm::IRBuilder<>* builder, llvm::Value* lhs, llvm::Value* rhs) const;

    llvm::Value* genSub(llvm::IRBuilder<>* builder, llvm::Value* lhs, llvm::Value* rhs) const;

    llvm::Value* genMul(llvm::IRBuilder<>* builder, llvm::Value* lhs, llvm::Value* rhs) const;

    llvm::Value* genDiv(llvm::IRBuilder<>* builder, llvm::Value* lhs, llvm::Value* rhs) const;

    llvm::Value* genMod(llvm::IRBuilder<>* builder, llvm::Value* lhs, llvm::Value* rhs) const;
};

}   // namespace nebula
#endif   // COMMON_EXPRESSION_EXPRESSION_H_
