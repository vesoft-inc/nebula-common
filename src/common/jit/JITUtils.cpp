/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/jit/JITUtils.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/IR/Verifier.h"

namespace nebula {

const Value& nebulaGetValue(intptr_t ctx, intptr_t sym, intptr_t prop, Expression::Kind kind) {
    ExpressionContext* exprCtx = reinterpret_cast<ExpressionContext*>(ctx);
    std::string* symName = reinterpret_cast<std::string*>(sym);
    std::string* propName = reinterpret_cast<std::string*>(prop);
    if (!exprCtx || !symName || !propName) {
        LOG(FATAL) << "Can't access the invalid memory address!";
    }
    VLOG(1) << "symName = " << *symName
            << ", propName = " << *propName
            << ", kind is " << static_cast<int32_t>(kind);
    switch (kind) {
        case Expression::Kind::kEdgeProperty:
        case Expression::Kind::kEdgeDst:
        case Expression::Kind::kEdgeSrc:
        case Expression::Kind::kEdgeType:
        case Expression::Kind::kEdgeRank: {
            const auto& value = exprCtx->getEdgeProp(*symName, *propName);
            VLOG(1) << "symName = " << *symName
                    << ", propName = " << *propName
                    << ", value type = " << static_cast<int32_t>(value.type());
            return value;
        }
        case Expression::Kind::kSrcProperty: {
            const auto& value = exprCtx->getSrcProp(*symName, *propName);
            VLOG(1) << "symName = " << *symName
                    << ", propName = " << *propName
                    << ", value type = " << static_cast<int32_t>(value.type());
            return value;
        }
        case Expression::Kind::kDstProperty: {
            const auto& value = exprCtx->getDstProp(*symName, *propName);
            VLOG(1) << "symName = " << *symName
                    << ", propName = " << *propName
                    << ", value type = " << static_cast<int32_t>(value.type());
            return value;
        }
        default:
            break;
    }
    LOG(WARNING) << "Unsupport kind " << static_cast<int32_t>(kind);
    return Value::null();
}

using IntOrNull = std::pair<bool, int64_t>;
using DoubleOrNull = std::pair<bool, double>;
using PointerOrNull = std::pair<bool, const char*>;

extern "C" IntOrNull nebulaIGetValue(intptr_t ctx,
                                     intptr_t sym,
                                     intptr_t prop,
                                     Expression::Kind kind) {
    const auto& value = nebulaGetValue(ctx, sym, prop, kind);
    IntOrNull ret;
    switch (value.type()) {
        case Value::Type::INT:
            ret.first = false;
            ret.second = value.getInt();
            break;
        case Value::Type::FLOAT:
            ret.first = false;
            ret.second = static_cast<int64_t>(value.getFloat());
            break;
        case Value::Type::BOOL:
            ret.first = false;
            ret.second = static_cast<int64_t>(value.getBool());
            break;
        default:
            ret.first = true;
            ret.second = 0;
            break;
    }
    return ret;
}

extern "C" DoubleOrNull nebulaDGetValue(intptr_t ctx,
                                        intptr_t sym,
                                        intptr_t prop,
                                        Expression::Kind kind) {
    const auto& value = nebulaGetValue(ctx, sym, prop, kind);
    DoubleOrNull ret;
    switch (value.type()) {
        case Value::Type::FLOAT:
            ret.first = false;
            ret.second = value.getFloat();
            break;
        case Value::Type::INT:
            ret.first = false;
            ret.second = static_cast<double>(value.getInt());
            break;
        case Value::Type::BOOL:
            ret.first = false;
            ret.second = static_cast<double>(value.getBool());
            break;
        default:
            ret.first = true;
            ret.second = 0.0;
            break;
    }
    return ret;
}

extern "C" PointerOrNull nebulaSGetValue(intptr_t ctx,
                                         intptr_t sym,
                                         intptr_t prop,
                                         Expression::Kind kind) {
    const auto& value = nebulaGetValue(ctx, sym, prop, kind);
    PointerOrNull ret;
    switch (value.type()) {
        case Value::Type::STRING:
            ret.first = false;
            ret.second = reinterpret_cast<const char*>(value.getStr().data());
            break;
        default:
            ret.first = true;
            ret.second = nullptr;
            break;
    }
    return ret;
}


std::string moduleToStr(llvm::Module* module) {
    std::string outStr;
    llvm::raw_string_ostream out(outStr);
    module->print(out, nullptr);
    return outStr;
}


FuncPtr JITUtils::createExprFunc(JITExprContext& ctx) {
    auto* builder = ctx.builder_;
    auto module = std::make_unique<llvm::Module>("NeublaExprJIT", builder->getContext());
    module->setDataLayout(ctx.jit_->getTargetMachine().createDataLayout());

    // Create a new pass manager attached to it.
    auto theFPM = std::make_unique<llvm::legacy::FunctionPassManager>(module.get());
    // Promote allocas to registers.
    theFPM->add(llvm::createPromoteMemoryToRegisterPass());
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    theFPM->add(llvm::createInstructionCombiningPass());
    // Reassociate expressions.
    theFPM->add(llvm::createReassociatePass());
    // Eliminate Common SubExpressions.
    theFPM->add(llvm::createGVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    theFPM->add(llvm::createCFGSimplificationPass());
    theFPM->doInitialization();

    auto* jitResultType = llvm::StructType::create(
                                            builder->getContext(),
                                            {builder->getInt8Ty(),    // type
                                             builder->getInt64Ty(),
                                             builder->getDoubleTy(),
                                             builder->getInt8PtrTy()},
                                            "JITResult");
    auto* sly = module->getDataLayout().getStructLayout(jitResultType);
    VLOG(1) << "The returned type size is " << sly->getSizeInBytes()
            << ", alignment " << sly->getAlignment()
            << ", the first element offset is " << sly->getElementOffset(0)
            << ", the second element offset is " << sly->getElementOffset(1)
            << ", the third element offset is " << sly->getElementOffset(2);
    llvm::FunctionType* funcType =
                llvm::FunctionType::get(builder->getVoidTy(),
                                        {builder->getInt64Ty(),
                                         llvm::PointerType::getUnqual(jitResultType)},
                                        false);
    llvm::Function* func =
                llvm::Function::Create(funcType,
                                       llvm::Function::ExternalLinkage,
                                       ctx.funcName_,
                                       module.get());

    auto* agg = func->arg_begin() + 1;
    llvm::BasicBlock* BB = llvm::BasicBlock::Create(builder->getContext(),
                                                    "entry",
                                                    func);
    builder->SetInsertPoint(BB);

    ExprCodegenContext ecctx;
    ecctx.builder_ = builder;
    ecctx.module_ = module.get();
    ecctx.exprCtx_ = func->arg_begin();
    ecctx.schemaMan_ = ctx.schemaMan_;
    ecctx.spaceId_ = ctx.spaceId_;
    ecctx.output_ = agg;

    auto* val = ctx.expr_->codegen(ecctx);
    if (val == nullptr) {
        LOG(WARNING) << "No function generated!";
        return (intptr_t)nullptr;
    }

    auto* typeField = builder->CreateConstInBoundsGEP2_32(jitResultType, agg, 0, 0);
    switch (val->getType()->getTypeID()) {
        case llvm::Type::TypeID::IntegerTyID: {
            auto* valueField = builder->CreateConstInBoundsGEP2_32(jitResultType, agg, 0, 1);
            if (val->getType()->isIntegerTy(1)) {
                builder->CreateStore(builder->getInt8(3), typeField);
                auto* extValue = builder->CreateZExt(val, builder->getInt64Ty());
                builder->CreateStore(extValue, valueField);
            } else {
                builder->CreateStore(builder->getInt8(1), typeField);
                builder->CreateStore(val, valueField);
            }
            break;
        }
        case llvm::Type::TypeID::DoubleTyID: {
            builder->CreateStore(builder->getInt8(2), typeField);
            auto* valueField = builder->CreateConstInBoundsGEP2_32(jitResultType, agg, 0, 2);
            builder->CreateStore(val, valueField);
            break;
        }
        case llvm::Type::TypeID::PointerTyID: {
            builder->CreateStore(builder->getInt8(4), typeField);
            auto* valueField = builder->CreateConstInBoundsGEP2_32(jitResultType, agg, 0, 3);
            builder->CreateStore(val, valueField);
            break;
        }
        default:
            LOG(FATAL) << "Unsupport type, typeId is "
                       << static_cast<int32_t>(val->getType()->getTypeID());
            break;
    }
    builder->CreateRetVoid();
    std::string errorMsgStr;
    llvm::raw_string_ostream rso(errorMsgStr);
    if (llvm::verifyFunction(*func, &rso)) {
        LOG(ERROR) << "Verify function failed, error " << errorMsgStr;
        return nullptr;
    }
    theFPM->run(*func);
    VLOG(1) << "The expression codegen is " << moduleToStr(module.get());
    ctx.jit_->addModule(std::move(module));
    auto exprSymbol = ctx.jit_->findSymbol(ctx.funcName_);
    assert(exprSymbol && "Function not found");
    return (FuncPtr)(intptr_t)cantFail(exprSymbol.getAddress());
}

llvm::Value* JITUtils::convertToBool(llvm::IRBuilder<>* builder, llvm::Value* val) {
    if (val->getType()->isIntegerTy(1)) {
        return val;
    } else if (val->getType()->isIntegerTy(64)) {
        return builder->CreateICmpNE(val, builder->getInt64(0));
    } else if (val->getType()->isDoubleTy()) {
        return builder->CreateFCmpUNE(val, llvm::ConstantFP::get(builder->getDoubleTy(), 0.0));
    }
    LOG(WARNING) << "Unsupport tye for logical expression!";
    return nullptr;
}


}   // namespace nebula
