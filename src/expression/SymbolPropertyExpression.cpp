/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "expression/SymbolPropertyExpression.h"

namespace nebula {
Value SymbolPropertyExpression::eval() const {
    return Value(NullType::NaN);
}

Value EdgePropertyExpression::eval() const {
    return expCtxt_->getEdgeProp(*sym_, *prop_);
}

Value InputPropertyExpression::eval() const {
    // TODO
    return Value(NullType::NaN);
}

Value VariablePropertyExpression::eval() const {
    return expCtxt_->getVarProp(*sym_, *prop_);
}

Value SourcePropertyExpression::eval() const {
    return expCtxt_->getSrcProp(*sym_, *prop_);
}

Value DestPropertyExpression::eval() const {
    return expCtxt_->getDstProp(*sym_, *prop_);
}

Value EdgeSrcIdExpression::eval() const {
    return expCtxt_->getEdgeProp(*sym_, *prop_);
}

Value EdgeTypeExpression::eval() const {
    return expCtxt_->getEdgeProp(*sym_, *prop_);
}

Value EdgeRankExpression::eval() const {
    return expCtxt_->getEdgeProp(*sym_, *prop_);
}

Value EdgeDstIdExpression::eval() const {
    return expCtxt_->getEdgeProp(*sym_, *prop_);
}
}  // namespace nebula
