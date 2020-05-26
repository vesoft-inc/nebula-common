/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/FunctionCallExpression.h"

namespace nebula {

bool ArgumentList::operator==(const ArgumentList& rhs) const {
    if (args_.size() != rhs.args_.size()) {
        return false;
    }

    for (size_t i = 0; i < args_.size(); i++) {
        if (*(args_[i]) != *(rhs.args_[i])) {
            return false;
        }
    }

    return true;
}


bool FunctionCallExpression::operator==(const Expression& rhs) const {
    if (kind_ != rhs.kind()) {
        return false;
    }

    const auto& r = dynamic_cast<const FunctionCallExpression&>(rhs);
    return *name_ == *(r.name_) && *args_ == *(r.args_);
}


size_t FunctionCallExpression::encode(std::string& buf) const {
    size_t len = 1;

    // kind_
    buf.append(reinterpret_cast<const char*>(&kind_), sizeof(uint8_t));

    // name_
    size_t sz = name_->size();
    buf.append(reinterpret_cast<char*>(&sz), sizeof(size_t));
    buf.append(name_->data(), sz);
    len += sizeof(size_t) + sz;

    // args_
    if (args_) {
        sz = args_->numArgs();
    } else {
        sz = 0;
    }
    buf.append(reinterpret_cast<char*>(&sz), sizeof(size_t));
    len += sizeof(size_t);
    if (sz > 0) {
        for (const auto& arg : args_->args()) {
            len += arg->encode(buf);
        }
    }

    return len;
}


void FunctionCallExpression::resetFrom(char*& ptr, const char* end) {
    // Read name_
    CHECK_LT(ptr + sizeof(size_t), end);
    size_t sz = 0;
    memcpy(reinterpret_cast<void*>(&sz), ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    DCHECK_GT(sz, 0);
    CHECK_LT(ptr + sz, end);
    name_.reset(new std::string(ptr, sz));
    ptr += sz;

    // Read args_
    CHECK_LE(ptr + sizeof(size_t), end);
    sz = 0;
    memcpy(reinterpret_cast<void*>(&sz), ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    args_ = std::make_unique<ArgumentList>();
    for (size_t i = 0;  i < sz; i++) {
        args_->addArgument(Expression::decode(ptr, end));
    }
}


Value FunctionCallExpression::eval(const ExpressionContext& ctx) const {
    UNUSED(ctx);
    return Value(NullType::NaN);
}

}  // namespace nebula
