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

    virtual void visitConstantExpr(ConstantExpression *expr) = 0;
    virtual void visitArithmeticExpr(ArithmeticExpression *expr) = 0;
    virtual void visitUnaryExpr(UnaryExpression *expr) = 0;
    virtual void visitRelationalExpr(RelationalExpression *expr) = 0;
    virtual void visitLogicalExpr(LogicalExpression *expr) = 0;
    virtual void visitTypeCastingExpr(TypeCastingExpression *expr) = 0;
    virtual void visitFunctionCallExpr(FunctionCallExpression *expr) = 0;
    virtual void visitEdgePropertyExpr(EdgePropertyExpression *expr) = 0;
    virtual void visitTagPropertyExpr(TagPropertyExpression *expr) = 0;
    virtual void visitInputPropertyExpr(InputPropertyExpression *expr) = 0;
    virtual void visitVariablePropertyExpr(VariablePropertyExpression *expr) = 0;
    virtual void visitSourcePropertyExpr(SourcePropertyExpression *expr) = 0;
    virtual void visitDestPropertyExpr(DestPropertyExpression *expr) = 0;
    virtual void visitEdgeSrcIdExpr(EdgeSrcIdExpression *expr) = 0;
    virtual void visitEdgeTypeExpr(EdgeTypeExpression *expr) = 0;
    virtual void visitEdgeRankExpr(EdgeRankExpression *expr) = 0;
    virtual void visitEdgeDstIdExpr(EdgeDstIdExpression *expr) = 0;
    virtual void visitUUIDExpr(UUIDExpression *expr) = 0;
    virtual void visitVariableExpr(VariableExpression *expr) = 0;
    virtual void visitVersionedVariableExpr(VersionedVariableExpression *expr) = 0;
    virtual void visitListExpr(ListExpression *expr) = 0;
    virtual void visitSetExpr(SetExpression *expr) = 0;
    virtual void visitMapExpr(MapExpression *expr) = 0;
    virtual void visitLabelExpr(LabelExpression *expr) = 0;
    virtual void visitSubscriptExpr(SubscriptExpression *expr) = 0;
    virtual void visitSymbolPropertyExpr(SymbolPropertyExpression *expr) = 0;
};

}   // namespace nebula

#endif   // EXPRESSION_EXPRVISITOR_H_
