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
    void addArgument(std::unique_ptr<Expression> arg) {
        CHECK(!!arg);
        args_.emplace_back(std::move(arg));
    }

    auto moveArgs() {
        return std::move(args_);
    }

    const std::vector<Expression*> args() const {
        std::vector<Expression*> result;
        result.resize(args_.size());
        auto get = [] (auto &ptr) {return ptr.get(); };
        std::transform(args_.begin(), args_.end(), result.begin(), get);
        return result;
    }

    size_t numArgs() const {
        return args_.size();
    }

    bool operator==(const ArgumentList& rhs) const;

private:
    std::vector<std::unique_ptr<Expression>> args_;
};


class FunctionCallExpression final : public Expression {
    friend class Expression;

public:
    FunctionCallExpression(std::string* name = nullptr,
                           ArgumentList* args = nullptr)
        : Expression(Kind::kFunctionCall)
        , name_(name)
        , args_(args) {}

    const Value& eval(ExpressionContext& ctx) override;

    bool operator==(const Expression& rhs) const override;

    std::string toString() const override;

    const std::string* name() const {
        return name_.get();
    }

    const ArgumentList* args() const {
        return args_.get();
    }

private:
    void writeTo(Encoder& encoder) const override;

    void resetFrom(Decoder& decoder) override;

    std::unique_ptr<std::string>    name_;
    std::unique_ptr<ArgumentList>   args_;
    Value                           result_;
};

}  // namespace nebula
#endif  // EXPRESSION_FUNCTIONCALLEXPRESSION_H_
