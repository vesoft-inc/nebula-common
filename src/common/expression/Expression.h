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
    enum class Kind {
        kConstant,

        kAdd,
        kMinus,
        kMultiply,
        kDivision,
        kMod,

        kUnaryPlus,
        kUnaryNegate,
        kUnaryNot,
        kUnaryIncr,
        kUnaryDecr,

        kRelEQ,
        kRelNE,
        kRelLT,
        kRelLE,
        kRelGT,
        kRelGE,
        kRelIn,

        kLogicalAnd,
        kLogicalOr,
        kLogicalXor,

        kTypeCasting,

        kFunctionCall,

        kSymProperty,
        kEdgeProperty,
        kInputProperty,
        kVarProperty,
        kDstProperty,
        kSrcProperty,
        kEdgeSrc,
        kEdgeType,
        kEdgeRank,
        kEdgeDst,

        kUUID,

        kVar,
        kVersionedVar,
    };

    explicit Expression(Kind kind) : kind_(kind) {}

    virtual ~Expression() = default;

    Kind kind() const {
        return kind_;
    }

    static Value eval(Expression* expr) {
        return expr->eval();
    }

    virtual const Value& eval() = 0;

    virtual void setEctx(ExpressionContext* ectx) = 0;

    virtual std::string toString() const = 0;

    virtual std::string encode() const = 0;

    virtual std::string decode() const = 0;

    static std::unique_ptr<Expression> decode(folly::StringPiece) {
        return nullptr;
    }

protected:
    Kind                kind_;
    ExpressionContext*  ectx_;
};

std::ostream& operator<<(std::ostream& os, Expression::Kind kind);
}   // namespace nebula
#endif   // COMMON_EXPRESSION_EXPRESSION_H_
