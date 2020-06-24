/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/TypeCastingExpression.h"

namespace nebula {
const Value& TypeCastingExpression::eval(ExpressionContext& ctx) {
    UNUSED(ctx);
    return result_;
}


bool TypeCastingExpression::operator==(const Expression& rhs) const {
    if (kind_ != rhs.kind()) {
        return false;
    }

    const auto& r = dynamic_cast<const TypeCastingExpression&>(rhs);
    return vType_ == r.vType_ && *operand_ == *(r.operand_);
}


void TypeCastingExpression::writeTo(Encoder& encoder) const {
    // kind_
    encoder << kind_;

    // vType_
    encoder << vType_;

    // operand_
    DCHECK(!!operand_);
    encoder << *operand_;
}


void TypeCastingExpression::resetFrom(Decoder& decoder) {
    // Read vType_
    vType_ = decoder.readValueType();

    // Read operand_
    operand_ = decoder.readExpression();
    CHECK(!!operand_);
}

std::string TypeToString(Value::Type type) {
    switch (type) {
        case Value::Type::INT:
            return "int";
        case Value::Type::STRING:
            return "string";
        case Value::Type::FLOAT:
            return "float";
        case Value::Type::BOOL:
            return "bool";
        case Value::Type::DATETIME:
            return  "datatime";
        case Value::Type::DATE:
            return "date";
        default:
            return "unknown type";
    }
}

std::string TypeCastingExpression::toString() const {
    std::string buf;
    buf.reserve(256);

    buf += "(";
    buf += TypeToString(vType_);
    buf += ")";
    buf += operand_->toString();

    return buf;
}

}  // namespace nebula
