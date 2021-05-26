/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_ATTRIBUTEEXPRESSION_H_
#define COMMON_EXPRESSION_ATTRIBUTEEXPRESSION_H_

#include "common/expression/BinaryExpression.h"

namespace nebula {

// <expr>.label
class AttributeExpression final : public BinaryExpression {
public:
    static AttributeExpression *make(ObjectPool *pool = nullptr,
                                     Expression *lhs = nullptr,
                                     Expression *rhs = nullptr) {
        return pool->add(new AttributeExpression(lhs, rhs));
    }
    explicit AttributeExpression(Expression *lhs = nullptr,
                                 Expression *rhs = nullptr)
        : BinaryExpression(Kind::kAttribute, lhs, rhs) {}

    const Value& eval(ExpressionContext &ctx) override;

    void accept(ExprVisitor *visitor) override;

    std::string toString() const override;

    std::unique_ptr<Expression> clone() const override {
        return std::make_unique<AttributeExpression>(left()->clone().release(),
                                                     right()->clone().release());
    }

private:
    Value                       result_;
};

}   // namespace nebula

#endif  // COMMON_EXPRESSION_ATTRIBUTEEXPRESSION_H_
