/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_FUNCTIONCALLEXPRESSION_H_
#define COMMON_EXPRESSION_FUNCTIONCALLEXPRESSION_H_

#include "common/expression/Expression.h"

namespace nebula {
class ArgumentList final {
public:
    void addArgument(Expression* arg) {
        args_.emplace_back(arg);
    }

    auto&& args() {
        return std::move(args_);
    }

private:
    std::vector<std::unique_ptr<Expression>> args_;
};

class FunctionCallExpression final : public Expression {
public:
    FunctionCallExpression(std::string* name, ArgumentList* args)
        : Expression(Kind::kFunctionCall) {
        name_.reset(name);
        if (args != nullptr) {
            args_ = std::move(args)->args();
            delete args;
        }
    }

    void setExpCtxt(ExpressionContext* ctxt) override {
        expCtxt_ = ctxt;
        for (auto& arg : args_) {
            arg->setExpCtxt(ctxt);
        }
    }

    const Value& eval() override;

    std::string encode() const override {
        // TODO
        return "";
    }

    std::string decode() const override {
        // TODO
        return "";
    }

    std::string toString() const override {
        // TODO
        return "";
    }

private:
    std::unique_ptr<std::string>                name_;
    std::vector<std::unique_ptr<Expression>>    args_;
    Value                                       result_;
};
}   // namespace nebula
#endif
