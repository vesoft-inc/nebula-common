/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_BINARYEXPRESSION_H_
#define COMMON_EXPRESSION_BINARYEXPRESSION_H_

#include "common/expression/Expression.h"
#include "common/jit/JITUtils.h"

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

protected:
    using BinOP = std::function<llvm::Value*(llvm::Value* lhs, llvm::Value* rhs)>;

    llvm::Value* createBinOp(llvm::IRBuilder<>* builder,
                             llvm::Value* lhs,
                             llvm::Value* rhs,
                             BinOP opForInt,
                             BinOP opForDouble) const;

    void writeTo(Encoder& encoder) const override;

    void resetFrom(Decoder& decoder) override;

    std::unique_ptr<Expression>                 lhs_;
    std::unique_ptr<Expression>                 rhs_;
};

}  // namespace nebula
#endif  // COMMON_EXPRESSION_BINARYEXPRESSION_H_

