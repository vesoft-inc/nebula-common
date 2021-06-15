/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/charset/Charset.h"

namespace nebula {

Status CharsetInfo::isSupportCharset(const std::string& charsetName) {
    if (supportCharsets_.find(charsetName) == supportCharsets_.end()) {
        return Status::Error(ErrorCode::E_CHARSET_NOT_SUPPORTED,
                             ERROR_FLAG(1),
                             charsetName.c_str());
    }
    return Status::OK();
}


Status CharsetInfo::isSupportCollate(const std::string& collateName) {
    if (supportCollations_.find(collateName) == supportCollations_.end()) {
        return Status::Error(ErrorCode::E_COLLATION_NOT_SUPPORTED,
                             ERROR_FLAG(1),
                             collateName.c_str());
    }
    return Status::OK();
}


Status CharsetInfo::charsetAndCollateMatch(const std::string& charsetName,
                                           const std::string& collateName) {
    auto iter = charsetDesc_.find(charsetName);
    if (iter != charsetDesc_.end()) {
        for (auto& sc : iter->second.supportColls_) {
            if (!sc.compare(collateName)) {
                return Status::OK();
            }
        }
    }
    return  Status::Error(ErrorCode::E_CHARSET_AND_COLLATION_NOT_MATCH,
                          ERROR_FLAG(2),
                          charsetName.c_str(), collateName.c_str());
}


StatusOr<std::string> CharsetInfo::getDefaultCollationbyCharset(const std::string& charsetName) {
    auto iter = charsetDesc_.find(charsetName);
    if (iter != charsetDesc_.end()) {
        return iter->second.defaultColl_;
    }
    return Status::Error(ErrorCode::E_CHARSET_NOT_SUPPORTED, ERROR_FLAG(1), charsetName.c_str());
}


StatusOr<std::string> CharsetInfo::getCharsetbyCollation(const std::string& collationName ) {
    for (auto& cset : charsetDesc_) {
        for (auto& coll : cset.second.supportColls_) {
            if (!coll.compare(collationName)) {
                return cset.first;
            }
        }
    }
    return Status::Error(ErrorCode::E_COLLATION_NOT_SUPPORTED,
                         ERROR_FLAG(1),
                         collationName.c_str());
}

}   // namespace nebula
