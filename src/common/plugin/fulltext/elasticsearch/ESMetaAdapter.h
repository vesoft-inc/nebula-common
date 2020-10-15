/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef NEBULA_PLUGIN_ESMETAADAPTER_H
#define NEBULA_PLUGIN_ESMETAADAPTER_H

#include "common/plugin/fulltext/FTMetaAdapter.h"

namespace nebula {
namespace plugin {
class ESMetaAdapter final : public FTMetaAdapter {
public:
    static std::unique_ptr<FTMetaAdapter> kAdapter;

    bool createIndex(const HttpClient& client,
                     const std::string& index,
                     const std::string& indexTemplate = nullptr) const override;

    bool dropIndex(const HttpClient& client,
                   const std::string& index) const override;

    bool activeNode(const HttpClient& client,
                    std::vector<std::string>& nodes) const override;

    bool indexExist(const HttpClient& client,
                    const std::string& index) const override;

    bool listIndexes(const HttpClient& client, folly::dynamic& indexes) const override;

private:
    ESMetaAdapter() {}

    std::string header(const HttpClient& client) const noexcept;

    bool checkResult(const std::string& ret, const std::string& cmd) const;
};

}  // namespace plugin
}  // namespace nebula

#endif   // NEBULA_PLUGIN_ESMETAADAPTER_H
