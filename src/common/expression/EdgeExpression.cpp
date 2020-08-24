/* Copyright (c) 2019 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/EdgeExpression.h"

namespace nebula {

const Value& EdgeExpression::eval(ExpressionContext &ctx) {
    result_ = ctx.getEdge();
    return result_;
}

}   // namespace nebula
