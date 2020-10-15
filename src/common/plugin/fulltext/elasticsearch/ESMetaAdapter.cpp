/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/plugin/fulltext/elasticsearch/ESMetaAdapter.h"

namespace nebula {
namespace plugin {

std::unique_ptr<FTMetaAdapter> ESMetaAdapter::kAdapter =
    std::unique_ptr<ESMetaAdapter>(new ESMetaAdapter());

bool ESMetaAdapter::createIndex(const HttpClient&,
                                const std::string&,
                                const std::string&) const {
    LOG(ERROR) << "No support yet";
    return false;
}

bool ESMetaAdapter::dropIndex(const HttpClient&,
                              const std::string&) const {
    LOG(ERROR) << "No support yet";
    return true;
}

bool ESMetaAdapter::activeNode(const HttpClient&,
                               std::vector<std::string>&) const {
    LOG(ERROR) << "No support yet";
    return true;
}

bool ESMetaAdapter::indexExist(const HttpClient&,
                               const std::string&) const {
    LOG(ERROR) << "No support yet";
    return true;
}

bool ESMetaAdapter::listIndexes(const HttpClient&,
                                folly::dynamic&) const {
    LOG(ERROR) << "No support yet";
    return true;
}

std::string ESMetaAdapter::header(const HttpClient& client) const noexcept {
    //    curl -H "Content-Type: application/json; charset=utf-8" -XPOST http://127.0.0.1:9200/
    std::stringstream os;
    os << CURL << CURL_CONTENT_JSON << XPOST << client.toString();
    return os.str();
}

bool ESMetaAdapter::checkResult(const std::string& ret, const std::string& cmd) const {
    auto root = folly::parseJson(ret);
    auto result = root.find("acknowledged");
    if (result != root.items().end() && result->second.getBool()) {
        return true;
    }
    LOG(ERROR) << "command failed : " << cmd;
    LOG(ERROR) << ret;
    return false;
}
}  // namespace plugin
}  // namespace nebula
