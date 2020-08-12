/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef EXPRESSION_EXPRVISITOR_H_
#define EXPRESSION_EXPRVISITOR_H_

namespace nebula {

class ArithmeticExpression;
class ConstantExpression;
class DestPropertyExpression;
class EdgeDstIdExpression;
class EdgePropertyExpression;
class EdgeRankExpression;
class EdgeSrcIdExpression;
class EdgeTypeExpression;
class FunctionCallExpression;
class InputPropertyExpression;
class LabelExpression;
class ListExpression;
class LogicalExpression;
class MapExpression;
class RelationalExpression;
class SetExpression;
class SourcePropertyExpression;
class SubscriptExpression;
class SymbolPropertyExpression;
class TagPropertyExpression;
class TypeCastingExpression;
class UnaryExpression;
class UUIDExpression;
class VariableExpression;
class VariablePropertyExpression;
class VersionedVariableExpression;

class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;

    virtual void visitConstantExpr(const ConstantExpression *expr) = 0;
    virtual void visitArithmeticExpr(const ArithmeticExpression *expr) = 0;
    virtual void visitUnaryExpr(const UnaryExpression *expr) = 0;
    virtual void visitRelationalExpr(const RelationalExpression *expr) = 0;
    virtual void visitLogicalExpr(const LogicalExpression *expr) = 0;
    virtual void visitTypeCastingExpr(const TypeCastingExpression *expr) = 0;
    virtual void visitFunctionCallExpr(const FunctionCallExpression *expr) = 0;
    virtual void visitEdgePropertyExpr(const EdgePropertyExpression *expr) = 0;
    virtual void visitTagPropertyExpr(const TagPropertyExpression *expr) = 0;
    virtual void visitInputPropertyExpr(const InputPropertyExpression *expr) = 0;
    virtual void visitVariablePropertyExpr(const VariablePropertyExpression *expr) = 0;
    virtual void visitSourcePropertyExpr(const SourcePropertyExpression *expr) = 0;
    virtual void visitDestPropertyExpr(const DestPropertyExpression *expr) = 0;
    virtual void visitEdgeSrcIdExpr(const EdgeSrcIdExpression *expr) = 0;
    virtual void visitEdgeTypeExpr(const EdgeTypeExpression *expr) = 0;
    virtual void visitEdgeRankExpr(const EdgeRankExpression *expr) = 0;
    virtual void visitEdgeDstIdExpr(const EdgeDstIdExpression *expr) = 0;
    virtual void visitUUIDExpr(const UUIDExpression *expr) = 0;
    virtual void visitVariableExpr(const VariableExpression *expr) = 0;
    virtual void visitVersionedVariableExpr(const VersionedVariableExpression *expr) = 0;
    virtual void visitListExpr(const ListExpression *expr) = 0;
    virtual void visitSetExpr(const SetExpression *expr) = 0;
    virtual void visitMapExpr(const MapExpression *expr) = 0;
    virtual void visitLabelExpr(const LabelExpression *expr) = 0;
    virtual void visitSubscriptExpr(const SubscriptExpression *expr) = 0;
    virtual void visitSymbolPropertyExpr(const SymbolPropertyExpression *expr) = 0;
};

}   // namespace nebula

#endif   // EXPRESSION_EXPRVISITOR_H_
