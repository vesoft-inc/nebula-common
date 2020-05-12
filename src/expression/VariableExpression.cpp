/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "expression/VariableExpression.h"
#include "datatypes/List.h"

namespace nebula {
const Value& VariableExpression::eval() {
    return expCtxt_->getVar(*var_);
}

const Value& VersionedVariableExpression::eval() {
    if (version_ != nullptr) {
        auto version = version_->eval();
        auto ver = version.getInt();
        return expCtxt_->getVersionedVar(*var_, ver);
    } else {
        return expCtxt_->getVar(*var_);
    }
}
}  // namespace nebula
