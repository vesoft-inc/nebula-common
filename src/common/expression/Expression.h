/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_EXPRESSION_H_
#define COMMON_EXPRESSION_EXPRESSION_H_

#include "common/base/Base.h"
#include "common/datatypes/Value.h"
#include "common/context/ExpressionContext.h"

namespace nebula {

class Expression {
public:
    enum class Kind : uint8_t {
        kConstant,

        kAdd,
        kMinus,
        kMultiply,
        kDivision,
        kMod,

        kUnaryPlus,
        kUnaryNegate,
        kUnaryNot,

        kRelEQ,
        kRelNE,
        kRelLT,
        kRelLE,
        kRelGT,
        kRelGE,

        kLogicalAnd,
        kLogicalOr,
        kLogicalXor,

        kTypeCasting,

        kFunctionCall,

        kAliasProperty,
        kInputProperty,
        kVarProperty,
        kDstProperty,
        kSrcProperty,
        kEdgeSrc,
        kEdgeType,
        kEdgeRank,
        kEdgeDst,

        kUUID,
    };

    explicit Expression(Kind kind) : kind_(kind) {}

    virtual ~Expression() = default;

    Kind kind() const {
        return kind_;
    }

    virtual bool operator==(const Expression& rhs) const = 0;
    virtual bool operator!=(const Expression& rhs) const {
        return !operator==(rhs);
    }

    virtual Value eval(const ExpressionContext& ctx) const = 0;

    virtual std::string toString() const = 0;

    virtual size_t encode(std::string& buf) const = 0;

    // Reset the content of the expression from the given binary string.
    // It returns the position of the character right after the expression
    virtual void resetFrom(char*& ptr, const char* end) = 0;

    static std::unique_ptr<Expression> decode(char*& ptr, const char* end);

    static std::unique_ptr<Expression> decode(folly::StringPiece) {
        return nullptr;
    }

    bool isAliasPropertyExpression() const {
        return kind_ == Kind::kAliasProperty;
    }

protected:
    Kind kind_;
};

std::ostream& operator<<(std::ostream& os, Expression::Kind kind);

}   // namespace nebula
#endif   // COMMON_EXPRESSION_EXPRESSION_H_
