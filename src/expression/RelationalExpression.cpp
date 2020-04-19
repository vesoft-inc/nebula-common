/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "expression/RelationalExpression.h"

namespace nebula {
Value RelationaExpression::eval() const {
    auto lhs = lhs_->eval();
    auto rhs = ths_->eval();

    switch (type) {
        case EXP_
    }
}
}  // namespace nebula
