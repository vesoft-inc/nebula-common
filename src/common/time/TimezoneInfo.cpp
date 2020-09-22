/* Copyright (c) 2010 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/time/TimezoneInfo.h"

namespace nebula {
namespace time {

/*static*/ const std::unordered_map<std::string, std::tuple<std::string, int32_t>> tzdb =
#include "common/time/Timezone"
;  // NOLINT

}  // namespace time
}  // namespace nebula
