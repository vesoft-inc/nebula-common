/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "expression/SymbolPropertyExpression.h"

namespace nebula {

const Value& EdgePropertyExpression::eval() {
    return ectx_->getEdgeProp(*sym_, *prop_);
}

const Value& InputPropertyExpression::eval() {
    // TODO
    return ectx_->getInputProp(*prop_);
}

const Value& VariablePropertyExpression::eval() {
    return ectx_->getVarProp(*sym_, *prop_);
}

const Value& SourcePropertyExpression::eval() {
    return ectx_->getSrcProp(*sym_, *prop_);
}

const Value& DestPropertyExpression::eval() {
    return ectx_->getDstProp(*sym_, *prop_);
}

const Value& EdgeSrcIdExpression::eval() {
    return ectx_->getEdgeProp(*sym_, *prop_);
}

const Value& EdgeTypeExpression::eval() {
    return ectx_->getEdgeProp(*sym_, *prop_);
}

const Value& EdgeRankExpression::eval() {
    return ectx_->getEdgeProp(*sym_, *prop_);
}

const Value& EdgeDstIdExpression::eval() {
    return ectx_->getEdgeProp(*sym_, *prop_);
}
}  // namespace nebula
