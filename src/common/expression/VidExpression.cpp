/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/VidExpression.h"

#include "common/expression/ExprVisitor.h"

namespace nebula {

const Value& VidExpression::eval(ExpressionContext &ctx) {
    result_ = ctx.getInputProp(kVid);
    return result_;
}

void VidExpression::accept(ExprVisitor *visitor) {
    visitor->visit(this);
}

}   // namespace nebula
