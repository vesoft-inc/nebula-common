/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef EXPRESSION_UNARYEXPRESSION_H_
#define EXPRESSION_UNARYEXPRESSION_H_

#include "expression/Expression.h"

namespace nebula {
class UnaryExpression final : public Expression {
public:
    UnaryExpression(Type type, std::unique_ptr<Expression> operand)
        : Expression(type), operand_(std::move(operand)) {}

    Value eval() const override;

    std::string encode() const override {
        return "";
    }
private:
    std::unique_ptr<Expression>                 operand_;
};
}  // namespace nebula
#endif
