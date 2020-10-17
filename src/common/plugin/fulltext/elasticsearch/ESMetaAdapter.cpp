/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/plugin/fulltext/elasticsearch/ESMetaAdapter.h"
#include <common/plugin/fulltext/FTUtils.h>
#include "common/process/ProcessUtils.h"

namespace nebula {
namespace plugin {

std::unique_ptr<FTMetaAdapter> ESMetaAdapter::kAdapter =
    std::unique_ptr<ESMetaAdapter>(new ESMetaAdapter());

bool ESMetaAdapter::createIndex(const HttpClient& client,
                                const std::string& index,
                                const std::string&) const {
    // curl -H "Content-Type: application/json; charset=utf-8"
    //      -XPUT "http://127.0.0.1:9200/index_exist"
    std::string cmd = createIndexCmd(client, index);
    auto ret = nebula::ProcessUtils::runCommand(cmd.c_str());
    if (!ret.ok()) {
        LOG(ERROR) << "Http PUT Failed: " << cmd;
        return false;
    }
    return statusCheck(ret.value(), cmd);
}

std::string ESMetaAdapter::createIndexCmd(const HttpClient& client,
    const std::string& index, const std::string&) const noexcept {
    std::stringstream os;
    os << header() << XPUT << client.toString() << index << "\"";
    return os.str();
}

bool ESMetaAdapter::dropIndex(const HttpClient& client,
                              const std::string& index) const {
    // curl -H "Content-Type: application/json; charset=utf-8"
    //      -XPUT "http://127.0.0.1:9200/index_exist"
    std::string cmd = dropIndexCmd(client, index);
    auto ret = nebula::ProcessUtils::runCommand(cmd.c_str());
    if (!ret.ok()) {
        LOG(ERROR) << "Http PUT Failed: " << cmd;
        return false;
    }
    return statusCheck(ret.value(), cmd);
}

std::string ESMetaAdapter::dropIndexCmd(const HttpClient& client,
    const std::string& index) const noexcept {
    std::stringstream os;
    os << header() << XDELETE << client.toString() << index << "\"";
    return os.str();
}

bool ESMetaAdapter::activeNode(const HttpClient&,
                               std::vector<std::string>&) const {
    LOG(ERROR) << "No support yet";
    return true;
}

bool ESMetaAdapter::isExistsIndex(const HttpClient&,
                                  const std::string&) const {
    // curl -H "Content-Type: application/json; charset=utf-8"
    // -XGET "http://127.0.0.1:9200/_cat/indices/index_exist?v"
    LOG(ERROR) << "No support yet";
    return true;
}

std::string ESMetaAdapter::isExistsIndexCmd(const HttpClient& client,
    const std::string& index) const noexcept {
    std::stringstream os;
    os << header() << XGET << client.toString()
       << "_cat/indices/" << index << "?format=json" << "\"";
    return os.str();
}

bool ESMetaAdapter::listIndices(const HttpClient&,
                                std::vector<std::string>&) const {
    LOG(ERROR) << "No support yet";
    return true;
}

std::string ESMetaAdapter::listIndicesCmd(const HttpClient& client) const noexcept {
    std::stringstream os;
    os << header() << XGET << client.toString()
       << "_cat/indices?format=json" << "\"";
    return os.str();
}

std::string ESMetaAdapter::header() const noexcept {
    std::stringstream os;
    os << CURL << CURL_CONTENT_JSON;
    return os.str();
}

bool ESMetaAdapter::statusCheck(const std::string& ret, const std::string& cmd) const {
    if (ret.empty()) {
        LOG(ERROR) << "command failed : " << cmd;
        return false;
    }
    auto root = folly::parseJson(ret);
    auto result = root.find("acknowledged");
    if (result != root.items().end() && result->second.isBool() && result->second.getBool()) {
        return true;
    }
    LOG(ERROR) << "command failed : " << cmd;
    LOG(ERROR) << ret;
    return false;
}

bool ESMetaAdapter::indicesCheck(const std::string&,
                                 const std::string&,
                                 std::vector<std::string>&) const {
    // TODO (sky) : folly does not support parsing in the following format.
    //              so the method not supported yet.
    //
    //    [{
    //        "health": "yellow",
    //            "status": "open",
    //            "index": "index1",
    //            "uuid": "3lXSn-kQRUSE7oe5xBNUGQ",
    //            "pri": "1",
    //            "rep": "1",
    //            "docs.count": "3",
    //            "docs.deleted": "2",
    //            "store.size": "18.3kb",
    //            "pri.store.size": "18.3kb"
    //    }, {
    //        "health": "yellow",
    //            "status": "open",
    //            "index": "test_index",
    //            "uuid": "0FNdubc5RqCmAttN7FiPRw",
    //            "pri": "1",
    //            "rep": "1",
    //            "docs.count": "0",
    //            "docs.deleted": "0",
    //            "store.size": "208b",
    //            "pri.store.size": "208b"
    //    }]
    return false;
}

}  // namespace plugin
}  // namespace nebula
