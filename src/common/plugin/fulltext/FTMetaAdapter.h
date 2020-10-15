/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_PLUGIN_FULLTEXT_M_ADAPTER_H_
#define COMMON_PLUGIN_FULLTEXT_M_ADAPTER_H_

#include "common/base/Base.h"
#include "common/plugin/fulltext/FTUtils.h"

namespace nebula {
namespace plugin {

class FTMetaAdapter {
public:
    FTMetaAdapter() = default;

    virtual ~FTMetaAdapter() = default;

    virtual bool createIndex(const HttpClient& client,
                             const std::string& index,
                             const std::string& indexTemplate) const = 0;

    virtual bool dropIndex(const HttpClient& client,
                           const std::string& index) const = 0;

    virtual bool activeNode(const HttpClient& client,
                            std::vector<std::string>& nodes) const = 0;

    virtual bool indexExist(const HttpClient& client,
                            const std::string& index) const = 0;

    virtual bool listIndexes(const HttpClient& client, folly::dynamic& indexes) const = 0;
};

}  // namespace plugin
}  // namespace nebula

#endif  // COMMON_PLUGIN_FULLTEXT_M_ADAPTER_H_
