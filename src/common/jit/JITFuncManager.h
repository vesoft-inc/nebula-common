/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_JIT_JITFUNCMANAGER_H_
#define COMMON_JIT_JITFUNCMANAGER_H_

#include "common/base/Base.h"
#include "common/jit/NebulaJIT.h"
#include "common/expression/Expression.h"
#include "common/jit/JITUtils.h"

namespace nebula {

class JITFuncManager {
public:
    JITFuncManager() {
        jit_ = std::make_unique<NebulaJIT>();
        builder_ = std::make_unique<llvm::IRBuilder<>>(context);
    }

    FuncPtr getOrCreateExprFunc(GraphSpaceID spaceId,
                                meta::SchemaManager* schemaMan,
                                Expression* expr) {
        auto id = expr->toString();
        {
            folly::RWSpinLock::ReadHolder rh(rwLock_);
            auto it = funcCache_.find(id);
            if (it != funcCache_.end()) {
                return it->second;
            }
        }
        {
            folly::RWSpinLock::WriteHolder rh(rwLock_);
            auto it = funcCache_.find(id);
            if (it != funcCache_.end()) {
                return it->second;
            }
            JITExprContext jitctx;
            jitctx.jit_ = jit_.get();
            jitctx.schemaMan_ = schemaMan;
            jitctx.builder_ = builder_.get();
            jitctx.spaceId_ = spaceId;
            jitctx.funcName_ = id;
            jitctx.expr_ = expr;
            FuncPtr func = JITUtils::createExprFunc(jitctx);
            if (func == nullptr) {
                return nullptr;
            }
            funcCache_.emplace(std::move(id), func);
            return func;
        }
    }

private:
    folly::RWSpinLock                        rwLock_;
    std::unordered_map<std::string, FuncPtr> funcCache_;
    llvm::LLVMContext                        context;
    std::unique_ptr<NebulaJIT>               jit_;
    std::unique_ptr<llvm::IRBuilder<>>       builder_;
};

}   // namespace nebula

#endif   // COMMON_JIT_JITFUNCMANAGER_H_
