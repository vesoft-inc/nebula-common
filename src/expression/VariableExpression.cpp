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
    auto& val = expCtxt_->getVar(*var_);
    if (UNLIKELY(val.type() != Value::Type::LIST)) {
        return null_;
    }

    auto version = version_->eval();
    if (UNLIKELY(version.type() != Value::Type::INT)) {
        return null_;
    }

    auto ver = version.getInt();
    if (UNLIKELY(static_cast<size_t>(ver) >= val.getList().size())) {
        return null_;
    }
    return val.getList()[ver];
}
}  // namespace nebula
