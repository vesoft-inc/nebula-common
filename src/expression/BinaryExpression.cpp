/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "expression/BinaryExpression.h"

namespace nebula {

bool BinaryExpression::operator==(const Expression& rhs) const {
    if (kind_ != rhs.kind()) {
        return false;
    }

    const auto& r = dynamic_cast<const BinaryExpression&>(rhs);
    return *lhs_ == *(r.lhs_) && *rhs_ == *(r.rhs_);
}


size_t BinaryExpression::encode(std::string& buf) const {
    size_t len = 1;

    // kind_
    buf.append(reinterpret_cast<const char*>(&kind_), sizeof(uint8_t));

    // lhs_
    DCHECK(!!lhs_);
    len += lhs_->encode(buf);

    // rhs_
    DCHECK(!!rhs_);
    len += rhs_->encode(buf);

    return len;
}


void BinaryExpression::resetFrom(char*& ptr, const char* end) {
    CHECK_LT(ptr, end);
    // Read lhs_
    lhs_ = Expression::decode(ptr, end);
    CHECK(!!lhs_);
    // Read rhs_
    rhs_ = Expression::decode(ptr, end);
    CHECK(!!rhs_);
}

}  // namespace nebula
