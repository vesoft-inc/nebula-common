/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef NEBULA_PLUGIN_ESGRAPHADAPTER_H
#define NEBULA_PLUGIN_ESGRAPHADAPTER_H

#include <gtest/gtest.h>
#include "common/plugin/fulltext/FTGraphAdapter.h"

namespace nebula {
namespace plugin {
class ESGraphAdapter final : public FTGraphAdapter {
    FRIEND_TEST(FulltextPluginTest, ESResultTest);
    FRIEND_TEST(FulltextPluginTest, ESPrefixTest);
    FRIEND_TEST(FulltextPluginTest, ESWildcardTest);
    FRIEND_TEST(FulltextPluginTest, ESRegexpTest);
    FRIEND_TEST(FulltextPluginTest, ESFuzzyTest);

public:
    static std::unique_ptr<FTGraphAdapter> kAdapter;

    bool prefix(const HttpClient& client,
                const DocItem& item,
                const LimitItem& limit,
                std::vector<std::string>& rows) const override;

    bool wildcard(const HttpClient& client,
                  const DocItem& item,
                  const LimitItem& limit,
                  std::vector<std::string>& rows) const override;

    bool regexp(const HttpClient& client,
                const DocItem& item,
                const LimitItem& limit,
                std::vector<std::string>& rows) const override;

    bool fuzzy(const HttpClient& client,
               const DocItem& item,
               const LimitItem& limit,
               const folly::dynamic& fuzziness,
               const std::string& op,
               std::vector<std::string>& rows) const override;

private:
    ESGraphAdapter() {}

    bool result(const std::string& ret,
                const std::string& cmd,
                std::vector<std::string>& rows) const;

    std::string header(const HttpClient& client,
                       const DocItem& item,
                       const LimitItem& limit) const noexcept;

    folly::dynamic schemaBody(int32_t id) const noexcept;

    folly::dynamic columnBody(const std::string& col) const noexcept;

    std::string body(const DocItem& item,
                     int32_t maxRows,
                     FT_SEARCH_OP type,
                     const folly::dynamic& fuzziness = nullptr,
                     const std::string& op = "") const noexcept;

    folly::dynamic prefixBody(const std::string& prefix) const noexcept;

    folly::dynamic wildcardBody(const std::string& wildcard) const noexcept;

    folly::dynamic regexpBody(const std::string& regexp) const noexcept;

    folly::dynamic fuzzyBody(const std::string& regexp,
                             const folly::dynamic& fuzziness,
                             const std::string& op) const noexcept;
};
}  // namespace plugin
}  // namespace nebula

#endif   // NEBULA_PLUGIN_ESGRAPHADAPTER_H
