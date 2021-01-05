/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/utils/IndexUtils.h"

namespace nebula {

/*static*/ Value::Type IndexUtils::toValueType(meta::cpp2::PropertyType type) {
    switch (type) {
        case meta::cpp2::PropertyType::BOOL:
            return Value::Type::BOOL;
        case meta::cpp2::PropertyType::INT64:
        case meta::cpp2::PropertyType::INT32:
        case meta::cpp2::PropertyType::INT16:
        case meta::cpp2::PropertyType::INT8:
        case meta::cpp2::PropertyType::TIMESTAMP:
            return Value::Type::INT;
        case meta::cpp2::PropertyType::VID:
            return Value::Type::VERTEX;
        case meta::cpp2::PropertyType::FLOAT:
        case meta::cpp2::PropertyType::DOUBLE:
            return Value::Type::FLOAT;
        case meta::cpp2::PropertyType::STRING:
        case meta::cpp2::PropertyType::FIXED_STRING:
            return Value::Type::STRING;
        case meta::cpp2::PropertyType::DATE:
            return Value::Type::DATE;
        case meta::cpp2::PropertyType::TIME:
            return Value::Type::TIME;
        case meta::cpp2::PropertyType::DATETIME:
            return Value::Type::DATETIME;
        case meta::cpp2::PropertyType::UNKNOWN:
            return Value::Type::__EMPTY__;
    }
    return Value::Type::__EMPTY__;
}

/*static*/ std::size_t IndexUtils::fieldIndexLen(const meta::cpp2::ColumnTypeDef &f) {
    switch (IndexUtils::toValueType(f.get_type())) {
        case Value::Type::BOOL: {
            return sizeof(bool);
        }
        case Value::Type::INT: {
            return sizeof(int64_t);
        }
        case Value::Type::FLOAT: {
            return sizeof(double);
        }
        case Value::Type::STRING: {
            return *DCHECK_NOTNULL(f.get_type_length());
        }
        case Value::Type::TIME: {
            return sizeof(int8_t) * 3 + sizeof(int32_t);
        }
        case Value::Type::DATE: {
            return sizeof(int8_t) * 2 + sizeof(int16_t);
        }
        case Value::Type::DATETIME: {
            return sizeof(int32_t) + sizeof(int16_t) + sizeof(int8_t) * 5;
        }
        default: {
            return 0;
        }
    }
}

}   // namespace nebula
