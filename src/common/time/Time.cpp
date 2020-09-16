/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "gflags/gflags.h"

#include "common/time/Time.h"

namespace nebula {
namespace time {

// If it's invalid timezone the service initialize will failed.
DEFINE_string(timezone_name, "Etc/UTC", "The name of timezone used in current system");

}  // time
}  // nebula
