/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/TypeCastingExpression.h"
#include "common/jit/JITUtils.h"

namespace nebula {
const Value& TypeCastingExpression::eval(ExpressionContext& ctx) {
    UNUSED(ctx);
    return result_;
}


bool TypeCastingExpression::operator==(const Expression& rhs) const {
    if (kind_ != rhs.kind()) {
        return false;
    }

    const auto& r = dynamic_cast<const TypeCastingExpression&>(rhs);
    return vType_ == r.vType_ && *operand_ == *(r.operand_);
}


void TypeCastingExpression::writeTo(Encoder& encoder) const {
    // kind_
    encoder << kind_;

    // vType_
    encoder << vType_;

    // operand_
    DCHECK(!!operand_);
    encoder << *operand_;
}


void TypeCastingExpression::resetFrom(Decoder& decoder) {
    // Read vType_
    vType_ = decoder.readValueType();

    // Read operand_
    operand_ = decoder.readExpression();
    CHECK(!!operand_);
}

// TODO(heng): support string <==> numeric in the future.
llvm::Value*
TypeCastingExpression::codegen(ExprCodegenContext& ctx) const {
    auto* builder = ctx.builder_;
    if (builder == nullptr) {
        return nullptr;
    }
    auto* val = operand_->codegen(ctx);
    switch (vType_) {
        case Value::Type::INT:
            if (val->getType()->isDoubleTy()) {
                return builder->CreateFPToSI(val, builder->getInt64Ty(), "fp2si");
            } else if (val->getType()->isIntegerTy()) {
                return val;
            }
            break;
        case Value::Type::FLOAT:
            if (val->getType()->isDoubleTy()) {
                return val;
            } else if (val->getType()->isIntegerTy()) {
                return builder->CreateSIToFP(val, builder->getDoubleTy(), "si2fp");
            }
            break;
        case Value::Type::BOOL:
            if (val->getType()->isDoubleTy()) {
                return builder->CreateFPToSI(val, builder->getInt1Ty(), "fp2si");
            } else if (val->getType()->isIntegerTy()) {
                return val;
            }
            break;
        case Value::Type::STRING:
        default:
            break;
    }
    return nullptr;
}

}  // namespace nebula
