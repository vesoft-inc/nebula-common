/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "base/Base.h"
#include "datatypes/HostAddr.h"

namespace nebula {

std::ostream& operator <<(std::ostream &os, const HostAddr &addr) {
    os << folly::stringPrintf("[%s:%u]", addr.host.c_str(), addr.port);
    return os;
}


bool HostAddr::operator==(const HostAddr& rhs) const {
    return host == rhs.host && port == rhs.port;
}

bool HostAddr::operator!=(const HostAddr& rhs) const {
    return !(*this == rhs);
}

bool HostAddr::operator<(const HostAddr& rhs) const {
    if (host == rhs.host) {
        return port < rhs.port;
    }
    return host < rhs.host;
}

std::string serializeHostAddr(const HostAddr& host) {
    std::string ret;
    ret.reserve(sizeof(size_t) + 15 + sizeof(Port));    // 255.255.255.255
    size_t len = host.host.size();
    ret.append(reinterpret_cast<char*>(&len), sizeof(size_t))
       .append(host.host.data(), len)
       .append(reinterpret_cast<const char*>(&host.port), sizeof(Port));
    return ret;
}

HostAddr deserializeHostAddr(folly::StringPiece raw) {
    HostAddr addr;
    CHECK_GE(raw.size(), sizeof(size_t) + sizeof(Port));  // host may be ""
    size_t offset = 0;
    size_t len = *reinterpret_cast<const size_t*>(raw.begin() + offset);
    offset += sizeof(size_t);
    addr.host = std::move(std::string(raw.begin() + offset, len));
    offset += len;
    addr.port = *reinterpret_cast<const Port*>(raw.begin() + offset);
    return addr;
}

}  // namespace nebula

