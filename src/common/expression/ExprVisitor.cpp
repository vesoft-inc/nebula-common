/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/ExprVisitor.h"

#include "common/base/Logging.h"
#include "common/expression/ArithmeticExpression.h"
#include "common/expression/ConstantExpression.h"
#include "common/expression/ContainerExpression.h"
#include "common/expression/FunctionCallExpression.h"
#include "common/expression/LabelExpression.h"
#include "common/expression/LogicalExpression.h"
#include "common/expression/RelationalExpression.h"
#include "common/expression/SubscriptExpression.h"
#include "common/expression/SymbolPropertyExpression.h"
#include "common/expression/TypeCastingExpression.h"
#include "common/expression/UUIDExpression.h"
#include "common/expression/UnaryExpression.h"
#include "common/expression/VariableExpression.h"

namespace nebula {

void ExprVisitor::visitConstantExpr(const ConstantExpression *expr) {
    DLOG(INFO) << "Visit constant expr: " << expr->toString();
}

void ExprVisitor::visitArithmeticExpr(const ArithmeticExpression *expr) {
    DLOG(INFO) << "Visit arithmetic expr: " << expr->toString();
    expr->left()->accept(this);
    expr->right()->accept(this);
}

void ExprVisitor::visitUnaryExpr(const UnaryExpression *expr) {
    DLOG(INFO) << "Visit unary expr: " << expr->toString();
    expr->operand()->accept(this);
}

void ExprVisitor::visitRelationalExpr(const RelationalExpression *expr) {
    DLOG(INFO) << "Visit relational expr: " << expr->toString();
    expr->left()->accept(this);
    expr->right()->accept(this);
}

void ExprVisitor::visitLogicalExpr(const LogicalExpression *expr) {
    DLOG(INFO) << "Visit logical expr: " << expr->toString();
    expr->left()->accept(this);
    expr->right()->accept(this);
}

void ExprVisitor::visitTypeCastingExpr(const TypeCastingExpression *expr) {
    DLOG(INFO) << "Visit type casting expr: " << expr->toString();
    expr->operand()->accept(this);
}

void ExprVisitor::visitFunctionCallExpr(const FunctionCallExpression *expr) {
    DLOG(INFO) << "Visit function call expr: " << expr->toString();
    for (const auto &arg : expr->args()->args()) {
        arg->accept(this);
    }
}

void ExprVisitor::visitEdgePropertyExpr(const EdgePropertyExpression *expr) {
    DLOG(INFO) << "Visit edge props expr: " << expr->toString();
}

void ExprVisitor::visitTagPropertyExpr(const TagPropertyExpression *expr) {
    DLOG(INFO) << "Visit tag props expr: " << expr->toString();
}

void ExprVisitor::visitInputPropertyExpr(const InputPropertyExpression *expr) {
    DLOG(INFO) << "Visit input props expr: " << expr->toString();
}

void ExprVisitor::visitVariablePropertyExpr(const VariablePropertyExpression *expr) {
    DLOG(INFO) << "Visit variable props expr: " << expr->toString();
}

void ExprVisitor::visitSourcePropertyExpr(const SourcePropertyExpression *expr) {
    DLOG(INFO) << "Visit source props expr: " << expr->toString();
}

void ExprVisitor::visitDestPropertyExpr(const DestPropertyExpression *expr) {
    DLOG(INFO) << "Visit dest props expr: " << expr->toString();
}

void ExprVisitor::visitEdgeSrcIdExpr(const EdgeSrcIdExpression *expr) {
    DLOG(INFO) << "Visit edge src id expr: " << expr->toString();
}

void ExprVisitor::visitEdgeTypeExpr(const EdgeTypeExpression *expr) {
    DLOG(INFO) << "Visit edge type expr: " << expr->toString();
}

void ExprVisitor::visitEdgeRankExpr(const EdgeRankExpression *expr) {
    DLOG(INFO) << "Visit edge rank expr: " << expr->toString();
}

void ExprVisitor::visitEdgeDstIdExpr(const EdgeDstIdExpression *expr) {
    DLOG(INFO) << "Visit edge dst id expr: " << expr->toString();
}

void ExprVisitor::visitUUIDExpr(const UUIDExpression *expr) {
    DLOG(INFO) << "Visit uuid expr: " << expr->toString();
}

void ExprVisitor::visitVariableExpr(const VariableExpression *expr) {
    DLOG(INFO) << "Visit variable expr: " << expr->toString();
}

void ExprVisitor::visitVersionedVariableExpr(const VersionedVariableExpression *expr) {
    DLOG(INFO) << "Visit version variable expr: " << expr->toString();
}

void ExprVisitor::visitListExpr(const ListExpression *expr) {
    DLOG(INFO) << "Visit list expr: " << expr->toString();
    for (const auto &item : expr->items()) {
        item->accept(this);
    }
}

void ExprVisitor::visitSetExpr(const SetExpression *expr) {
    DLOG(INFO) << "Visit set expr: " << expr->toString();
    for (const auto &item : expr->items()) {
        item->accept(this);
    }
}

void ExprVisitor::visitMapExpr(const MapExpression *expr) {
    DLOG(INFO) << "Visit map expr: " << expr->toString();
    for (const auto &pair : expr->items()) {
        pair.second->accept(this);
    }
}

void ExprVisitor::visitLabelExpr(const LabelExpression *expr) {
    DLOG(INFO) << "Visit label expr: " << expr->toString();
}

void ExprVisitor::visitSubscriptExpr(const SubscriptExpression *expr) {
    DLOG(INFO) << "Visit subscript expr: " << expr->toString();
    expr->left()->accept(this);
    expr->right()->accept(this);
}

void ExprVisitor::visitSymbolPropertyExpr(const SymbolPropertyExpression *expr) {
    DLOG(INFO) << "Visit symbol property expr: " << expr->toString();
}

}   // namespace nebula
