/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/time/TimezoneInfo.h"

namespace nebula {
namespace time {

/*static*/ ::boost::local_time::tz_database Timezone::tzdb;
/*static*/ constexpr char Timezone::kTzDbFile[];

}   // namespace time
}   // namespace nebula
