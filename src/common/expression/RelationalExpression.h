/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_RELATIONALEXPRESSION_H_
#define COMMON_EXPRESSION_RELATIONALEXPRESSION_H_

#include "common/expression/BinaryExpression.h"

namespace nebula {
class RelationalExpression final : public BinaryExpression {
public:
    static RelationalExpression* makeRelEQ(ObjectPool* pool = nullptr,
                                           Expression* lhs = nullptr,
                                           Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new RelationalExpression(Kind::kRelEQ, lhs, rhs));
    }

    static RelationalExpression* makeRelNE(ObjectPool* pool = nullptr,
                                           Expression* lhs = nullptr,
                                           Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new RelationalExpression(Kind::kRelNE, lhs, rhs));
    }

    static RelationalExpression* makeRelLT(ObjectPool* pool = nullptr,
                                           Expression* lhs = nullptr,
                                           Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new RelationalExpression(Kind::kRelLT, lhs, rhs));
    }

    static RelationalExpression* makeRelLE(ObjectPool* pool = nullptr,
                                           Expression* lhs = nullptr,
                                           Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new RelationalExpression(Kind::kRelLE, lhs, rhs));
    }

    static RelationalExpression* makeRelGT(ObjectPool* pool = nullptr,
                                           Expression* lhs = nullptr,
                                           Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new RelationalExpression(Kind::kRelGT, lhs, rhs));
    }

    static RelationalExpression* makeRelGE(ObjectPool* pool = nullptr,
                                           Expression* lhs = nullptr,
                                           Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new RelationalExpression(Kind::kRelGE, lhs, rhs));
    }

    static RelationalExpression* makeRelREG(ObjectPool* pool = nullptr,
                                            Expression* lhs = nullptr,
                                            Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new RelationalExpression(Kind::kRelREG, lhs, rhs));
    }

    static RelationalExpression* makeRelIn(ObjectPool* pool = nullptr,
                                           Expression* lhs = nullptr,
                                           Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new RelationalExpression(Kind::kRelIn, lhs, rhs));
    }

    static RelationalExpression* makeRelNotIn(ObjectPool* pool = nullptr,
                                              Expression* lhs = nullptr,
                                              Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new RelationalExpression(Kind::kRelNotIn, lhs, rhs));
    }

    static RelationalExpression* makeContains(ObjectPool* pool = nullptr,
                                              Expression* lhs = nullptr,
                                              Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new RelationalExpression(Kind::kContains, lhs, rhs));
    }

    static RelationalExpression* makeNotContains(ObjectPool* pool = nullptr,
                                                 Expression* lhs = nullptr,
                                                 Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new RelationalExpression(Kind::kNotContains, lhs, rhs));
    }

    static RelationalExpression* makeStartsWith(ObjectPool* pool = nullptr,
                                                Expression* lhs = nullptr,
                                                Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new RelationalExpression(Kind::kStartsWith, lhs, rhs));
    }

    static RelationalExpression* makeNotStartsWith(ObjectPool* pool = nullptr,
                                                   Expression* lhs = nullptr,
                                                   Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new RelationalExpression(Kind::kNotStartsWith, lhs, rhs));
    }

    static RelationalExpression* makeEndsWith(ObjectPool* pool = nullptr,
                                              Expression* lhs = nullptr,
                                              Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new RelationalExpression(Kind::kEndsWith, lhs, rhs));
    }

    static RelationalExpression* makeNotEndsWith(ObjectPool* pool = nullptr,
                                                 Expression* lhs = nullptr,
                                                 Expression* rhs = nullptr) {
        DCHECK(!!pool);
        return pool->add(new RelationalExpression(Kind::kNotEndsWith, lhs, rhs));
    }

    explicit RelationalExpression(Kind kind, Expression* lhs = nullptr, Expression* rhs = nullptr)
        : BinaryExpression(kind, lhs, rhs) {}

    const Value& eval(ExpressionContext& ctx) override;

    std::string toString() const override;

    void accept(ExprVisitor* visitor) override;

    std::unique_ptr<Expression> clone() const override {
        return std::make_unique<RelationalExpression>(
            kind(), left()->clone().release(), right()->clone().release());
    }

    bool isRelExpr() const override {
        return true;
    }

private:
    Value                                       result_;
};

}  // namespace nebula
#endif  // COMMON_EXPRESSION_RELATIONALEXPRESSION_H_
