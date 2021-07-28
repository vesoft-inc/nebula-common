/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_EDGEEXPRESSION_H_
#define COMMON_EXPRESSION_EDGEEXPRESSION_H_

#include "common/expression/Expression.h"

namespace nebula {

/**
 * This is an internal type of expression to denote a Edge value.
 * It has no corresponding rules in parser.
 * It is generated from LabelExpression during semantic analysis
 * and expression rewrite.
 */
class EdgeExpression final : public Expression {
public:
    EdgeExpression& operator=(const EdgeExpression& rhs) = delete;
    EdgeExpression& operator=(EdgeExpression&&) = delete;

    static EdgeExpression* make(ObjectPool* pool) {
        return pool->add(new EdgeExpression(pool));
    }

    static EdgeExpression* make(ObjectPool* pool, const std::string& name) {
        return pool->add(new EdgeExpression(pool, name));
    }

    const Value& eval(ExpressionContext& ctx) override;

    void accept(ExprVisitor* visitor) override;

    Expression* clone() const override {
        return EdgeExpression::make(pool_);
    }

    std::string toString() const override {
        return outPutName_;
    }

    bool operator==(const Expression& expr) const override {
        return kind() == expr.kind();
    }

private:
    explicit EdgeExpression(ObjectPool* pool) : Expression(pool, Kind::kEdge) {
        outPutName_ = "EDGE";
    }

    EdgeExpression(ObjectPool* pool, const std::string& name) : Expression(pool, Kind::kEdge) {
        outPutName_ = name;
    }

    void writeTo(Encoder& encoder) const override {
        encoder << kind();
    }

    void resetFrom(Decoder&) override {}

private:
    Value                                   result_;
    std::string                             outPutName_;
};

}   // namespace nebula

#endif   // COMMON_EXPRESSION_EDGEEXPRESSION_H_
