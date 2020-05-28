/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/ArithmeticExpression.h"

namespace nebula {

const Value& ArithmeticExpression::eval(ExpressionContext& ctx) {
    auto& lhs = lhs_->eval(ctx);
    auto& rhs = rhs_->eval(ctx);

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

llvm::Value* ArithmeticExpression::genAdd(llvm::IRBuilder<>* builder,
                                          llvm::Value* lhs,
                                          llvm::Value* rhs) const {
    return createBinOp(builder,
                       lhs,
                       rhs,
                       [builder] (llvm::Value* l, llvm::Value* r) {
                           return builder->CreateNSWAdd(l, r, "addtmp");
                       },
                       [builder] (llvm::Value* l, llvm::Value* r) {
                           return builder->CreateFAdd(l, r, "faddtmp");
                       });
}

llvm::Value* ArithmeticExpression::genSub(llvm::IRBuilder<>* builder,
                                          llvm::Value* lhs,
                                          llvm::Value* rhs) const {
    return createBinOp(builder,
                       lhs,
                       rhs,
                       [builder] (llvm::Value* l, llvm::Value* r) {
                           return builder->CreateNSWSub(l, r, "subtmp");
                       },
                       [builder] (llvm::Value* l, llvm::Value* r) {
                           return builder->CreateFSub(l, r, "fsubtmp");
                       });
}

llvm::Value* ArithmeticExpression::genMul(llvm::IRBuilder<>* builder,
                                          llvm::Value* lhs,
                                          llvm::Value* rhs) const {
    return createBinOp(builder,
                       lhs,
                       rhs,
                       [builder] (llvm::Value* l, llvm::Value* r) {
                           return builder->CreateNSWMul(l, r, "multmp");
                       },
                       [builder] (llvm::Value* l, llvm::Value* r) {
                           return builder->CreateFMul(l, r, "fmultmp");
                       });
}

llvm::Value* ArithmeticExpression::genDiv(llvm::IRBuilder<>* builder,
                                          llvm::Value* lhs,
                                          llvm::Value* rhs) const {
    return createBinOp(builder,
                       lhs,
                       rhs,
                       [builder] (llvm::Value* l, llvm::Value* r) {
                           return builder->CreateSDiv(l, r, "sdivtmp");
                       },
                       [builder] (llvm::Value* l, llvm::Value* r) {
                           return builder->CreateFDiv(l, r, "fdivtmp");
                       });
}

llvm::Value* ArithmeticExpression::genMod(llvm::IRBuilder<>* builder,
                                          llvm::Value* lhs,
                                          llvm::Value* rhs) const {
    return createBinOp(builder,
                       lhs,
                       rhs,
                       [builder] (llvm::Value* l, llvm::Value* r) {
                           return builder->CreateSRem(l, r, "sremtmp");
                       },
                       [builder] (llvm::Value* l, llvm::Value* r) {
                           return builder->CreateFRem(l, r, "fremtmp");
                       });
}

llvm::Value* ArithmeticExpression::codegen(ExprCodegenContext& ctx) const {
    auto* builder = ctx.builder_;
    if (builder == nullptr) {
        return nullptr;
    }
    auto* L = lhs_->codegen(ctx);
    auto* R = rhs_->codegen(ctx);
    if (!L || !R) {
        return nullptr;
    }
    switch (kind_) {
        case Kind::kAdd:
            return genAdd(builder, L, R);
        case Kind::kMinus:
            return genSub(builder, L, R);
        case Kind::kMultiply:
            return genMul(builder, L, R);
        case Kind::kDivision:
            return genDiv(builder, L, R);
        case Kind::kMod:
            return genMod(builder, L, R);
        default:
            break;
    }
    return nullptr;
}

}   // namespace nebula
