/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/base/Memory.h"

#include <errno.h>

#include "common/base/Logging.h"

namespace nebula {

StatusOr<std::unique_ptr<MemInfo>> MemInfo::make() {
    std::unique_ptr<MemInfo> mem(new MemInfo);
    NG_RETURN_IF_ERROR(mem->init());
    return mem;
}

MemInfo::MemInfo() noexcept {
    info_ = std::make_unique<struct sysinfo>();
}

Status MemInfo::init() {
    if (sysinfo(info_.get()) == -1) {
        return Status::Error(ErrorCode::E_SYSTEM_CALL_FAILED,
                             ERROR_FLAG(2),
                             "sysinfo",
                             std::strerror(errno));
    }
    return Status::OK();
}

}   // namespace nebula
