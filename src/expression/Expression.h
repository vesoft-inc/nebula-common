/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef EXPRESSION_EXPRESSION_H_
#define EXPRESSION_EXPRESSION_H_

#include "base/Base.h"
#include "datatypes/Value.h"

namespace nebula {

class Expression {
public:
    enum class Type {
        EXP_CONSTANT,

        EXP_ADD,
        EXP_MINUS,
        EXP_MULTIPLY,
        EXP_DIVIDE,

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
    };

    explicit Expression(Type type) : type_(type) {}

    Type type() const {
        return type_;
    }

    virtual ~Expression() = default;

    virtual Value eval() const = 0;

    virtual std::string encode() const = 0;

protected:
    Type type_;
};


std::ostream& operator<<(std::ostream& os, Expression::Type type) {
    switch (type) {
        case Expression::Type::EXP_CONSTANT:
            os << "Constant";
            break;
        case Expression::Type::EXP_ADD:
            os << "Add";
            break;
        case Expression::Type::EXP_MINUS:
            os << "Minus";
            break;
        case Expression::Type::EXP_MULTIPLY:
            os << "Multiply";
            break;
        case Expression::Type::EXP_DIVIDE:
            os << "Divide";
            break;
        case Expression::Type::EXP_UNARY_PLUS:
            os << "UnaryPlus";
            break;
        case Expression::Type::EXP_UNARY_NEGATE:
            os << "UnaryNegate";
            break;
        case Expression::Type::EXP_UNARY_NOT:
            os << "UnaryNot";
            break;
        case Expression::Type::EXP_REL_EQ:
            os << "Equal";
            break;
        case Expression::Type::EXP_REL_NE:
            os << "NotEuqal";
            break;
        case Expression::Type::EXP_REL_LT:
            os << "LessThan";
            break;
        case Expression::Type::EXP_REL_LE:
            os << "LessEqual";
            break;
        case Expression::Type::EXP_REL_GT:
            os << "GreaterThan";
            break;
        case Expression::Type::EXP_REL_GE:
            os << "GreaterEqual";
            break;
        case Expression::Type::EXP_LOGICAL_AND:
            os << "LogicalAnd";
            break;
        case Expression::Type::EXP_LOGICAL_OR:
            os << "LogicalOr";
            break;
        case Expression::Type::EXP_LOGICAL_XOR:
            os << "LogicalXor";
            break;
        case Expression::Type::EXP_TYPE_CASTING:
            os << "TypeCasting";
            break;
        case Expression::Type::EXP_FUNCTION_CALL:
            os << "FunctionCall";
            break;
    }
    return os;
}

}  // namespace nebula
#endif  // EXPRESSION_EXPRESSION_H_
