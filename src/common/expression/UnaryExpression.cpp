/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/UnaryExpression.h"

namespace nebula {

bool UnaryExpression::operator==(const Expression& rhs) const {
    if (kind_ != rhs.kind()) {
        return false;
    }

    const auto& r = dynamic_cast<const UnaryExpression&>(rhs);
    return *operand_ == *(r.operand_);
}


size_t UnaryExpression::encode(std::string& buf) const {
    size_t len = 1;

    // kind_
    buf.append(reinterpret_cast<const char*>(&kind_), sizeof(uint8_t));

    // operand_
    DCHECK(!!operand_);
    len += operand_->encode(buf);

    return len;
}


void UnaryExpression::resetFrom(char*& ptr, const char* end) {
    // Read operand_
    operand_ = Expression::decode(ptr, end);
    CHECK(!!operand_);
}


Value UnaryExpression::eval(const ExpressionContext& ctx) const {
    UNUSED(ctx);
    switch (kind_) {
        case Kind::kUnaryPlus:
            return operand_->eval(ctx);
        case Kind::kUnaryNegate:
            return -(operand_->eval(ctx));
        case Kind::kUnaryNot:
            return !(operand_->eval(ctx));
        default:
            break;
    }
    LOG(FATAL) << "Unknown type: " << kind_;
}

}  // namespace nebula
