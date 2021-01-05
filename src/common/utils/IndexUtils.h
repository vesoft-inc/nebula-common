/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_UTILS_INDEXUTILS_H_
#define COMMON_UTILS_INDEXUTILS_H_

#include "common/datatypes/Value.h"
#include "common/interface/gen-cpp2/meta_types.h"

namespace nebula {

class IndexUtils {
public:
    static Value::Type toValueType(meta::cpp2::PropertyType type);

    static std::size_t fieldIndexLen(const meta::cpp2::ColumnTypeDef &f);
};

}   // namespace nebula

#endif   // COMMON_UTILS_INDEXUTILS_H_
