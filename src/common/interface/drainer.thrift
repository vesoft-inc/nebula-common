/* vim: ft=proto
 * Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

namespace cpp nebula.drainer
namespace java com.vesoft.nebula.drainer
namespace go nebula.drainer
namespace csharp nebula.drainer
namespace js nebula.drainer
namespace py nebula2.drainer

include "common.thrift"

/*
enum ErrorCode {
    // for common code
    SUCCEEDED                         = 0,
    E_DISCONNECTED                    = -1,        // RPC Failure
    E_FAIL_TO_CONNECT                 = -2,
    E_RPC_FAILURE                     = -3,
    E_LEADER_CHANGED                  = -4,
    // only unify metad and storaged error code
    E_SPACE_NOT_FOUND                 = -5,
    E_WRONGCLUSTER                    = -6,
}
*/

struct LogEntry {
    1: common.ClusterID cluster;
    2: binary           log_str;
}


/*
  AppendLogRequest send a log message to from sync listener to drainer
*/
struct AppendLogRequest {
    // last_log_term_sent and last_log_id_sent are the term and log id
    // for the last log being sent
    //
    1: common.ClusterID        clusterId;           // source cluster ID
    2: common.GraphSpaceID     space;               // Graphspace ID
    3: common.PartitionID      part;                // Partition ID
    4: common.LogID            last_log_id;         // To be sent last log id this time
    5: common.TermID           last_log_term_sent;  // LastlogTerm sent successfully last time
    6: common.LogID            last_log_id_sent;    // LastlogId sent successfully last time
    //
    // In the case of AppendLogRequest, the id of the first log is greater than or equal to
    //      last_log_id_sent + 1
    //
    // All logs in the log_str_list must belong to the same term,
    // which specified by log_term
    //
    7: common.TermID          log_term;
    // log count
    8: i64                    count;
    // log id is in the range [last_log_id_sent + 1, last_log_id]
    9: list<LogEntry>         log_str_list;
    10: bool                  sending_snapshot;
}

struct AppendLogResponse {
    1: common.ErrorCode    error_code;
    2: common.LogID        last_log_id;
    3: common.TermID       last_log_term;
}

service DrainerService {
     AppendLogResponse appendLog(1: AppendLogRequest req);
}
