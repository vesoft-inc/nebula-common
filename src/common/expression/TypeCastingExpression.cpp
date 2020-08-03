/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/TypeCastingExpression.h"

namespace nebula {

static std::unordered_multimap<Value::Type, Value::Type> typeCastMap = {
    {Value::Type::INT, Value::Type::INT},
    {Value::Type::FLOAT, Value::Type::INT},
    {Value::Type::STRING, Value::Type::INT},

    {Value::Type::STRING, Value::Type::STRING},
    {Value::Type::__EMPTY__, Value::Type::STRING},
    {Value::Type::NULLVALUE, Value::Type::STRING},
    {Value::Type::BOOL, Value::Type::STRING},
    {Value::Type::INT, Value::Type::STRING},
    {Value::Type::FLOAT, Value::Type::STRING},
    {Value::Type::DATE, Value::Type::STRING},
    {Value::Type::DATETIME, Value::Type::STRING},
    {Value::Type::MAP, Value::Type::STRING},
    {Value::Type::PATH, Value::Type::STRING},
    {Value::Type::LIST, Value::Type::STRING},
    {Value::Type::SET, Value::Type::STRING},
    {Value::Type::DATASET, Value::Type::STRING},
    {Value::Type::VERTEX, Value::Type::STRING},
    {Value::Type::EDGE, Value::Type::STRING},

    {Value::Type::BOOL, Value::Type::BOOL},
    {Value::Type::__EMPTY__, Value::Type::BOOL},
    {Value::Type::NULLVALUE, Value::Type::BOOL},
    {Value::Type::INT, Value::Type::BOOL},
    {Value::Type::FLOAT, Value::Type::BOOL},
    {Value::Type::STRING, Value::Type::BOOL},
    {Value::Type::DATE, Value::Type::BOOL},

    {Value::Type::FLOAT, Value::Type::FLOAT},
    {Value::Type::INT, Value::Type::FLOAT},
    {Value::Type::STRING, Value::Type::FLOAT}
};

static bool TypeCastingExpression::validateTypeCast(const Value::Type& operandType,
                                                    const Value::Type& type) {
    auto range = typeCastMap.equal_range(operandType);
    if (range.first == typeCastMap.end() && range.second == typeCastMap.end()) {
        return false;
    }
    for (const auto& it = range.first; it != range.second; ++it) {
        if (it->second == type) {
            return true;
        }
    }
    return false;
}

const Value& TypeCastingExpression::eval(ExpressionContext& ctx) {
    auto val = operand_->eval(ctx);

    switch (vType_) {
        case Value::Type::BOOL: {
            auto result = val.toBool();
            if (!result.ok()) {
                return Value::kNullValue;
            }
            result_.setBool(result.value());
            break;
        }
        case Value::Type::INT: {
            auto result = val.toInt();
            if (!result.ok()) {
                return Value::kNullValue;
            }
            result_.setInt(result.value());
            break;
        }
        case Value::Type::FLOAT: {
            auto result = val.toFloat();
            if (!result.ok()) {
                return Value::kNullValue;
            }
            result_.setFloat(result.value());
            break;
        }
        case Value::Type::STRING: {
            result_.setStr(val.toString());
            break;
        }
        default: {
            LOG(ERROR) << "Can not convert the type of `" << val << "` to `" << vType_ << "`";
            return Value::kNullValue;
        }
    }
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

std::string TypeCastingExpression::toString() const {
    std::stringstream out;
    out << "(" << vType_ << ")" << operand_->toString();
    return out.str();
}

}  // namespace nebula
