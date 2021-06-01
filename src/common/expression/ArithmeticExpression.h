/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_ARITHMETICEXPRESSION_H_
#define COMMON_EXPRESSION_ARITHMETICEXPRESSION_H_

#include "common/expression/BinaryExpression.h"

namespace nebula {

class ArithmeticExpression final : public BinaryExpression {
public:
    static ArithmeticExpression* makeAdd(ObjectPool* pool = nullptr,
                                         Expression* lhs = nullptr,
                                         Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new ArithmeticExpression(pool, Expression::Kind::kAdd, lhs, rhs));
    }
    static ArithmeticExpression* makeMinus(ObjectPool* pool = nullptr,
                                           Expression* lhs = nullptr,
                                           Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new ArithmeticExpression(pool, Expression::Kind::kMinus, lhs, rhs));
    }
    static ArithmeticExpression* makeMultiply(ObjectPool* pool = nullptr,
                                              Expression* lhs = nullptr,
                                              Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new ArithmeticExpression(pool, Expression::Kind::kMultiply, lhs, rhs));
    }
    static ArithmeticExpression* makeDivision(ObjectPool* pool = nullptr,
                                              Expression* lhs = nullptr,
                                              Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new ArithmeticExpression(pool, Expression::Kind::kDivision, lhs, rhs));
    }
    static ArithmeticExpression* makeMod(ObjectPool* pool = nullptr,
                                         Expression* lhs = nullptr,
                                         Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new ArithmeticExpression(pool, Expression::Kind::kMod, lhs, rhs));
    }

    explicit ArithmeticExpression(ObjectPool* pool, Kind kind, Expression* lhs, Expression* rhs)
        : BinaryExpression(pool, kind, lhs, rhs) {}

    const Value& eval(ExpressionContext& ctx) override;

    void accept(ExprVisitor* visitor) override;

    std::string toString() const override;

    Expression* clone() const override {
        return pool_->add(new ArithmeticExpression(pool_,
            kind(), left()->clone(), right()->clone()));
    }

    bool isArithmeticExpr() const override {
        return true;
    }

private:
    Value result_;
};

}   // namespace nebula
#endif   // COMMON_EXPRESSION_EXPRESSION_H_
