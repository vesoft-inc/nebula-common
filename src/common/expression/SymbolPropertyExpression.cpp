/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/SymbolPropertyExpression.h"
#include "common/meta/NebulaSchemaProvider.h"
#include "common/jit/JITUtils.h"

namespace nebula {

bool SymbolPropertyExpression::operator==(const Expression& rhs) const {
    if (kind_ != rhs.kind()) {
        return false;
    }

    const auto& r = dynamic_cast<const SymbolPropertyExpression&>(rhs);
    return *ref_ == *(r.ref_) && *sym_ == *(r.sym_) && *prop_ == *(r.prop_);
}


void SymbolPropertyExpression::writeTo(Encoder& encoder) const {
    // kind_
    encoder << kind_;

    // ref_
    encoder << ref_.get();

    // alias_
    encoder << sym_.get();

    // prop_
    encoder << prop_.get();
}


void SymbolPropertyExpression::resetFrom(Decoder& decoder) {
    // Read ref_
    ref_ = decoder.readStr();

    // Read alias_
    sym_ = decoder.readStr();

    // Read prop_
    prop_ = decoder.readStr();
}


const Value& EdgePropertyExpression::eval(ExpressionContext& ctx) {
    return ctx.getEdgeProp(*sym_, *prop_);
}


const Value& InputPropertyExpression::eval(ExpressionContext& ctx) {
    // TODO
    return ctx.getInputProp(*prop_);
}


const Value& VariablePropertyExpression::eval(ExpressionContext& ctx) {
    return ctx.getVarProp(*sym_, *prop_);
}


const Value& SourcePropertyExpression::eval(ExpressionContext& ctx) {
    return ctx.getSrcProp(*sym_, *prop_);
}


const Value& DestPropertyExpression::eval(ExpressionContext& ctx) {
    return ctx.getDstProp(*sym_, *prop_);
}


const Value& EdgeSrcIdExpression::eval(ExpressionContext& ctx) {
    return ctx.getEdgeProp(*sym_, *prop_);
}


const Value& EdgeTypeExpression::eval(ExpressionContext& ctx) {
    return ctx.getEdgeProp(*sym_, *prop_);
}


const Value& EdgeRankExpression::eval(ExpressionContext& ctx) {
    return ctx.getEdgeProp(*sym_, *prop_);
}


const Value& EdgeDstIdExpression::eval(ExpressionContext& ctx) {
    return ctx.getEdgeProp(*sym_, *prop_);
}

StatusOr<meta::cpp2::PropertyType>
SymbolPropertyExpression::getFieldType(meta::SchemaManager* schemaMan,
                                       GraphSpaceID spaceId) const {
    switch (kind_) {
        case Expression::Kind::kEdgeProperty: {
            auto edgeTypeRet = schemaMan->toEdgeType(spaceId, *sym_);
            if (!edgeTypeRet.ok()) {
                LOG(WARNING) << "Can't find edge " << *sym_;
                return Status::EdgeNotFound();
            }
            auto schema = schemaMan->getEdgeSchema(spaceId,
                                                   edgeTypeRet.value());
            if (!schema) {
                LOG(WARNING) << "Can't find edge " << *sym_;
                return Status::EdgeNotFound();
            }
            auto fieldType = schema->getFieldType(*prop_);
            VLOG(1) << "edgeName = " << *sym_
                    << ", propName = " << *prop_
                    << ", type = " << static_cast<int32_t>(fieldType);
            return fieldType;
        }
        case Expression::Kind::kEdgeDst:
        case Expression::Kind::kEdgeSrc: {
            // TODO(heng): check the edge exist or not.
            return meta::cpp2::PropertyType::FIXED_STRING;
        }
        case Expression::Kind::kEdgeType:
        case Expression::Kind::kEdgeRank: {
            // TODO(heng): check the edge exist or not.
            return meta::cpp2::PropertyType::INT64;
        }
        case Expression::Kind::kSrcProperty:
        case Expression::Kind::kDstProperty: {
            auto tagRet = schemaMan->toTagID(spaceId, *sym_);
            if (!tagRet.ok()) {
                LOG(WARNING) << "Can't find tag " << *sym_;
                return Status::TagNotFound();
            }
            auto schema = schemaMan->getTagSchema(spaceId,
                                                  tagRet.value());
            if (!schema) {
                LOG(WARNING) << "Can't find tag " << *sym_;
                return Status::TagNotFound();
            }
            auto fieldType = schema->getFieldType(*prop_);
            VLOG(1) << "tagName = " << *sym_
                    << ", propName = " << *prop_
                    << ", type = " << static_cast<int32_t>(fieldType);
            return fieldType;
        }
        default:
            break;
    }
    LOG(WARNING) << "Unsuppport expression type " << static_cast<int32_t>(kind_);
    return Status::Error("Unsupport now!");
}

llvm::Value* SymbolPropertyExpression::codegen(ExprCodegenContext& ctx) const {
    if (kind_ == Kind::kInputProperty
            || kind_ == Kind::kVarProperty) {
        LOG(WARNING) << "Unsupport codegen for this type!";
        return nullptr;
    }
    if (!ctx.schemaMan_
            || !sym_
            || !prop_
            || !ctx.module_) {
        LOG(WARNING) << "ExprCodegenContext is not inited!";
        return nullptr;
    }
    auto* builder = ctx.builder_;

    auto fieldTypeRet = getFieldType(ctx.schemaMan_, ctx.spaceId_);
    if (!fieldTypeRet.ok()) {
        LOG(WARNING) << "Can't fetch the related field type!";
        return nullptr;
    }
    auto fieldType = fieldTypeRet.value();
    // TODO(heng): Currently we have not handle the STRING type.
    std::pair<std::string, llvm::Type*> nameAndType;
    switch (fieldType) {
        case nebula::meta::cpp2::PropertyType::TIMESTAMP:
        case nebula::meta::cpp2::PropertyType::BOOL:
        case nebula::meta::cpp2::PropertyType::INT8:
        case nebula::meta::cpp2::PropertyType::INT16:
        case nebula::meta::cpp2::PropertyType::INT32:
        case nebula::meta::cpp2::PropertyType::INT64: {
            nameAndType.first  = "nebulaIGetValue";
            nameAndType.second = llvm::StructType::get(builder->getContext(),
                                                       {builder->getInt1Ty(),
                                                        builder->getInt64Ty()});
            break;
        }
        case nebula::meta::cpp2::PropertyType::FLOAT:
        case nebula::meta::cpp2::PropertyType::DOUBLE: {
            nameAndType.first  = "nebulaDGetValue";
            nameAndType.second = llvm::StructType::get(builder->getContext(),
                                                       {builder->getInt1Ty(),
                                                        builder->getDoubleTy()});
            break;
        }
        case nebula::meta::cpp2::PropertyType::STRING:
        case nebula::meta::cpp2::PropertyType::FIXED_STRING: {
            nameAndType.first  = "nebulaSGetValue";
            nameAndType.second = llvm::StructType::get(builder->getContext(),
                                                       {builder->getInt1Ty(),
                                                       builder->getInt8PtrTy()});
            break;
        }
        default:
            LOG(WARNING) << "Unsupportted type!";
            return nullptr;
    }

    std::vector<llvm::Value*> args;
    args.emplace_back(ctx.exprCtx_);
    args.emplace_back(builder->getInt64(reinterpret_cast<intptr_t>(sym_.get())));
    args.emplace_back(builder->getInt64(reinterpret_cast<intptr_t>(prop_.get())));
    args.emplace_back(builder->getInt8(static_cast<uint8_t>(kind_)));

    VLOG(1) << "Call " << nameAndType.first;
    auto* getPropFunc = ctx.module_->getFunction(nameAndType.first);
    if (getPropFunc == nullptr) {
        VLOG(1) << "Create function " << nameAndType.first;
        getPropFunc = llvm::Function::Create(
                                llvm::FunctionType::get(nameAndType.second,
                                                       {builder->getInt64Ty(),
                                                        builder->getInt64Ty(),
                                                        builder->getInt64Ty(),
                                                        builder->getInt8Ty()},
                                                       false),
                                llvm::Function::ExternalLinkage,
                                nameAndType.first,
                                ctx.module_);
    }
    auto* agg = builder->CreateCall(getPropFunc, args, "calltmp");

    VLOG(1) << "agg type is " << static_cast<int32_t>(agg->getType()->getTypeID());
    auto* isNull = builder->CreateExtractValue(agg, 0);
    llvm::Function* currFunction = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* thenBB =
                        llvm::BasicBlock::Create(builder->getContext(), "then", currFunction);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(builder->getContext(), "else");
    builder->CreateCondBr(isNull, thenBB, elseBB);
    builder->SetInsertPoint(thenBB);
    CHECK_NOTNULL(ctx.output_);
    auto* typeField = builder->CreateConstInBoundsGEP2_32(nullptr,
                                                          ctx.output_,
                                                          0,
                                                          0);
    builder->CreateStore(builder->getInt8(0), typeField);
    builder->CreateRetVoid();

    currFunction->getBasicBlockList().push_back(elseBB);
    builder->SetInsertPoint(elseBB);
    return builder->CreateExtractValue(agg, 1);
}

}  // namespace nebula
