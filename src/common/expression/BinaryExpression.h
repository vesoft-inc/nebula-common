/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_BINARYEXPRESSION_H_
#define COMMON_EXPRESSION_BINARYEXPRESSION_H_

#include "common/expression/Expression.h"

namespace nebula {

/**
 *  Base class for all binary expressions
 **/
class BinaryExpression : public Expression {
    friend class Expression;

public:
    BinaryExpression(Kind kind,
                     Expression* lhs,
                     Expression* rhs)
        : Expression(kind), lhs_(lhs), rhs_(rhs) {}

    bool operator==(const Expression& rhs) const override;

    const Expression* left() const {
        return lhs_.get();
    }

    const Expression* right() const {
        return rhs_.get();
    }

protected:
    void writeTo(Encoder& encoder) const override;

    void resetFrom(Decoder& decoder) override;

    bool traverse(Visitor visitor) const override {
        if (!visitor(this)) {
            return false;
        }
        if (!lhs_->traverse(visitor)) {
            return false;
        }
        return rhs_->traverse(visitor);
    }

    std::unique_ptr<Expression>                 lhs_;
    std::unique_ptr<Expression>                 rhs_;
};

}  // namespace nebula
#endif  // COMMON_EXPRESSION_BINARYEXPRESSION_H_

