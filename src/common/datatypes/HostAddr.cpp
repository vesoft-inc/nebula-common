/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/datatypes/HostAddr.h"

#include "common/base/Base.h"

namespace nebula {

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

}  // namespace nebula
