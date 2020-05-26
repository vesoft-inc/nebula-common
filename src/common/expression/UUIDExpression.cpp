/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/UUIDExpression.h"

namespace nebula {

bool UUIDExpression::operator==(const Expression& rhs) const {
    if (kind_ != rhs.kind()) {
        return false;
    }

    const auto& r = dynamic_cast<const UUIDExpression&>(rhs);
    return *field_ == *(r.field_);
}


size_t UUIDExpression::encode(std::string& buf) const {
    size_t len = 1;

    // kind_
    buf.append(reinterpret_cast<const char*>(&kind_), sizeof(uint8_t));

    // field_
    CHECK(!!field_);
    size_t sz = field_->size();
    buf.append(reinterpret_cast<char*>(&sz), sizeof(size_t));
    buf.append(field_->data(), sz);
    len += sizeof(size_t) + sz;

    return len;
}


void UUIDExpression::resetFrom(char*& ptr, const char* end) {
    // Read field_
    CHECK_LE(ptr + sizeof(size_t), end);
    size_t sz = 0;
    memcpy(reinterpret_cast<void*>(&sz), ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    CHECK_LE(ptr + sz, end);
    field_.reset(new std::string(ptr, sz));
    ptr += sz;
}


Value UUIDExpression::eval(const ExpressionContext& ctx) const {
    // TODO
    UNUSED(ctx);
    return Value(NullType::NaN);
}

}  // namespace nebula
