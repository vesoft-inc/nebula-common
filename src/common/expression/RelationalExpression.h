/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_RELATIONALEXPRESSION_H_
#define COMMON_EXPRESSION_RELATIONALEXPRESSION_H_

#include "common/expression/BinaryExpression.h"

namespace nebula {
class RelationalExpression final : public BinaryExpression {
public:
    RelationalExpression(Kind kind,
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
            case Kind::kRelLT:
                buf += '<';
                break;
            case Kind::kRelLE:
                buf += "<=";
                break;
            case Kind::kRelGT:
                buf += '>';
                break;
            case Kind::kRelGE:
                buf += ">=";
                break;
            case Kind::kRelEQ:
                buf += "==";
                break;
            case Kind::kRelNE:
                buf += "!=";
                break;
            case Kind::kRelIn:
                buf += " IN ";
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

private:
    llvm::Value* genGT(llvm::IRBuilder<>* builder,
                       llvm::Module* module,
                       llvm::Value* lhs,
                       llvm::Value* rhs) const;

    llvm::Value* genGE(llvm::IRBuilder<>* builder,
                       llvm::Module* module,
                       llvm::Value* lhs,
                       llvm::Value* rhs) const;

    llvm::Value* genLT(llvm::IRBuilder<>* builder,
                       llvm::Module* module,
                       llvm::Value* lhs,
                       llvm::Value* rhs) const;

    llvm::Value* genLE(llvm::IRBuilder<>* builder,
                       llvm::Module* module,
                       llvm::Value* lhs,
                       llvm::Value* rhs) const;

    llvm::Value* genEQ(llvm::IRBuilder<>* builder,
                       llvm::Module* module,
                       llvm::Value* lhs,
                       llvm::Value* rhs) const;

    llvm::Value* genNE(llvm::IRBuilder<>* builder,
                       llvm::Module* module,
                       llvm::Value* lhs,
                       llvm::Value* rhs) const;
};

}  // namespace nebula
#endif  // COMMON_EXPRESSION_RELATIONALEXPRESSION_H_
