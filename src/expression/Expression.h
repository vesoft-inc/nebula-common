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
    }
    return os;
}

}  // namespace nebula
#endif  // EXPRESSION_EXPRESSION_H_
