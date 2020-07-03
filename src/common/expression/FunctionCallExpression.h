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

    const auto& args() const {
        return args_;
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

protected:
    bool traversal(Visitor visitor) const override {
        if (!visitor(this)) {
            return false;
        }
        for (const auto &arg : args_->args()) {
            if (!arg->traversal(visitor)) {
                return false;
            }
        }
        return true;
    }

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
