/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_ARITHMETICEXPRESSION_H_
#define COMMON_EXPRESSION_ARITHMETICEXPRESSION_H_

#include "common/expression/Expression.h"

namespace nebula {

class ArithmeticExpression : public Expression {
public:
    ArithmeticExpression(Kind kind, Expression* lhs, Expression* rhs) : Expression(kind) {
        lhs_.reset(lhs);
        rhs_.reset(rhs);
    }

    void setEctx(ExpressionContext* ectx) override {
        ectx_ = ectx;
        lhs_->setEctx(ectx);
        rhs_->setEctx(ectx);
    }

    const Value& eval() override;

    std::string encode() const override;

    std::string decode() const override {
        // TODO
        return "";
    }

    std::string toString() const override {
        // TODO
        return "";
    }

private:
    std::unique_ptr<Expression> lhs_;
    std::unique_ptr<Expression> rhs_;
    Value                       result_;
};

}   // namespace nebula
#endif   // COMMON_EXPRESSION_EXPRESSION_H_
