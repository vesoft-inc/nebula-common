/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/BinaryExpression.h"
#include "common/jit/JITUtils.h"

namespace nebula {

bool BinaryExpression::operator==(const Expression& rhs) const {
    if (kind_ != rhs.kind()) {
        return false;
    }

    const auto& r = dynamic_cast<const BinaryExpression&>(rhs);
    return *lhs_ == *(r.lhs_) && *rhs_ == *(r.rhs_);
}



void BinaryExpression::writeTo(Encoder& encoder) const {
    // kind_
    encoder << kind_;

    // lhs_
    DCHECK(!!lhs_);
    encoder << *lhs_;

    // rhs_
    DCHECK(!!rhs_);
    encoder << *rhs_;
}


void BinaryExpression::resetFrom(Decoder& decoder) {
    // Read lhs_
    lhs_ = decoder.readExpression();
    CHECK(!!lhs_);
    // Read rhs_
    rhs_ = decoder.readExpression();
    CHECK(!!rhs_);
}

llvm::Value* BinaryExpression::createBinOp(llvm::IRBuilder<>* builder,
                                           llvm::Value* lhs,
                                           llvm::Value* rhs,
                                           BinOP opForInt,
                                           BinOP opForDouble) const {
    switch (lhs->getType()->getTypeID()) {
        case llvm::Type::TypeID::IntegerTyID: {
            switch (rhs->getType()->getTypeID()) {
                case llvm::Type::TypeID::IntegerTyID: {
                    return opForInt(lhs, rhs);
                }
                case llvm::Type::TypeID::DoubleTyID: {
                    auto* l = builder->CreateSIToFP(lhs, rhs->getType());
                    return opForDouble(l, rhs);
                }
            }
            break;
        }
        case llvm::Type::TypeID::DoubleTyID: {
            switch (rhs->getType()->getTypeID()) {
                case llvm::Type::TypeID::IntegerTyID: {
                    auto* r = builder->CreateSIToFP(rhs, lhs->getType());
                    return opForDouble(lhs, r);
                }
                case llvm::Type::TypeID::DoubleTyID: {
                    return opForDouble(lhs, rhs);
                }
           }
           break;
        }
        default:
            break;
    }
    LOG(WARNING) << "Unsupport operations left type is " << lhs->getType()->getTypeID()
                 << ", right type is " << rhs->getType()->getTypeID();
    return nullptr;
}

}  // namespace nebula
