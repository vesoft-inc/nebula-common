/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_FUNCTIONCALLEXPRESSION_H_
#define COMMON_EXPRESSION_FUNCTIONCALLEXPRESSION_H_

#include <boost/algorithm/string.hpp>

#include "common/function/FunctionManager.h"
#include "common/expression/Expression.h"

namespace nebula {

class ArgumentList final {
public:
    static ArgumentList* make(ObjectPool* pool = nullptr, size_t sz = 0) {
        DCHECK(!!pool);
        return pool->add(new ArgumentList(sz));
    }

    ArgumentList() = default;
    explicit ArgumentList(size_t sz) {
        args_.reserve(sz);
    }

    void addArgument(Expression* arg) {
        CHECK(!!arg);
        args_.emplace_back(arg);
    }

    const auto& args() const {
        return args_;
    }

    auto& args() {
        return args_;
    }

    size_t numArgs() const {
        return args_.size();
    }

    void setArg(size_t i, Expression* arg) {
        DCHECK_LT(i, numArgs());
        args_[i] = arg;
    }

    void setArgs(std::vector<Expression*> args) {
        args_ = args;
    }

    bool operator==(const ArgumentList& rhs) const;

private:
    std::vector<Expression*> args_;
};


class FunctionCallExpression final : public Expression {
    friend class Expression;

public:
    // TODO(aiee) replace new ArgumentList() with make()
    static FunctionCallExpression* make(ObjectPool* pool = nullptr,
                                        const std::string& name = "",
                                        ArgumentList* args = new ArgumentList()) {
        return pool->add(new FunctionCallExpression(name, args));
    }

    explicit FunctionCallExpression(const std::string& name = "",
                                    ArgumentList* args = new ArgumentList())
        : Expression(Kind::kFunctionCall), name_(name), args_(args) {
        if (!name_.empty()) {
            auto funcResult = FunctionManager::get(name_, DCHECK_NOTNULL(args_)->numArgs());
            if (funcResult.ok()) {
                func_ = funcResult.value();
            }
        }
    }

    // FunctionCallExpression(ObjectPool* pool = nullptr,
    //                        std::string* name = nullptr,
    //                        ArgumentList* args  = new ArgumentList())
    //     : Expression(Kind::kFunctionCall), name_(name), args_(pool->add(args)) {
    //     if (name_ != nullptr) {
    //         auto funcResult = FunctionManager::get(*name_, DCHECK_NOTNULL(args_)->numArgs());
    //         if (funcResult.ok()) {
    //             func_ = funcResult.value();
    //         }
    //     }
    // }

    const Value& eval(ExpressionContext& ctx) override;

    bool operator==(const Expression& rhs) const override;

    std::string toString() const override;

    void accept(ExprVisitor* visitor) override;

    std::unique_ptr<Expression> clone() const override {
        auto arguments = new ArgumentList(args_->numArgs());
        for (auto& arg : args_->args()) {
            arguments->addArgument(arg->clone().get());
        }
        return std::make_unique<FunctionCallExpression>(name_, arguments);
    }

<<<<<<< HEAD
    const std::string& name() const {
=======
    const std::string* name() const {
>>>>>>> Refactor expressions up to LogiacalExpression
        return name_;
    }

    bool isFunc(const std::string &name) const {
        return boost::iequals(name, name_);
    }

    const ArgumentList* args() const {
        return args_;
    }

    ArgumentList* args() {
        return args_;
    }

private:
    void writeTo(Encoder& encoder) const override;
    void resetFrom(Decoder& decoder) override;

<<<<<<< HEAD
    std::string name_;
    std::unique_ptr<ArgumentList> args_;
=======
    std::string*                                 name_;
    ArgumentList*                                args_;
>>>>>>> Refactor expressions up to LogiacalExpression

    // runtime cache
    Value                                        result_;
    FunctionManager::Function                    func_;
};

}  // namespace nebula
#endif  // EXPRESSION_FUNCTIONCALLEXPRESSION_H_
