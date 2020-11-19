/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_COLUMNEXPRESSION_H_
#define COMMON_EXPRESSION_COLUMNEXPRESSION_H_

#include "common/expression/Expression.h"

namespace nebula {
/**
 * This is an internal type of expression
 * It has no corresponding rules in parser.
 * you can get the corresponding value by column number
 */
class ColumnExpression final : public Expression {
public:
    explicit ColumnExpression(size_t index = 0) : Expression(Kind::kColumn), index_(index) {}

    const Value& eval(ExpressionContext &ctx) override;

    void accept(ExprVisitor *visitor) override;

    std::unique_ptr<Expression> clone() const override {
        return std::make_unique<ColumnExpression>(index_);
    }

    std::string toString() const override {
        return "COLUMN";
    }

    bool operator==(const Expression &expr) const override {
        return kind() == expr.kind();
    }

private:
    void writeTo(Encoder &encoder) const override {
        encoder << kind();
    }

    void resetFrom(Decoder&) override {}

private:
    Value                                   result_;
    size_t                                  index_;
};

}   // namespace nebula

#endif  // COMMON_EXPRESSION_COLUMNEXPRESSION_H_
