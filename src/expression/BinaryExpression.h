/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef EXPRESSION_BINARYEXPRESSION_H_
#define EXPRESSION_BINARYEXPRESSION_H_

#include "expression/Expression.h"

namespace nebula {

/**
 *  Base class for all binary expressions
 **/
class BinaryExpression : public Expression {
public:
    BinaryExpression(Kind kind,
                     std::unique_ptr<Expression>&& lhs,
                     std::unique_ptr<Expression>&& rhs)
        : Expression(kind)
        , lhs_(std::move(lhs))
        , rhs_(std::move(rhs)) {}

    size_t encode(std::string& buf) const override;

    bool operator==(const Expression& rhs) const override;

    void resetFrom(char*& ptr, const char* end) override;

protected:
    std::unique_ptr<Expression>                 lhs_;
    std::unique_ptr<Expression>                 rhs_;
};

}  // namespace nebula
#endif  // EXPRESSION_BINARYEXPRESSION_H_

