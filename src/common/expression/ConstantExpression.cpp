/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/ConstantExpression.h"
#include <thrift/lib/cpp2/protocol/Serializer.h>
#include "common/datatypes/ValueOps.h"

namespace nebula {

using serializer = apache::thrift::CompactSerializer;

bool ConstantExpression::operator==(const Expression& rhs) const {
    if (kind_ != rhs.kind()) {
        return false;
    }

    const auto& r = dynamic_cast<const ConstantExpression&>(rhs);
    return val_ == r.val_;
}


size_t ConstantExpression::encode(std::string& buf) const {
    size_t len = 1;

    // kind_
    buf.append(reinterpret_cast<const char*>(&kind_), sizeof(uint8_t));

    // val_
    size_t before = buf.size();
    serializer::serialize(val_, &buf);
    len += (buf.size() - before);

    return len;
}


void ConstantExpression::resetFrom(char*& ptr, const char* end) {
    // Deserialize val_
    size_t len = serializer::deserialize(folly::StringPiece(ptr, end), val_);
    ptr += len;
}

}  // namespace nebula
