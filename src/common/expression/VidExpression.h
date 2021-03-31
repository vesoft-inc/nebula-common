/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_VIDEXPRESSION_H_
#define COMMON_EXPRESSION_VIDEXPRESSION_H_

#include "common/expression/Expression.h"

namespace nebula {
class VidExpression final : public Expression {
public:
    VidExpression() : Expression(Kind::kVidExpr) {}

    const Value& eval(ExpressionContext &ctx) override;

    void accept(ExprVisitor *visitor) override;

    std::unique_ptr<Expression> clone() const override {
        return std::make_unique<VidExpression>();
    }

    std::string toString() const override {
        return kVid;
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
};

}   // namespace nebula

#endif  // COMMON_EXPRESSION_VIDEXPRESSION_H_
