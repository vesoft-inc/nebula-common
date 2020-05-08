/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef EXPRESSION_EXPRESSION_H_
#define EXPRESSION_EXPRESSION_H_

#include "base/Base.h"
#include "datatypes/Value.h"
#include "context/ExpressionContext.h"

namespace nebula {

class Expression {
public:
    enum class Type {
        EXP_CONSTANT,

        EXP_ADD,
        EXP_MINUS,
        EXP_MULTIPLY,
        EXP_DIVIDE,
        EXP_MOD,

        EXP_UNARY_PLUS,
        EXP_UNARY_NEGATE,
        EXP_UNARY_NOT,

        EXP_REL_EQ,
        EXP_REL_NE,
        EXP_REL_LT,
        EXP_REL_LE,
        EXP_REL_GT,
        EXP_REL_GE,

        EXP_LOGICAL_AND,
        EXP_LOGICAL_OR,
        EXP_LOGICAL_XOR,

        EXP_TYPE_CASTING,

        EXP_FUNCTION_CALL,

        EXP_SYMBOL_PROPERTY,
        EXP_EDGE_PROPERTY,
        EXP_INPUT_PROPERTY,
        EXP_VAR_PROPERTY,
        EXP_DST_PROPERTY,
        EXP_SRC_PROPERTY,
        EXP_EDGE_SRC,
        EXP_EDGE_TYPE,
        EXP_EDGE_RANK,
        EXP_EDGE_DST,

        EXP_UUID,

        EXP_VAR,
    };

    explicit Expression(Type type) : type_(type) {}

    virtual ~Expression() = default;

    static Value eval(Expression* expr) {
        return expr->eval();
    }

    Type type() const {
        return type_;
    }

    virtual void setExpCtxt(ExpressionContext* ctxt) = 0;

    virtual std::string toString() const = 0;

    virtual std::string encode() const = 0;

    virtual std::string decode() const = 0;

protected:
    friend class ArithmeticExpression;
    friend class ConstantExpression;
    friend class FunctionCallExpression;
    friend class LogicalExpression;
    friend class RelationalExpression;
    friend class TypeCastingExpression;
    friend class UnaryExpression;
    virtual const Value& eval() = 0;

protected:
    Type                type_;
    ExpressionContext*  expCtxt_;
};


std::ostream& operator<<(std::ostream& os, Expression::Type type);
}  // namespace nebula
#endif  // EXPRESSION_EXPRESSION_H_
