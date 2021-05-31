/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_VERTEXEXPRESSION_H_
#define COMMON_EXPRESSION_VERTEXEXPRESSION_H_

#include "common/expression/LabelExpression.h"

namespace nebula {

/**
 * This is an internal type of expression to denote a Vertex value.
 * It has no corresponding rules in parser.
 * It is generated from LabelExpression during semantic analysis
 * and expression rewrite.
 */
class VertexExpression final : public LabelExpression {
public:
    explicit VertexExpression(std::string name) : LabelExpression(std::move(name)) {
        kind_ = Kind::kVertex;
    }

    const Value& eval(ExpressionContext &ctx) override;

    void accept(ExprVisitor *visitor) override;

    std::unique_ptr<Expression> clone() const override {
        return std::make_unique<VertexExpression>(name_);
    }

private:
    Value                                   result_;
};

}   // namespace nebula

#endif  // COMMON_EXPRESSION_VERTEXEXPRESSION_H_
