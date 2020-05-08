/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "expression/SymbolPropertyExpression.h"

namespace nebula {

const Value& EdgePropertyExpression::eval() {
    return expCtxt_->getEdgeProp(*sym_, *prop_);
}

const Value& InputPropertyExpression::eval() {
    // TODO
    return expCtxt_->getInputProp(*prop_);
}

const Value& VariablePropertyExpression::eval() {
    return expCtxt_->getVarProp(*sym_, *prop_);
}

const Value& SourcePropertyExpression::eval() {
    return expCtxt_->getSrcProp(*sym_, *prop_);
}

const Value& DestPropertyExpression::eval() {
    return expCtxt_->getDstProp(*sym_, *prop_);
}

const Value& EdgeSrcIdExpression::eval() {
    return expCtxt_->getEdgeProp(*sym_, *prop_);
}

const Value& EdgeTypeExpression::eval() {
    return expCtxt_->getEdgeProp(*sym_, *prop_);
}

const Value& EdgeRankExpression::eval() {
    return expCtxt_->getEdgeProp(*sym_, *prop_);
}

const Value& EdgeDstIdExpression::eval() {
    return expCtxt_->getEdgeProp(*sym_, *prop_);
}
}  // namespace nebula
