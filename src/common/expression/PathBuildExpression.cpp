/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/PathBuildExpression.h"

namespace nebula {
const Value& PathBuildExpression::eval(ExpressionContext& ctx) {
    UNUSED(ctx);
    return result_;
}

bool PathBuildExpression::operator==(const Expression& rhs) const {
    UNUSED(rhs);
    return false;
}

std::string PathBuildExpression::toString() const {
    return "";
}

void PathBuildExpression::accept(ExprVisitor* visitor) {
    UNUSED(visitor);
}

std::unique_ptr<Expression> PathBuildExpression::clone() const {
    return nullptr;
}

void PathBuildExpression::writeTo(Encoder &encoder) const {
    UNUSED(encoder);
}

void PathBuildExpression::resetFrom(Decoder &decoder) {
    UNUSED(decoder);
}
}  // namespace nebula
