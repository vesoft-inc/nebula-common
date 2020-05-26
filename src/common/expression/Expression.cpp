/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/Expression.h"
#include "common/expression/AliasPropertyExpression.h"
#include "common/expression/ArithmeticExpression.h"
#include "common/expression/ConstantExpression.h"
#include "common/expression/FunctionCallExpression.h"
#include "common/expression/LogicalExpression.h"
#include "common/expression/RelationalExpression.h"
#include "common/expression/TypeCastingExpression.h"
#include "common/expression/UUIDExpression.h"
#include "common/expression/UnaryExpression.h"

namespace nebula {

// static
std::unique_ptr<Expression> Expression::decode(char*& ptr, const char* end) {
    if (ptr >= end) {
        return nullptr;
    }

    Expression::Kind kind;
    memcpy(reinterpret_cast<void*>(&kind), ptr, sizeof(uint8_t));
    ptr += sizeof(uint8_t);
    switch (kind) {
        case Expression::Kind::kConstant: {
            auto exp = std::make_unique<ConstantExpression>();
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kAdd: {
            auto exp = std::make_unique<ArithmeticExpression>(Expression::Kind::kAdd);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kMinus: {
            auto exp = std::make_unique<ArithmeticExpression>(Expression::Kind::kMinus);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kMultiply: {
            auto exp = std::make_unique<ArithmeticExpression>(Expression::Kind::kMultiply);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kDivision: {
            auto exp = std::make_unique<ArithmeticExpression>(Expression::Kind::kDivision);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kMod: {
            auto exp = std::make_unique<ArithmeticExpression>(Expression::Kind::kMod);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kUnaryPlus: {
            auto exp = std::make_unique<UnaryExpression>(Expression::Kind::kUnaryPlus);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kUnaryNegate: {
            auto exp = std::make_unique<UnaryExpression>(Expression::Kind::kUnaryNegate);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kUnaryNot: {
            auto exp = std::make_unique<UnaryExpression>(Expression::Kind::kUnaryNot);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kRelEQ: {
            auto exp = std::make_unique<RelationalExpression>(Expression::Kind::kRelEQ);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kRelNE: {
            auto exp = std::make_unique<RelationalExpression>(Expression::Kind::kRelNE);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kRelLT: {
            auto exp = std::make_unique<RelationalExpression>(Expression::Kind::kRelLT);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kRelLE: {
            auto exp = std::make_unique<RelationalExpression>(Expression::Kind::kRelLE);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kRelGT: {
            auto exp = std::make_unique<RelationalExpression>(Expression::Kind::kRelGT);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kRelGE: {
            auto exp = std::make_unique<RelationalExpression>(Expression::Kind::kRelGE);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kLogicalAnd: {
            auto exp = std::make_unique<LogicalExpression>(Expression::Kind::kLogicalAnd);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kLogicalOr: {
            auto exp = std::make_unique<LogicalExpression>(Expression::Kind::kLogicalOr);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kLogicalXor: {
            auto exp = std::make_unique<LogicalExpression>(Expression::Kind::kLogicalXor);
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kTypeCasting: {
            auto exp = std::make_unique<TypeCastingExpression>();
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kFunctionCall: {
            auto exp = std::make_unique<FunctionCallExpression>();
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kAliasProperty: {
            return nullptr;
        }
        case Expression::Kind::kInputProperty: {
            auto exp = std::make_unique<InputPropertyExpression>();
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kVarProperty: {
            auto exp = std::make_unique<VariablePropertyExpression>();
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kDstProperty: {
            auto exp = std::make_unique<DestPropertyExpression>();
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kSrcProperty: {
            auto exp = std::make_unique<SourcePropertyExpression>();
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kEdgeSrc: {
            auto exp = std::make_unique<EdgeSrcIdExpression>();
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kEdgeType: {
            auto exp = std::make_unique<EdgeTypeExpression>();
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kEdgeRank: {
            auto exp = std::make_unique<EdgeRankExpression>();
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kEdgeDst: {
            auto exp = std::make_unique<EdgeDstIdExpression>();
            exp->resetFrom(ptr, end);
            return exp;
        }
        case Expression::Kind::kUUID: {
            auto exp = std::make_unique<UUIDExpression>();
            exp->resetFrom(ptr, end);
            return exp;
        }
        default: {
            break;
        }
    }

    LOG(FATAL) << "Unknown expression: "
               << toHexStr(folly::StringPiece(ptr - sizeof(uint8_t), end));
}


std::ostream& operator<<(std::ostream& os, Expression::Kind kind) {
    switch (kind) {
        case Expression::Kind::kConstant:
            os << "Constant";
            break;
        case Expression::Kind::kAdd:
            os << "Add";
            break;
        case Expression::Kind::kMinus:
            os << "Minus";
            break;
        case Expression::Kind::kMultiply:
            os << "Multiply";
            break;
        case Expression::Kind::kDivision:
            os << "Division";
            break;
        case Expression::Kind::kMod:
            os << "Mod";
            break;
        case Expression::Kind::kUnaryPlus:
            os << "UnaryPlus";
            break;
        case Expression::Kind::kUnaryNegate:
            os << "UnaryNegate";
            break;
        case Expression::Kind::kUnaryNot:
            os << "UnaryNot";
            break;
        case Expression::Kind::kRelEQ:
            os << "Equal";
            break;
        case Expression::Kind::kRelNE:
            os << "NotEuqal";
            break;
        case Expression::Kind::kRelLT:
            os << "LessThan";
            break;
        case Expression::Kind::kRelLE:
            os << "LessEqual";
            break;
        case Expression::Kind::kRelGT:
            os << "GreaterThan";
            break;
        case Expression::Kind::kRelGE:
            os << "GreaterEqual";
            break;
        case Expression::Kind::kLogicalAnd:
            os << "LogicalAnd";
            break;
        case Expression::Kind::kLogicalOr:
            os << "LogicalOr";
            break;
        case Expression::Kind::kLogicalXor:
            os << "LogicalXor";
            break;
        case Expression::Kind::kTypeCasting:
            os << "TypeCasting";
            break;
        case Expression::Kind::kFunctionCall:
            os << "FunctionCall";
            break;
        case Expression::Kind::kAliasProperty:
            os << "AliasProp";
            break;
        case Expression::Kind::kInputProperty:
            os << "InputProp";
            break;
        case Expression::Kind::kVarProperty:
            os << "VarProp";
            break;
        case Expression::Kind::kDstProperty:
            os << "DstProp";
            break;
        case Expression::Kind::kSrcProperty:
            os << "SrcProp";
            break;
        case Expression::Kind::kEdgeSrc:
            os << "EdgeSrc";
            break;
        case Expression::Kind::kEdgeType:
            os << "EdgeType";
            break;
        case Expression::Kind::kEdgeRank:
            os << "EdgeRank";
            break;
        case Expression::Kind::kEdgeDst:
            os << "EdgeDst";
            break;
        case Expression::Kind::kUUID:
            os << "UUID";
            break;
    }
    return os;
}

}  // namespace nebula
