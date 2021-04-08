/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <gflags/gflags.h>

#include "common/time/TimeZoneInfo.h"

DEFINE_string(time_zone_file,
              "share/resources/date_time_zonespec.csv",
              "The file path to the timezone file.");

namespace nebula {
namespace time {

/*static*/ ::boost::local_time::tz_database TimeZone::tzdb;

}   // namespace time
}   // namespace nebula
