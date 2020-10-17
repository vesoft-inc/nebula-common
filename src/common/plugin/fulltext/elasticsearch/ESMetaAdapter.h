/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef NEBULA_PLUGIN_ESMETAADAPTER_H
#define NEBULA_PLUGIN_ESMETAADAPTER_H

#include <gtest/gtest.h>
#include "common/plugin/fulltext/FTMetaAdapter.h"

namespace nebula {
namespace plugin {
class ESMetaAdapter final : public FTMetaAdapter {
    FRIEND_TEST(FulltextPluginTest, ESCreateIndexTest);
    FRIEND_TEST(FulltextPluginTest, ESDropIndexTest);
    FRIEND_TEST(FulltextPluginTest, ESListIndexTest);
    FRIEND_TEST(FulltextPluginTest, ESIsExistsIndexTest);

public:
    static std::unique_ptr<FTMetaAdapter> kAdapter;

    bool createIndex(const HttpClient& client,
                     const std::string& index,
                     const std::string& indexTemplate = "") const override;

    bool dropIndex(const HttpClient& client,
                   const std::string& index) const override;

    bool activeNode(const HttpClient& client,
                    std::vector<std::string>& nodes) const override;

    bool isExistsIndex(const HttpClient& client,
                       const std::string& index) const override;

    bool listIndices(const HttpClient& client,
                     std::vector<std::string>& indices) const override;

private:
    ESMetaAdapter() {}

    std::string header() const noexcept;

    bool statusCheck(const std::string& ret, const std::string& cmd) const;

    bool indicesCheck(const std::string& ret,
                      const std::string& cmd,
                      std::vector<std::string>& indices) const;

    std::string createIndexCmd(const HttpClient& client,
                               const std::string& index,
                               const std::string& indexTemplate = "") const noexcept;

    std::string dropIndexCmd(const HttpClient& client,
                             const std::string& index) const noexcept;

    std::string isExistsIndexCmd(const HttpClient& client,
                                 const std::string& index) const noexcept;

    std::string listIndicesCmd(const HttpClient& client) const noexcept;
};

}  // namespace plugin
}  // namespace nebula

#endif   // NEBULA_PLUGIN_ESMETAADAPTER_H
