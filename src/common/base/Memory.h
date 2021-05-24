/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_BASE_MEMORY_H_
#define COMMON_BASE_MEMORY_H_

#include <sys/sysinfo.h>

#include <cstdint>

#include "common/cpp/helpers.h"

namespace nebula {

class MemInfo final : protected cpp::NonCopyable, protected cpp::NonMovable {
public:
    MemInfo() noexcept {
        sysinfo(&info_);
    }

    uint64_t totalInKB() const {
        return (info_.totalram * info_.mem_unit) >> 10;
    }

    uint64_t freeInKB() const {
        return (info_.freeram * info_.mem_unit) >> 10;
    }

    uint64_t usedInKB() const {
        return totalInKB() - freeInKB();
    }

    bool hitsHighWatermark(float ratio = 0.9f) const {
        return usedInKB() > totalInKB() * ratio;
    }

private:
    struct sysinfo info_;
};

}   // namespace nebula

#endif   // COMMON_BASE_MEMORY_H_
