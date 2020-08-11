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

    virtual void visitConstantExpr(const ConstantExpression *expr);
    virtual void visitArithmeticExpr(const ArithmeticExpression *expr);
    virtual void visitUnaryExpr(const UnaryExpression *expr);
    virtual void visitRelationalExpr(const RelationalExpression *expr);
    virtual void visitLogicalExpr(const LogicalExpression *expr);
    virtual void visitTypeCastingExpr(const TypeCastingExpression *expr);
    virtual void visitFunctionCallExpr(const FunctionCallExpression *expr);
    virtual void visitEdgePropertyExpr(const EdgePropertyExpression *expr);
    virtual void visitTagPropertyExpr(const TagPropertyExpression *expr);
    virtual void visitInputPropertyExpr(const InputPropertyExpression *expr);
    virtual void visitVariablePropertyExpr(const VariablePropertyExpression *expr);
    virtual void visitSourcePropertyExpr(const SourcePropertyExpression *expr);
    virtual void visitDestPropertyExpr(const DestPropertyExpression *expr);
    virtual void visitEdgeSrcIdExpr(const EdgeSrcIdExpression *expr);
    virtual void visitEdgeTypeExpr(const EdgeTypeExpression *expr);
    virtual void visitEdgeRankExpr(const EdgeRankExpression *expr);
    virtual void visitEdgeDstIdExpr(const EdgeDstIdExpression *expr);
    virtual void visitUUIDExpr(const UUIDExpression *expr);
    virtual void visitVariableExpr(const VariableExpression *expr);
    virtual void visitVersionedVariableExpr(const VersionedVariableExpression *expr);
    virtual void visitListExpr(const ListExpression *expr);
    virtual void visitSetExpr(const SetExpression *expr);
    virtual void visitMapExpr(const MapExpression *expr);
    virtual void visitLabelExpr(const LabelExpression *expr);
    virtual void visitSubscriptExpr(const SubscriptExpression *expr);
    virtual void visitSymbolPropertyExpr(const SymbolPropertyExpression *expr);
};

}   // namespace nebula

#endif   // EXPRESSION_EXPRVISITOR_H_
