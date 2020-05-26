/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/AliasPropertyExpression.h"

namespace nebula {

bool AliasPropertyExpression::operator==(const Expression& rhs) const {
    if (kind_ != rhs.kind()) {
        return false;
    }

    const auto& r = dynamic_cast<const AliasPropertyExpression&>(rhs);
    return *ref_ == *(r.ref_) && *alias_ == *(r.alias_) && *prop_ == *(r.prop_);
}


size_t AliasPropertyExpression::encode(std::string& buf) const {
    size_t len = 1;

    // kind_
    buf.append(reinterpret_cast<const char*>(&kind_), sizeof(uint8_t));

    // ref_
    size_t sz = ref_ ? ref_->size() : 0;
    buf.append(reinterpret_cast<char*>(&sz), sizeof(size_t));
    if (sz > 0) {
        buf.append(ref_->data(), sz);
    }
    len += sizeof(size_t) + sz;

    // alias_
    sz = alias_ ? alias_->size() : 0;
    buf.append(reinterpret_cast<char*>(&sz), sizeof(size_t));
    if (sz > 0) {
        buf.append(alias_->data(), sz);
    }
    len += sizeof(size_t) + sz;

    // prop_
    sz = prop_ ? prop_->size() : 0;
    buf.append(reinterpret_cast<char*>(&sz), sizeof(size_t));
    if (sz > 0) {
        buf.append(prop_->data(), sz);
    }
    len += sizeof(size_t) + sz;

    return len;
}


void AliasPropertyExpression::resetFrom(char*& ptr, const char* end) {
    // Read ref_
    CHECK_LT(ptr + sizeof(size_t), end);
    size_t sz = 0;
    memcpy(reinterpret_cast<void*>(&sz), ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    if (sz > 0) {
        CHECK_LT(ptr + sz, end);
        ref_.reset(new std::string(ptr, sz));
    }
    ptr += sz;

    // Read alias_
    CHECK_LT(ptr + sizeof(size_t), end);
    sz = 0;
    memcpy(reinterpret_cast<void*>(&sz), ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    if (sz > 0) {
        CHECK_LT(ptr + sz, end);
        alias_.reset(new std::string(ptr, sz));
    }
    ptr += sz;

    // Read prop_
    CHECK_LE(ptr + sizeof(size_t), end);
    sz = 0;
    memcpy(reinterpret_cast<void*>(&sz), ptr, sizeof(size_t));
    ptr += sizeof(size_t);
    if (sz > 0) {
        CHECK_LE(ptr + sz, end);
        prop_.reset(new std::string(ptr, sz));
    }
    ptr += sz;
}


Value InputPropertyExpression::eval(const ExpressionContext& ctx) const {
    // TODO
    UNUSED(ctx);
    return Value(NullType::NaN);
}


Value VariablePropertyExpression::eval(const ExpressionContext& ctx) const {
    // TODO
    UNUSED(ctx);
    return Value(NullType::NaN);
}


Value SourcePropertyExpression::eval(const ExpressionContext& ctx) const {
    // TODO
    UNUSED(ctx);
    return Value(NullType::NaN);
}


Value DestPropertyExpression::eval(const ExpressionContext& ctx) const {
    // TODO
    UNUSED(ctx);
    return Value(NullType::NaN);
}


Value EdgeSrcIdExpression::eval(const ExpressionContext& ctx) const {
    // TODO
    UNUSED(ctx);
    return Value(NullType::NaN);
}


Value EdgeTypeExpression::eval(const ExpressionContext& ctx) const {
    // TODO
    UNUSED(ctx);
    return Value(NullType::NaN);
}


Value EdgeRankExpression::eval(const ExpressionContext& ctx) const {
    // TODO
    UNUSED(ctx);
    return Value(NullType::NaN);
}


Value EdgeDstIdExpression::eval(const ExpressionContext& ctx) const {
    // TODO
    UNUSED(ctx);
    return Value(NullType::NaN);
}

}  // namespace nebula
