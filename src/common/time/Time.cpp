/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "gflags/gflags.h"

#include "common/time/Time.h"

// If it's invalid timezone the service initialize will failed.
// Empty for system default configuration
DEFINE_string(timezone_name, "", "The name of timezone used in current system");

namespace nebula {
namespace time {

const DateTime TimeUtils::kEpoch(1970, 1, 1, 0, 0, 0, 0);

constexpr char TimeUtils::kTZdir[];

}   // namespace time
}   // namespace nebula
