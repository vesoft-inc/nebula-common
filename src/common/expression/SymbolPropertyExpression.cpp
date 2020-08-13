/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/SymbolPropertyExpression.h"
#include "common/expression/ExprVisitor.h"

namespace nebula {


bool SymbolPropertyExpression::operator==(const Expression& rhs) const {
    if (kind_ != rhs.kind()) {
        return false;
    }

    const auto& r = dynamic_cast<const SymbolPropertyExpression&>(rhs);
    return *ref_ == *(r.ref_) && *sym_ == *(r.sym_) && *prop_ == *(r.prop_);
}


void SymbolPropertyExpression::writeTo(Encoder& encoder) const {
    // kind_
    encoder << kind_;

    // ref_
    encoder << ref_.get();

    // alias_
    encoder << sym_.get();

    // prop_
    encoder << prop_.get();
}


void SymbolPropertyExpression::resetFrom(Decoder& decoder) {
    // Read ref_
    ref_ = decoder.readStr();

    // Read alias_
    sym_ = decoder.readStr();

    // Read prop_
    prop_ = decoder.readStr();
}

const Value& SymbolPropertyExpression::eval(ExpressionContext& ctx) {
    // TODO maybe cypher need it.
    UNUSED(ctx);
    LOG(FATAL) << "Unimplemented";
}

void SymbolPropertyExpression::accept(ExprVisitor* visitor) {
    visitor->visitSymbolPropertyExpr(this);
}

const Value& EdgePropertyExpression::eval(ExpressionContext& ctx) {
    result_ = ctx.getEdgeProp(*sym_, *prop_);
    return result_;
}

void EdgePropertyExpression::accept(ExprVisitor *visitor) {
    visitor->visitEdgePropertyExpr(this);
}

const Value& TagPropertyExpression::eval(ExpressionContext& ctx) {
    result_ = ctx.getTagProp(*sym_, *prop_);
    return result_;
}

void TagPropertyExpression::accept(ExprVisitor* visitor) {
    visitor->visitTagPropertyExpr(this);
}

const Value& InputPropertyExpression::eval(ExpressionContext& ctx) {
    return ctx.getInputProp(*prop_);
}

void InputPropertyExpression::accept(ExprVisitor* visitor) {
    visitor->visitInputPropertyExpr(this);
}

const Value& VariablePropertyExpression::eval(ExpressionContext& ctx) {
    return ctx.getVarProp(*sym_, *prop_);
}

void VariablePropertyExpression::accept(ExprVisitor* visitor) {
    visitor->visitVariablePropertyExpr(this);
}

const Value& SourcePropertyExpression::eval(ExpressionContext& ctx) {
    result_ = ctx.getSrcProp(*sym_, *prop_);
    return result_;
}

void SourcePropertyExpression::accept(ExprVisitor* visitor) {
    visitor->visitSourcePropertyExpr(this);
}

const Value& DestPropertyExpression::eval(ExpressionContext& ctx) {
    return ctx.getDstProp(*sym_, *prop_);
}

void DestPropertyExpression::accept(ExprVisitor* visitor) {
    visitor->visitDestPropertyExpr(this);
}

const Value& EdgeSrcIdExpression::eval(ExpressionContext& ctx) {
    result_ = ctx.getEdgeProp(*sym_, *prop_);
    return result_;
}

void EdgeSrcIdExpression::accept(ExprVisitor* visitor) {
    visitor->visitEdgeSrcIdExpr(this);
}

const Value& EdgeTypeExpression::eval(ExpressionContext& ctx) {
    result_ = ctx.getEdgeProp(*sym_, *prop_);
    return result_;
}

void EdgeTypeExpression::accept(ExprVisitor* visitor) {
    visitor->visitEdgeTypeExpr(this);
}

const Value& EdgeRankExpression::eval(ExpressionContext& ctx) {
    result_ = ctx.getEdgeProp(*sym_, *prop_);
    return result_;
}

void EdgeRankExpression::accept(ExprVisitor* visitor) {
    visitor->visitEdgeRankExpr(this);
}

const Value& EdgeDstIdExpression::eval(ExpressionContext& ctx) {
    result_ = ctx.getEdgeProp(*sym_, *prop_);
    return result_;
}

void EdgeDstIdExpression::accept(ExprVisitor* visitor) {
    visitor->visitEdgeDstIdExpr(this);
}

std::string SymbolPropertyExpression::toString() const {
    std::string buf;
    buf.reserve(64);

    if (ref_ != nullptr && !ref_->empty()) {
        buf += *ref_;
        buf += ".";
    }

    if (sym_ != nullptr && !sym_->empty()) {
        buf += *sym_;
        buf += ".";
    }
    if (prop_ != nullptr) {
        buf += *prop_;
    }

    return buf;
}

std::string EdgePropertyExpression::toString() const {
    std::string buf;
    buf.reserve(64);

    if (sym_ != nullptr && !sym_->empty()) {
        buf += *sym_;
        buf += ".";
    }
    if (prop_ != nullptr) {
        buf += *prop_;
    }

    return buf;
}

std::string TagPropertyExpression::toString() const {
    std::string buf;
    buf.reserve(64);

    if (sym_ != nullptr && !sym_->empty()) {
        buf += *sym_;
        buf += ".";
    }
    if (prop_ != nullptr) {
        buf += *prop_;
    }

    return buf;
}

std::string InputPropertyExpression::toString() const {
    std::string buf;
    buf.reserve(64);

    if (ref_ != nullptr && !ref_->empty()) {
        buf += *ref_;
        buf += ".";
    }
    if (prop_ != nullptr) {
        buf += *prop_;
    }

    return buf;
}

std::string VariablePropertyExpression::toString() const {
    std::string buf;
    buf.reserve(64);

    if (ref_ != nullptr) {
        buf += *ref_;
    }
    if (sym_ != nullptr && !sym_->empty()) {
        buf += *sym_;
        buf += ".";
    }
    if (prop_ != nullptr) {
        buf += *prop_;
    }

    return buf;
}

std::string SourcePropertyExpression::toString() const {
    std::string buf;
    buf.reserve(64);

    if (ref_ != nullptr && !ref_->empty()) {
        buf += *ref_;
        buf += ".";
    }
    if (sym_ != nullptr && !sym_->empty()) {
        buf += *sym_;
        buf += ".";
    }
    if (prop_ != nullptr) {
        buf += *prop_;
    }

    return buf;
}

std::string DestPropertyExpression::toString() const {
    std::string buf;
    buf.reserve(64);

    if (ref_ != nullptr && !ref_->empty()) {
        buf += *ref_;
        buf += ".";
    }
    if (sym_ != nullptr && !sym_->empty()) {
        buf += *sym_;
        buf += ".";
    }
    if (prop_ != nullptr) {
        buf += *prop_;
    }

    return buf;
}

std::string EdgeSrcIdExpression::toString() const {
    std::string buf;
    buf.reserve(64);

    if (sym_ != nullptr && !sym_->empty()) {
        buf += *sym_;
        buf += ".";
    }
    if (prop_ != nullptr) {
        buf += *prop_;
    }

    return buf;
}

std::string EdgeTypeExpression::toString() const {
    std::string buf;
    buf.reserve(64);

    if (sym_ != nullptr && !sym_->empty()) {
        buf += *sym_;
        buf += ".";
    }
    if (prop_ != nullptr) {
        buf += *prop_;
    }

    return buf;
}

std::string EdgeRankExpression::toString() const {
    std::string buf;
    buf.reserve(64);

    if (sym_ != nullptr && !sym_->empty()) {
        buf += *sym_;
        buf += ".";
    }
    if (prop_ != nullptr) {
        buf += *prop_;
    }

    return buf;
}

std::string EdgeDstIdExpression::toString() const {
    std::string buf;
    buf.reserve(64);

    if (sym_ != nullptr && !sym_->empty()) {
        buf += *sym_;
        buf += ".";
    }
    if (prop_ != nullptr) {
        buf += *prop_;
    }

    return buf;
}

}  // namespace nebula
