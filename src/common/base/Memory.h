/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_BASE_MEMORY_H_
#define COMMON_BASE_MEMORY_H_

#include <sys/sysinfo.h>

#include <cstdint>
#include <memory>

#include "common/base/StatusOr.h"
#include "common/cpp/helpers.h"

namespace nebula {

struct ProcessMemInfo {
    uint64_t   allocated;
    uint64_t   active;
    uint64_t   resident;
    uint64_t   mapped;
};

class MemInfo final : protected cpp::NonCopyable, protected cpp::NonMovable {
public:
    static std::unique_ptr<MemInfo> make();

    Status initProcessMemInfo();

    Status initSysMemInfo();

    uint64_t totalInKB() const {
        return (system_memory_info_->totalram * system_memory_info_->mem_unit) >> 10;
    }

    uint64_t freeInKB() const {
        return (system_memory_info_->freeram * system_memory_info_->mem_unit) >> 10;
    }

    uint64_t bufferInKB() const {
        return (system_memory_info_->bufferram * system_memory_info_->mem_unit) >> 10;
    }

    uint64_t usedInKB() const {
        return totalInKB() - freeInKB() - bufferInKB();
    }

    uint64_t processAllocatedInByte() const {
        return process_memory_info_->allocated;
    }

    uint64_t processActiveInByte() const {
        return process_memory_info_->active;
    }

    uint64_t processResidentInByte() const {
        return process_memory_info_->resident;
    }

    uint64_t processMappedInByte() const {
        return process_memory_info_->mapped;
    }

    bool hitsHighWatermark(float ratio = 0.8f) const {
        return usedInKB() > totalInKB() * ratio;
    }

private:
    MemInfo() noexcept;

    std::unique_ptr<struct sysinfo> system_memory_info_;
    std::unique_ptr<struct ProcessMemInfo> process_memory_info_;
};

}   // namespace nebula

#endif   // COMMON_BASE_MEMORY_H_
