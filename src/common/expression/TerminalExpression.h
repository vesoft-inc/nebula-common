/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_TERMINALEXPRESSION_H_
#define COMMON_EXPRESSION_TERMINALEXPRESSION_H_

#include "common/expression/Expression.h"

namespace nebula {

/**
 *  Base class for terminal expression which not reference any other expression
 **/
class TerminalExpression : public Expression {
    friend class Expression;

public:
    explicit TerminalExpression(Kind kind) : Expression(kind) {}

protected:
    bool traversal(Visitor visitor) const override {
        return visitor(this);
    }
};

}  // namespace nebula
#endif  // COMMON_EXPRESSION_TERMINALEXPRESSION_H_

