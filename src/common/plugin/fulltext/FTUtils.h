/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_PLUGIN_FULLTEXT_UTILS_H_
#define COMMON_PLUGIN_FULLTEXT_UTILS_H_

#include <iomanip>
#include "common/base/Base.h"
#include "common/datatypes/HostAddr.h"
#include "common/encryption/MD5Utils.h"

#define CURL "/usr/bin/curl"
#define XPUT " -XPUT"
#define XPOST " -XPOST"
#define XGET " -XGET"
#define XDELETE " -XDELETE"
#define MAX_VALUE_LEN 255
#define CURL_CONTENT_JSON " -H \"Content-Type: application/json; charset=utf-8\""
#define CURL_CONTENT_NDJSON " -H \"Content-Type: application/x-ndjson; charset=utf-8\""

namespace nebula {
namespace plugin {

enum class FT_SEARCH_OP {
    kPrefix = 0,
    kWildcard = 1,
    kRegexp = 2,
    kFuzzy = 3,
};

struct HttpClient {
    HostAddr host;
    std::string user;
    std::string password;

    ~HttpClient() = default;

    HttpClient(HttpClient&& v) noexcept
        : host(std::move(v.host))
        , user(std::move(v.user))
        , password(std::move(v.password)) {}

    explicit HttpClient(const HttpClient& v) noexcept
        : host(v.host)
        , user(v.user)
        , password(v.password) {}

    explicit HttpClient(HostAddr&& h) noexcept
        : host(std::move(h)) {}

    explicit HttpClient(const HostAddr& h) noexcept
        : host(h) {}

    HttpClient(HostAddr&& h,
               std::string&& u,
               std::string&& p) noexcept
        : host(std::move(h))
        , user(std::move(u))
        , password(std::move(p)) {}

    HttpClient(const HostAddr& h,
               const std::string& u,
               const std::string& p) noexcept
        : host(h)
        , user(u)
        , password(p) {}

    void clear() {
        host.clear();
        user.clear();
        password.clear();
    }

    std::string toString() const {
        std::stringstream os;
        if (!user.empty()) {
            os << "-u " << user;
            if (!password.empty()) {
                os << ":" << password;
            }
        }
        os << " \"http://" << host.host << ":" << host.port << "/";
        return os.str();
    }
};

struct DocItem {
    std::string index;
    std::string column;
    int32_t part;
    int32_t schema;
    std::string val;

    ~DocItem() = default;

    DocItem(DocItem&& v) noexcept
    : index(std::move(v.index))
    , column(std::move(v.column))
    , part(std::move(v.part))
    , schema(std::move(v.schema))
    , val(std::move(v.val)) {}

    explicit DocItem(const DocItem& v) noexcept
        : index(v.index)
        , column(v.column)
        , part(v.part)
        , schema(v.schema)
        , val(v.val) {}

    DocItem(std::string&& idx,
            std::string&& col,
            int32_t&& p,
            int32_t&& sc,
            std::string&& v)
        : index(std::move(idx))
        , column(std::move(col))
        , part(std::move(p))
        , schema(std::move(sc))
        , val(std::move(v)) {}

    DocItem(const std::string& idx,
            const std::string& col,
            const int32_t& p,
            const int32_t& sc,
            const std::string& v)
        : index(idx)
        , column(col)
        , part(p)
        , schema(sc)
        , val(v) {}
};

struct LimitItem {
    int32_t timeout_;
    int32_t maxRows_;

    ~LimitItem() = default;

    LimitItem(int32_t timeout, int32_t maxRows)
        : timeout_(timeout)
        , maxRows_(maxRows) {}
};

struct DocIDTraits {
    static std::string id(int32_t id) {
        // int32_t max value is 2147483647, It takes 10 bytes to turn into a string
        // for PartitionID ,TagID and EdgeType
        std::stringstream ss;
        ss << std::setw(10) << std::setfill('0') << id;
        return ss.str();
    }

    static std::string column(const std::string &col) {
        // normalized column name is 32 bytes
        return encryption::MD5Utils::md5Encode(col);
    }

    static std::string val(const std::string &v) {
        // normalized column name is 32 bytes
        return ((v.size() > MAX_VALUE_LEN) ? v.substr(0, MAX_VALUE_LEN) : v);
    }

    static std::string docId(const DocItem& item) {
        // partId_schemaId_column_value,
        // The value length limit is 255 bytes
        std::stringstream ss;
        ss << id(item.part) << "_" << id(item.schema) << "_" << column(item.column) << "_";
        ss << ((item.val.size() > MAX_VALUE_LEN) ? item.val.substr(0, MAX_VALUE_LEN) : item.val);
        return ss.str();
    }
};

}  // namespace plugin
}  // namespace nebula

#endif  // COMMON_PLUGIN_FULLTEXT_UTILS_H_
