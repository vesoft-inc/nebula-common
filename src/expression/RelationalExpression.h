/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef EXPRESSION_RELATIONALEXPRESSION_H_
#define EXPRESSION_RELATIONALEXPRESSION_H_

#include "expression/Expression.h"

namespace nebula {
class RelationalExpression final : public Expression {
public:
    RelationalExpression(Type type,
                         Expression* lhs,
                         Expression* rhs)
        : Expression(type) {
        lhs_.reset(lhs);
        rhs_.reset(rhs);
    }

    void setExpCtxt(ExpressionContext* ctxt) override {
        expCtxt_ = ctxt;
        lhs_->setExpCtxt(ctxt);
        rhs_->setExpCtxt(ctxt);
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
    std::unique_ptr<Expression>                 lhs_;
    std::unique_ptr<Expression>                 rhs_;
    Value                                       result_;
};
}  // namespace nebula
#endif
