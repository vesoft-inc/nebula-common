/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef EXPRESSION_TYPECASTINGEXPRESSION_H_
#define EXPRESSION_TYPECASTINGEXPRESSION_H_

#include "expression/Expression.h"

namespace nebula {
class TypeCastingExpression final : public Expression {
public:
    TypeCastingExpression(Type type,
                          Value::Type vType,
                          std::unique_ptr<Expression> operand)
        : Expression(type), vType_(vType), operand_(std::move(operand)) {}

    Value eval() const override;

    std::string encode() const override {
        // TODO
        return "";
    }

private:
    Value::Type                                 vType_;
    std::unique_ptr<Expression>                 operand_;
};
}  // namespace nebula
#endif
