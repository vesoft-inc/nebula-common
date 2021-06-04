/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/base/Memory.h"

#include <errno.h>

#include "common/base/Logging.h"

namespace nebula {

std::unique_ptr<MemInfo> MemInfo::make() {
    return std::unique_ptr<MemInfo>(new MemInfo);;
}

MemInfo::MemInfo() noexcept {
    system_memory_info_ = std::make_unique<struct sysinfo>();
    process_memory_info_ = std::make_unique<struct ProcessMemInfo>();
}

Status MemInfo::initSysMemInfo() {
    if (sysinfo(system_memory_info_.get()) == -1) {
        auto err = errno;
        return Status::Error("Fail to call sysinfo to get system memory info, errno: %d", err);
    }
    return Status::OK();
}


Status MemInfo::initProcessMemInfo() {
#ifdef ENABLE_JEMALLOC_STATS
    uint64_t epoch = 1;
    size_t sz = sizeof(uint64_t);
    // refreshing of cached dynamic statistics
    mallctl("epoch", 0, 0, &epoch, sz);
    if (mallctl("stats.allocated", &(process_memory_info_->allocated), &sz, NULL, 0) ||
        mallctl("stats.active", &(process_memory_info_->active), &sz, NULL, 0) ||
        mallctl("stats.resident", &(process_memory_info_->resident), &sz, NULL, 0) ||
        mallctl("stats.mapped", &(process_memory_info_->mapped), &sz, NULL, 0)) {
        return Status::Error("Fail to call jemalloc mallctl to get process memory info");
    }
#endif
    return Status::OK();
}

}   // namespace nebula
