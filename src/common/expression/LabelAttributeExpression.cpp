/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/LabelAttributeExpression.h"

namespace nebula {

std::string LabelAttributeExpression::toString() const {
    CHECK(left()->kind() == Kind::kLabel || left()->kind() == Kind::kConstant);
    CHECK(right()->kind() == Kind::kLabel || right()->kind() == Kind::kConstant);
    return left()->toString() + "." + right()->toString();
}

}   // namespace nebula
