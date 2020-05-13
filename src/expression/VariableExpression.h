/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef EXPRESSION_VARIABLEEXPRESSION_H_
#define EXPRESSION_VARIABLEEXPRESSION_H_

#include "expression/Expression.h"

namespace nebula {
class VariableExpression final : public Expression {
public:
    explicit VariableExpression(std::string* var)
        : Expression(Type::EXP_VAR) {
        var_.reset(var);
    }

    void setExpCtxt(ExpressionContext* ctxt) override {
        expCtxt_ = ctxt;
    }

    const std::string& var() const {
        return *var_.get();
    }

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

protected:
    const Value& eval() override;

private:
    std::unique_ptr<std::string>                 var_;
};

/*
 * VersionedVariableExpression is designed for getting the historical results
 * of a variable.
 */
class VersionedVariableExpression final : public Expression {
public:
    VersionedVariableExpression(std::string* var, Expression* version)
        : Expression(Type::EXP_VERSIONED_VAR) {
        var_.reset(var);
        version_.reset(version);
    }

    void setExpCtxt(ExpressionContext* ctxt) override {
        expCtxt_ = ctxt;
        version_->setExpCtxt(ctxt);
    }

    const std::string& var() const {
        return *var_.get();
    }

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

protected:
    const Value& eval() override;

private:
    std::unique_ptr<std::string>                 var_;
    // 0 means the latest, -1 the previous one, and so on.
    // 1 means the eldest, 2 the second elder one, and so on.
    std::unique_ptr<Expression>                  version_;
};
}  // namespace nebula
#endif
