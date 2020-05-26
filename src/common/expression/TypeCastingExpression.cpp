/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/TypeCastingExpression.h"

namespace nebula {

bool TypeCastingExpression::operator==(const Expression& rhs) const {
    if (kind_ != rhs.kind()) {
        return false;
    }

    const auto& r = dynamic_cast<const TypeCastingExpression&>(rhs);
    return vType_ == r.vType_ && *operand_ == *(r.operand_);
}


size_t TypeCastingExpression::encode(std::string& buf) const {
    size_t len = 1;

    // kind_
    buf.append(reinterpret_cast<const char*>(&kind_), sizeof(uint8_t));

    // vType_
    buf.append(reinterpret_cast<const char*>(&vType_), sizeof(uint8_t));
    len += sizeof(uint8_t);

    // operand_
    DCHECK(!!operand_);
    len += operand_->encode(buf);

    return len;
}


void TypeCastingExpression::resetFrom(char*& ptr, const char* end) {
    // Read vType_
    CHECK_LT(ptr, end);
    memcpy(reinterpret_cast<void*>(&vType_), ptr, sizeof(uint8_t));
    ptr += sizeof(uint8_t);

    // Read operand_
    operand_ = Expression::decode(ptr, end);
    CHECK(!!operand_);
}


Value TypeCastingExpression::eval(const ExpressionContext& ctx) const {
    // TODO:
    UNUSED(ctx);
    return Value(NullType::NaN);
}

}  // namespace nebula
