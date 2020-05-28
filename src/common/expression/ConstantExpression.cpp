/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/ConstantExpression.h"
#include "common/jit/JITUtils.h"

namespace nebula {

bool ConstantExpression::operator==(const Expression& rhs) const {
    if (kind_ != rhs.kind()) {
        return false;
    }

    const auto& r = dynamic_cast<const ConstantExpression&>(rhs);
    return val_ == r.val_;
}



void ConstantExpression::writeTo(Encoder& encoder) const {
    // kind_
    encoder << kind_;

    // val_
    encoder << val_;
}


void ConstantExpression::resetFrom(Decoder& decoder) {
    // Deserialize val_
    val_ = decoder.readValue();
}

llvm::Constant* toLLVMConstant(llvm::IRBuilder<>* builder, const Value& val) {
     switch (val.type()) {
        case Value::Type::INT:
            return builder->getInt64(val.getInt());
        case Value::Type::FLOAT:
            return llvm::ConstantFP::get(builder->getContext(),
                                         llvm::APFloat(val.getFloat()));
        case Value::Type::STRING: {
            return builder->CreateGlobalStringPtr(val.getStr());
        }
        case Value::Type::BOOL:
            return val.getBool()
                        ? builder->getTrue()
                        : builder->getFalse();
        case Value::Type::LIST: {
              // TODO(heng): We assume the list has the same type values.
             auto& list = val.getList().values;
             std::vector<llvm::Constant*> values;
             llvm::Type* elementType = nullptr;
             for (auto& e : list) {
                auto* tmp = toLLVMConstant(builder, e);
                if (tmp == nullptr) {
                    LOG(WARNING) << "convert failed for e " << e.type();
                    return nullptr;
                }
                if (elementType == nullptr) {
                    elementType = tmp->getType();
                }
                if (elementType != tmp->getType()) {
                    LOG(WARNING) << "The elements in list should be the same type!";
                    return nullptr;
                }
                values.emplace_back(tmp);
             }
             if (values.size() > 0) {
                CHECK_NOTNULL(elementType);
                VLOG(1) << "ConstantArray, e type "
                        << static_cast<int32_t>(elementType->getTypeID())
                        << ", size " << values.size();

                return llvm::ConstantArray::get(llvm::ArrayType::get(elementType, values.size()),
                                                values);
             }
             LOG(WARNING) << "The list is empty!";
             return nullptr;
        }
        default:
            LOG(ERROR) << "Unsupportted value type";
            break;
    }
    return nullptr;
}

llvm::Value* ConstantExpression::codegen(ExprCodegenContext& ctx) const {
    auto* builder = ctx.builder_;
    if (builder == nullptr) {
        return nullptr;
    }
    return toLLVMConstant(builder, val_);
}

}  // namespace nebula
