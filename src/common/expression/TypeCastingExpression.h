/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_TYPECASTINGEXPRESSION_H_
#define COMMON_EXPRESSION_TYPECASTINGEXPRESSION_H_

#include "common/expression/Expression.h"

namespace nebula {
class TypeCastingExpression final : public Expression {
public:
    TypeCastingExpression(Value::Type vType, Expression* operand)
        : Expression(Kind::kTypeCasting), vType_(vType) {
        operand_.reset(operand);
    }

    void setEctx(ExpressionContext* ectx) override {
        ectx_ = ectx;
        operand_->setEctx(ectx);
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
    Value::Type                                 vType_;
    std::unique_ptr<Expression>                 operand_;
    Value                                       result_;
};
}   // namespace nebula
#endif
