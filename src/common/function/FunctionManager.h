/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_FUNCTION_FUNCTIONMANAGER_H_
#define COMMON_FUNCTION_FUNCTIONMANAGER_H_

#include "common/base/StatusOr.h"
#include "common/base/Status.h"
#include "common/datatypes/Value.h"
#include <folly/futures/Future.h>

/**
 * FunctionManager is for managing builtin and dynamic-loaded functions,
 * which users could use as function call expressions.
 *
 * TODO(dutor) To implement dynamic loading.
 */

namespace nebula {

struct TypeSignature {
    TypeSignature() = default;
    TypeSignature(std::vector<Value::Type> argsType, Value::Type returnType)
        :argsType_(std::move(argsType)), returnType_(returnType) {}
    std::vector<Value::Type> argsType_;
    Value::Type returnType_;
};

class FunctionManager final {
public:
    using Function = std::function<Value(const std::vector<Value>&)>;

    /**
     * To obtain a function named `func', with the actual arity.
     */
    static StatusOr<Function> get(const std::string &func, size_t arity);

    /**
     * To load a set of functions from a shared object dynamically.
     */
    static Status load(const std::string &soname, const std::vector<std::string> &funcs);

    /**
     * To unload a shared object.
     */
    static Status unload(const std::string &soname, const std::vector<std::string> &funcs);

    /**
     * To obtain the return value type according to the parameter type
     */
    static StatusOr<Value::Type> getReturnType(const std::string &funcName,
                                               const std::vector<Value::Type> &argsType,
                                               const std::string &scope = "");

private:
    /**
     * FunctionManager functions as a singleton, since the dynamic loading is process-wide.
     */
    FunctionManager();

    static FunctionManager &instance();

    StatusOr<Function> getInternal(const std::string &func,
                                   size_t arity,
                                   const std::string &scope = "") const;

    Status loadInternal(const std::string &soname, const std::vector<std::string> &funcs);

    Status unloadInternal(const std::string &soname, const std::vector<std::string> &funcs);

    struct FunctionAttributes final {
        size_t minArity_{0};
        size_t maxArity_{0};
        Function body_;
    };

    struct FunctionAttr {
        std::string name;
        FunctionAttributes attributes;
        std::vector<TypeSignature> signatures;
    };

    struct ScopedFunction {
        std::string scope;
        std::vector<FunctionAttr> functions;
    };

    class ScopedFunctions final {
    public:
        ScopedFunctions(std::initializer_list<ScopedFunction> &&scopedFunctions) {
            for (auto &scopedFunction : scopedFunctions) {
                if (functionScopedIndexes_.find(scopedFunction.scope) !=
                    functionScopedIndexes_.end()) {
                    // already exists scoped
                    // do nothing about it
                    continue;
                }
                functions_.emplace_back();
                functionSignatures_.emplace_back();
                for (auto &function : scopedFunction.functions) {
                    functions_.back().emplace(function.name, std::move(function.attributes));
                    functionSignatures_.back().emplace(std::move(function.name),
                                                       std::move(function.signatures));
                }
                functionScopedIndexes_.emplace(scopedFunction.scope, functions_.size() - 1);
            }
        }

        StatusOr<std::unordered_map<std::string, FunctionAttributes>::const_iterator>
        find(const std::string &scope, const std::string &name) const {
            const auto scopedIndex = functionScopedIndexes_.find(scope);
            if (scopedIndex == functionScopedIndexes_.end()) {
                return Status::Error("No function scoped named `%s'.", scope.c_str());
            }
            const auto &functions = functions_[scopedIndex->second];
            const auto function = functions.find(name);
            if (function == functions.end()) {
                return Status::Error("No function named `%s' in scope `%s'.",
                                     name.c_str(),
                                     scope.c_str());
            }
            return function;
        }

        StatusOr<std::unordered_map<std::string, std::vector<TypeSignature>>::const_iterator>
        findSignature(const std::string &scope, const std::string &name) const {
            const auto scopedIndex = functionScopedIndexes_.find(scope);
            if (scopedIndex == functionScopedIndexes_.end()) {
                return Status::Error("No function scoped named `%s'.", scope.c_str());
            }
            const auto &functionSignatures = functionSignatures_[scopedIndex->second];
            const auto functionSignature = functionSignatures.find(name);
            if (functionSignature == functionSignatures.end()) {
                return Status::Error("No function named `%s' in scope `%s'.",
                                     name.c_str(),
                                     scope.c_str());
            }
            return functionSignature;
        }

    private:
        // Scope->index
        std::unordered_map<std::string, std::size_t>                     functionScopedIndexes_;
        // Function->attribute
        std::vector<std::unordered_map<std::string, FunctionAttributes>> functions_;
        // Function->signature
        std::vector<std::unordered_map<std::string, std::vector<TypeSignature>>>
                                                                         functionSignatures_;
    };

    static const Function abs;
    static const Function floor;
    static const Function ceil;
    static const Function round;
    static const Function sqrt;
    static const Function cbrt;
    static const Function hypot;
    static const Function pow;
    static const Function exp;
    static const Function exp2;
    static const Function log;
    static const Function log2;
    static const Function log10;
    static const Function sin;
    static const Function asin;
    static const Function cos;
    static const Function acos;
    static const Function tan;
    static const Function atan;
    static const Function rand32;
    static const Function rand64;
    static const Function now;
    static const Function strcasecmp;
    static const Function lower;
    static const Function upper;
    static const Function length;
    static const Function trim;
    static const Function ltrim;
    static const Function rtrim;
    static const Function left;
    static const Function right;
    static const Function lpad;
    static const Function rpad;
    static const Function substr;
    static const Function hash;
    static const Function udf_is_in;
    static const Function near;
    static const Function cos_similarity;
    static const Function size;

    ScopedFunctions functions_;
};

}   // namespace nebula

#endif  // COMMON_FUNCTION_FUNCTIONMANAGER_H_
