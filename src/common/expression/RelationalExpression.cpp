/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/RelationalExpression.h"
#include "common/datatypes/List.h"

namespace nebula {

bool isLLVMString(llvm::Value* val) {
    return val->getType()->isPointerTy();
}

llvm::Value* stringCMP(llvm::IRBuilder<>* builder,
                       llvm::Module* module,
                       llvm::Value* l,
                       llvm::Value* r) {
    std::vector<llvm::Value*> args;
    args.reserve(2);
    args.emplace_back(l);
    args.emplace_back(r);
    VLOG(1) << "Call strcmp, l type " << l->getType()->getTypeID()
            << ", r type " << r->getType()->getTypeID();
    auto* strcmpFunc = module->getFunction("strcmp");
    if (strcmpFunc == nullptr) {
        VLOG(1) << "Create function strcmp";
        strcmpFunc = llvm::Function::Create(
                                llvm::FunctionType::get(builder->getInt32Ty(),
                                                       {builder->getInt8PtrTy(),
                                                        builder->getInt8PtrTy()},
                                                       false),
                                llvm::Function::ExternalLinkage,
                                "strcmp",
                                module);
    }
    auto* ret = builder->CreateCall(strcmpFunc, args, "calltmp");
    return ret;
}


const Value& RelationalExpression::eval(ExpressionContext& ctx) {
    auto& lhs = lhs_->eval(ctx);
    auto& rhs = rhs_->eval(ctx);

    switch (kind_) {
        case Kind::kRelEQ:
            result_ = lhs == rhs;
            break;
        case Kind::kRelNE:
            result_ = lhs != rhs;
            break;
        case Kind::kRelLT:
            result_ = lhs < rhs;
            break;
        case Kind::kRelLE:
            result_ = lhs <= rhs;
            break;
        case Kind::kRelGT:
            result_ = lhs > rhs;
            break;
        case Kind::kRelGE:
            result_ = lhs >= rhs;
            break;
        case Kind::kRelIn: {
            if (UNLIKELY(rhs.type() != Value::Type::LIST)) {
                result_ = Value(NullType::BAD_TYPE);
                break;
            }
            auto& list = rhs.getList().values;
            auto found = std::find(list.begin(), list.end(), lhs);
            if (found == list.end()) {
                result_ = false;
            } else {
                result_ = true;
            }
            break;
        }
        default:
            LOG(FATAL) << "Unknown type: " << kind_;
    }
    return result_;
}

llvm::Value* RelationalExpression::genGT(llvm::IRBuilder<>* builder,
                                         llvm::Module* module,
                                         llvm::Value* lhs,
                                         llvm::Value* rhs) const {
    if (isLLVMString(lhs) && isLLVMString(rhs)) {
        auto* ret = stringCMP(builder, module, lhs, rhs);
        return builder->CreateICmpSGT(ret, builder->getInt32(0), "icmpsgt");
    }
    return createBinOp(builder,
                       lhs,
                       rhs,
                       [builder] (llvm::Value* l, llvm::Value* r) -> llvm::Value* {
                           return builder->CreateICmpSGT(l, r, "icmpsgt");
                       },
                       [builder] (llvm::Value* l, llvm::Value* r) -> llvm::Value* {
                           VLOG(1) << "Call fcmpugt";
                           return builder->CreateFCmpUGT(l, r, "fcmpugt");
                       });
}

llvm::Value* RelationalExpression::genGE(llvm::IRBuilder<>* builder,
                                         llvm::Module* module,
                                         llvm::Value* lhs,
                                         llvm::Value* rhs) const {
    if (isLLVMString(lhs) && isLLVMString(rhs)) {
        auto* ret = stringCMP(builder, module, lhs, rhs);
        return builder->CreateICmpSGE(ret, builder->getInt32(0), "icmpsge");
    }
    return createBinOp(builder,
                       lhs,
                       rhs,
                       [builder] (llvm::Value* l, llvm::Value* r) -> llvm::Value* {
                           return builder->CreateICmpSGE(l, r, "icmpsge");
                       },
                       [builder] (llvm::Value* l, llvm::Value* r) -> llvm::Value* {
                           return builder->CreateFCmpUGE(l, r, "fcmpuge");
                       });
}

llvm::Value* RelationalExpression::genLT(llvm::IRBuilder<>* builder,
                                         llvm::Module* module,
                                         llvm::Value* lhs,
                                         llvm::Value* rhs) const {
    if (isLLVMString(lhs) && isLLVMString(rhs)) {
        auto* ret = stringCMP(builder, module, lhs, rhs);
        return builder->CreateICmpSLT(ret, builder->getInt32(0), "icmpslt");
    }
    return createBinOp(builder,
                       lhs,
                       rhs,
                       [builder] (llvm::Value* l, llvm::Value* r) -> llvm::Value* {
                           return builder->CreateICmpSLT(l, r, "icmpslt");
                       },
                       [builder] (llvm::Value* l, llvm::Value* r) -> llvm::Value* {
                           return builder->CreateFCmpULT(l, r, "fcmpult");
                       });
}

llvm::Value* RelationalExpression::genLE(llvm::IRBuilder<>* builder,
                                         llvm::Module* module,
                                         llvm::Value* lhs,
                                         llvm::Value* rhs) const {
    if (isLLVMString(lhs) && isLLVMString(rhs)) {
        auto* ret = stringCMP(builder, module, lhs, rhs);
        return builder->CreateICmpSLE(ret, builder->getInt32(0), "icmpsle");
    }

    return createBinOp(builder,
                       lhs,
                       rhs,
                       [builder] (llvm::Value* l, llvm::Value* r) -> llvm::Value* {
                           return builder->CreateICmpSLE(l, r, "icmpsle");
                       },
                       [builder] (llvm::Value* l, llvm::Value* r) -> llvm::Value* {
                           return builder->CreateFCmpULE(l, r, "fcmpule");
                       });
}

llvm::Value* RelationalExpression::genEQ(llvm::IRBuilder<>* builder,
                                         llvm::Module* module,
                                         llvm::Value* lhs,
                                         llvm::Value* rhs) const {
    if (isLLVMString(lhs) && isLLVMString(rhs)) {
        auto* ret = stringCMP(builder, module, lhs, rhs);
        return builder->CreateICmpEQ(ret, builder->getInt32(0), "icmpeq");
    }

    return createBinOp(builder,
                       lhs,
                       rhs,
                       [builder] (llvm::Value* l, llvm::Value* r) -> llvm::Value* {
                           return builder->CreateICmpEQ(l, r, "icmpeq");
                       },
                       [builder] (llvm::Value* l, llvm::Value* r) -> llvm::Value* {
                           return builder->CreateFCmpUEQ(l, r, "fcmpueq");
                       });
}

llvm::Value* RelationalExpression::genNE(llvm::IRBuilder<>* builder,
                                         llvm::Module* module,
                                         llvm::Value* lhs,
                                         llvm::Value* rhs) const {
    if (isLLVMString(lhs) && isLLVMString(rhs)) {
        auto* ret = stringCMP(builder, module, lhs, rhs);
        return builder->CreateICmpNE(ret, builder->getInt32(0), "icmpne");
    }
    return createBinOp(builder,
                       lhs,
                       rhs,
                       [builder] (llvm::Value* l, llvm::Value* r) -> llvm::Value* {
                           return builder->CreateICmpNE(l, r, "icmpne");
                       },
                       [builder] (llvm::Value* l, llvm::Value* r) -> llvm::Value* {
                           return builder->CreateFCmpUNE(l, r, "fcmpune");
                       });
}

llvm::Value* RelationalExpression::codegen(ExprCodegenContext& ctx) const {
    auto* builder = ctx.builder_;
    if (builder == nullptr) {
        return nullptr;
    }
    auto* l = lhs_->codegen(ctx);
    auto* r = rhs_->codegen(ctx);
    if (!l || !r) {
        return nullptr;
    }
    switch (kind_) {
        case Kind::kRelEQ:
            return genEQ(builder, ctx.module_, l, r);
        case Kind::kRelNE:
            return genNE(builder, ctx.module_, l, r);
        case Kind::kRelLT:
            return genLT(builder, ctx.module_, l, r);
        case Kind::kRelLE:
            return genLE(builder, ctx.module_, l, r);
        case Kind::kRelGT:
            return genGT(builder, ctx.module_, l, r);
        case Kind::kRelGE:
            return genGE(builder, ctx.module_, l, r);
        case Kind::kRelIn: {
            if (r->getType()->isArrayTy()) {
                // The r should be a constant
                if (auto* c = llvm::dyn_cast<llvm::Constant>(r)) {
                    auto* constArr = static_cast<llvm::ConstantArray*>(c);
                    auto num = constArr->getType()->getArrayNumElements();
                    std::vector<llvm::Value*> ops;
                    ops.reserve(num);
                    for (unsigned i = 0; i < num; i++) {
                        auto* tmp = genEQ(builder,
                                          ctx.module_,
                                          l,
                                          constArr->getAggregateElement(i));
                        if (tmp != nullptr) {
                            ops.emplace_back(tmp);
                        }
                    }
                    VLOG(1) << "Total ops size " << ops.size() << ", ops number " << num;
                    if (ops.empty()) {
                        VLOG(1) << "The list is empty, return false forever!";
                        return builder->getFalse();
                    }
                    return builder->CreateOr(ops);
                } else {
                    LOG(WARNING) << "The list should be constant!";
                }
            }
            break;
        }
        default:
            break;
    }
    return nullptr;
}

}   // namespace nebula
