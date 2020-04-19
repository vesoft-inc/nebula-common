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
                         std::unique_ptr<Expression> lhs,
                         std::unique_ptr<Expression> rhs)
        : Expression(type), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

    Value eval() const override;

    std::string encode() const override {
        return "";
    }

private:
    std::unique_ptr<Expression>                 lhs_;
    std::unique_ptr<Expression>                 rhs_;
};
}  // namespace nebula
#endif
