/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_PATHBUILDEXPRESSION_H_
#define COMMON_EXPRESSION_PATHBUILDEXPRESSION_H_

#include "common/expression/Expression.h"

namespace nebula {
class PathBuildExpression final : public Expression {
public:
    PathBuildExpression() : Expression(Kind::kPathBuild) {
    }

    const Value& eval(ExpressionContext& ctx) override;

    bool operator==(const Expression &rhs) const override;

    std::string toString() const override;

    void accept(ExprVisitor* visitor) override;

    std::unique_ptr<Expression> clone() const override;

    PathBuildExpression& add(std::unique_ptr<Expression> expr) {
        items_.emplace_back(std::move(expr));
        return *this;
    }

private:
    void writeTo(Encoder &encoder) const override;

    void resetFrom(Decoder &decoder) override;

private:
    std::vector<std::unique_ptr<Expression>>    items_;
    Value                                       result_;
};
}  // namespace nebula
#endif  // COMMON_EXPRESSION_PATHBUILDEXPRESSION_H_
