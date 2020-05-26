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
    TypeCastingExpression(Value::Type vType = Value::Type::__EMPTY__,
                          std::unique_ptr<Expression>&& operand = nullptr)
        : Expression(Kind::kTypeCasting)
        , vType_(vType)
        , operand_(std::move(operand)) {}

    bool operator==(const Expression& rhs) const override;

    Value eval(const ExpressionContext& ctx) const override;

    size_t encode(std::string& buf) const override;

    void resetFrom(char*& ptr, const char* end) override;

    std::string toString() const override {
        // TODO
        return "";
    }

protected:
    Value::Type                 vType_;
    std::unique_ptr<Expression> operand_;
};

}  // namespace nebula
#endif  // EXPRESSION_TYPECASTINGEXPRESSION_H_
