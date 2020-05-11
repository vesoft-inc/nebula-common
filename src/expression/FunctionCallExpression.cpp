/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "expression/FunctionCallExpression.h"

namespace nebula {
const Value& FunctionCallExpression::eval() {
    std::vector<const Value*> args;
    args.reserve(args_.size());
    for (auto& arg : args_) {
        args.emplace_back(&arg->eval());
    }

    result_ = func_(args);
    return result_;
}
}  // namespace nebula
