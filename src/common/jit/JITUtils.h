/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_JIT_JITUTILS_H_
#define COMMON_JIT_JITUTILS_H_

#include "common/base/Base.h"
#include "common/datatypes/Value.h"
#include "common/jit/NebulaJIT.h"
#include "common/expression/Expression.h"
#include "common/meta/SchemaManager.h"
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>

#define USE_JIT_RESULT 1

namespace nebula {

// TODO(heng)
// Because llvm IR don't support union, to support return multi type
// results, use a structure here. It occupied more memory. I will
// figure out a better way for it.
// Currently, we only support numeric types.
class JITResult {
public:
    enum class Type : uint8_t {
        JIT_NULL = 0x00,
        INT64    = 0x01,
        DOUBLE   = 0x02,
        BOOL     = 0x03,
        STR      = 0x04,
        UNKNOWN  = 0xFF,
    };
    JITResult() = default;

    JITResult(int64_t i) {  // NOLINT
        type_ = Type::INT64;
        iVal_ = i;
    }

    JITResult(double d) {  // NOLINT
        type_ = Type::DOUBLE;
        dVal_ = d;
    }

    JITResult(bool b) {  // NOLINT
        type_ = Type::BOOL;
        iVal_ = b;
    }

    JITResult(const std::string& str) {  // NOLINT
        type_ = Type::STR;
        sVal_ = str.c_str();
    }

    bool operator==(const JITResult& that) const {
        VLOG(1) << "this " << toString()
                << ", that " << that.toString();
        if (this->type_ != that.type_) {
            return false;
        }
        if (this->type_ == Type::INT64
                || this->type_ == Type::BOOL) {
            return this->iVal_ == that.iVal_;
        }
        if (this->type_ == Type::DOUBLE) {
            return this->dVal_ == that.dVal_;
        }
        if (this->type_ == Type::STR) {
            return strcmp(sVal_, that.sVal_) == 0;
        }
        // null != null in our case.
        return false;
    }

    int64_t getInt() const {
        return iVal_;
    }

    bool getBool() const {
        return static_cast<bool>(iVal_);
    }

    double getDouble() const {
        return dVal_;
    }

    const char* getStr() const {
        return sVal_;
    }

    Type getType() const {
        return type_;
    }

    bool isNull() const {
        return type_ == Type::JIT_NULL;
    }

    std::string toString() const {
        std::stringstream ss;
        switch (type_) {
            case Type::JIT_NULL:
                ss << "Null value";
                break;
            case Type::INT64:
                ss << "Int value " << iVal_;
                break;
            case Type::DOUBLE:
                ss << "Double value " << dVal_;
                break;
            case Type::BOOL:
                ss << "Bool value " << iVal_;
                break;
            case Type::STR:
                ss << "String Value " << sVal_;
                break;
            case Type::UNKNOWN:
                ss << "Unknown";
                break;
        }
        ss << std::endl;
        return ss.str();
    }

private:
    Type            type_ = Type::JIT_NULL;
    // for bool, it will reuse this field.
    int64_t         iVal_;
    double          dVal_;
    const char*     sVal_;
};

struct JITExprContext {
    GraphSpaceID         spaceId_;
    NebulaJIT*           jit_ = nullptr;
    meta::SchemaManager* schemaMan_ = nullptr;
    llvm::IRBuilder<>*   builder_ = nullptr;
    std::string          funcName_;
    Expression*          expr_ = nullptr;
};

struct ExprCodegenContext {
    llvm::IRBuilder<>*          builder_ = nullptr;
    llvm::Module*               module_ = nullptr;
    llvm::Argument*             exprCtx_ = nullptr;
    llvm::Value*                output_ = nullptr;
    meta::SchemaManager*        schemaMan_ = nullptr;
    GraphSpaceID                spaceId_ = -1;
};

typedef void (*FuncPtr)(intptr_t, JITResult* r);

class JITUtils {
public:
    static FuncPtr createExprFunc(JITExprContext& jitExprCtx);

    static llvm::Value* convertToBool(llvm::IRBuilder<>* builder, llvm::Value* val);

private:
    JITUtils() = delete;
};

}   // namespace nebula
#endif   // COMMON_JIT_JITUTILS_H_
