/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/LogicalExpression.h"
#include "common/jit/JITUtils.h"

namespace nebula {
const Value& LogicalExpression::eval(ExpressionContext& ctx) {
    auto& lhs = lhs_->eval(ctx);
    auto& rhs = rhs_->eval(ctx);

    switch (kind_) {
        case Kind::kLogicalAnd:
            result_ = lhs && rhs;
            break;
        case Kind::kLogicalOr:
            result_ = lhs || rhs;
            break;
        case Kind::kLogicalXor:
            result_ = (lhs && !rhs) || (!lhs && rhs);
            break;
        default:
            LOG(FATAL) << "Unknown type: " << kind_;
    }
    return result_;
}

llvm::Value* LogicalExpression::codegen(ExprCodegenContext& ctx) const {
    auto* builder = ctx.builder_;
    if (builder == nullptr) {
        return nullptr;
    }
    auto* lhs = JITUtils::convertToBool(builder, lhs_->codegen(ctx));
    auto* rhs = JITUtils::convertToBool(builder, rhs_->codegen(ctx));
    if (!lhs || !rhs) {
        return nullptr;
    }
    switch (kind_) {
        case Kind::kLogicalAnd: {
            return builder->CreateAnd(lhs, rhs, "and");
        }
        case Kind::kLogicalOr: {
            return builder->CreateOr(lhs, rhs, "or");
        }
        case Kind::kLogicalXor: {
            return builder->CreateXor(lhs, rhs, "xor");
        }
        default:
            break;
    }
    LOG(WARNING) << "Unsupport operation " << static_cast<int32_t>(kind_);
    return nullptr;
}

}   // namespace nebula
