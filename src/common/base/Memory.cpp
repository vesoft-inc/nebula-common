/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/base/Memory.h"

#include "common/base/Logging.h"

namespace nebula {

MemInfo::MemInfo() noexcept {
    info_ = std::make_unique<struct sysinfo>();
    auto err = sysinfo(info_.get());
    if (err != 0) {
        LOG(ERROR) << "Fail to call sysinfo to get memory info, errno: " << err;
    }
}

}   // namespace nebula
