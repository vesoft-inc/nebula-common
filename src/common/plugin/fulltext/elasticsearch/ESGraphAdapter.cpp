/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/process/ProcessUtils.h"
#include "common/plugin/fulltext/elasticsearch/ESGraphAdapter.h"

namespace nebula {
namespace plugin {

std::unique_ptr<FTGraphAdapter> ESGraphAdapter::kAdapter =
    std::unique_ptr<ESGraphAdapter>(new ESGraphAdapter());

bool ESGraphAdapter::result(const std::string& ret,
                            const std::string& cmd,
                            std::vector<std::string>& rows) const {
    if (ret.empty()) {
        LOG(ERROR) << "command failed : " << cmd;
        LOG(ERROR) << ret;
        return false;
    }
    auto root = folly::parseJson(ret);
    auto rootHits = root.find("hits");
    if (rootHits != root.items().end()) {
        auto subHits = rootHits->second.find("hits");
        if (subHits != rootHits->second.items().end()) {
            for (auto& item : subHits->second) {
                auto s = item.find("_source");
                if (s != item.items().end()) {
                    auto v = s->second.find("value");
                    if (v != s->second.items().end()) {
                        rows.emplace_back(v->second.getString());
                    } else {
                        continue;
                    }
                } else {
                    continue;
                }
            }
        }
        return true;
    }
    LOG(ERROR) << "command failed : " << cmd;
    LOG(ERROR) << ret;
    return false;
}

bool ESGraphAdapter::prefix(const HttpClient& client,
                            const DocItem& item,
                            const LimitItem& limit,
                            std::vector<std::string>& rows) const {
    std::string cmd = header(client, item, limit) +
                      body(item, limit.maxRows_, FT_SEARCH_OP::kPrefix);
    auto ret = nebula::ProcessUtils::runCommand(cmd.c_str());
    if (!ret.ok()) {
        LOG(ERROR) << "Http PUT Failed: " << cmd;
        return false;
    }
    return result(ret.value(), cmd, rows);
}

bool ESGraphAdapter::wildcard(const HttpClient& client,
                              const DocItem& item,
                              const LimitItem& limit,
                              std::vector<std::string>& rows) const {
    std::string cmd = header(client, item, limit) +
                      body(item, limit.maxRows_, FT_SEARCH_OP::kWildcard);
    auto ret = nebula::ProcessUtils::runCommand(cmd.c_str());
    if (!ret.ok()) {
        LOG(ERROR) << "Http PUT Failed: " << cmd;
        return false;
    }
    return result(ret.value(), cmd, rows);
}

bool ESGraphAdapter::regexp(const HttpClient& client,
                            const DocItem& item,
                            const LimitItem& limit,
                            std::vector<std::string>& rows) const {
    std::string cmd = header(client, item, limit) +
                      body(item, limit.maxRows_, FT_SEARCH_OP::kRegexp);
    auto ret = nebula::ProcessUtils::runCommand(cmd.c_str());
    if (!ret.ok()) {
        LOG(ERROR) << "Http PUT Failed: " << cmd;
        return false;
    }
    return result(ret.value(), cmd, rows);
}

bool ESGraphAdapter::fuzzy(const HttpClient& client,
                           const DocItem& item,
                           const LimitItem& limit,
                           const folly::dynamic& fuzziness,
                           const std::string& op,
                           std::vector<std::string>& rows) const {
    std::string cmd = header(client, item, limit) +
                      body(item, limit.maxRows_, FT_SEARCH_OP::kFuzzy, fuzziness, op);
    auto ret = nebula::ProcessUtils::runCommand(cmd.c_str());
    if (!ret.ok()) {
        LOG(ERROR) << "Http PUT Failed: " << cmd;
        return false;
    }
    return result(ret.value(), cmd, rows);
}

std::string ESGraphAdapter::header(const HttpClient& client,
                                   const DocItem& item,
                                   const LimitItem& limit) const noexcept {
    //    curl -H "Content-Type: application/json; charset=utf-8"
    //    -XGET http://127.0.0.1:9200/my_temp_index_3/_search?timeout=10ms
    std::stringstream os;
    os << CURL << CURL_CONTENT_JSON << XGET;
    os << client.toString() << item.index << "/_search?timeout=" << limit.timeout_ << "ms" << "\"";
    return os.str();
}

folly::dynamic ESGraphAdapter::schemaBody(int32_t id) const noexcept {
    // "term": {"tag_id": 1}
    folly::dynamic itemTag = folly::dynamic::object("tag_id", id);
    return folly::dynamic::object("term", itemTag);
}

folly::dynamic ESGraphAdapter::columnBody(const std::string& col) const noexcept {
    // "term": {"column_id": "col1"}
    folly::dynamic itemColumn = folly::dynamic::object("column_id", DocIDTraits::column(col));
    return folly::dynamic::object("term", itemColumn);
}

std::string ESGraphAdapter::body(const DocItem& item,
                                 int32_t maxRows,
                                 FT_SEARCH_OP type,
                                 const folly::dynamic& fuzziness,
                                 const std::string& op) const noexcept {
    folly::dynamic obj;
    switch (type) {
        case FT_SEARCH_OP::kPrefix : {
            obj = prefixBody(item.val);
            break;
        }
        case FT_SEARCH_OP::kWildcard : {
            obj = wildcardBody(item.val);
            break;
        }
        case FT_SEARCH_OP::kRegexp : {
            obj = regexpBody(item.val);
            break;
        }
        case FT_SEARCH_OP::kFuzzy : {
            obj = fuzzyBody(item.val, fuzziness, op);
        }
    }
    auto itemArray = folly::dynamic::array(schemaBody(item.schema),
                                           columnBody(item.column),
                                           obj);
    folly::dynamic itemMust = folly::dynamic::object("must", itemArray);
    folly::dynamic itemBool = folly::dynamic::object("bool", itemMust);
    folly::dynamic itemQuery = folly::dynamic::object("query", itemBool)
                                                     ("_source", "value")
                                                     ("size", maxRows)
                                                     ("from", 0);
    std::stringstream os;
    os << " -d'" << folly::toJson(itemQuery) << "'";
    return os.str();
}

folly::dynamic ESGraphAdapter::prefixBody(const std::string& prefix) const noexcept {
    // {"prefix": {"value": "a"}}
    folly::dynamic itemValue = folly::dynamic::object("value", prefix);
    return folly::dynamic::object("prefix", itemValue);
}

folly::dynamic ESGraphAdapter::wildcardBody(const std::string& wildcard) const noexcept {
    // {"wildcard": {"value": "*a"}}
    folly::dynamic itemValue = folly::dynamic::object("value", wildcard);
    return folly::dynamic::object("wildcard", itemValue);
}

folly::dynamic ESGraphAdapter::regexpBody(const std::string& regexp) const noexcept {
    // {"regexp": {"value": "c+"}}
    folly::dynamic itemValue = folly::dynamic::object("value", regexp);
    return folly::dynamic::object("regexp", itemValue);
}

folly::dynamic ESGraphAdapter::fuzzyBody(const std::string& regexp,
                                         const folly::dynamic& fuzziness,
                                         const std::string& op) const noexcept {
    // {"match": {"value": {"query":"ccc aaa","fuzziness": "AUTO","operator":  "and"}}}
    folly::dynamic items = folly::dynamic::object("query", regexp)
                                                 ("fuzziness", fuzziness)
                                                 ("operator", op);
    folly::dynamic value = folly::dynamic::object("value", items);
    return folly::dynamic::object("match", value);
}

}  // namespace plugin
}  // namespace nebula
