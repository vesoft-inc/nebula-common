/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

namespace cpp nebula.graph
namespace java com.vesoft.nebula.graph
namespace go nebula.graph
namespace js nebula.graph
namespace csharp nebula.graph
namespace py nebula2.graph

include "common.thrift"

/*
 *
 *  Note: In order to support multiple languages, all string
 *        have to be defined as **binary** in the thrift file
 *
 */

enum ErrorCode {
    SUCCEEDED                  = 0,
    // 1xxx, for rpc Failure
    E_DISCONNECTED             = 1001,
    E_FAIL_TO_CONNECT          = 1002,
    E_RPC_FAILURE              = 1003,
    E_LEADER_CHANGE            = 1004,

    // 2xxx, for graph engine errors
    E_SESSION_INVALID          = 2001,
    E_SESSION_TIMEOUT          = 2002,
    E_BAD_USERNAME_PASSWORD    = 2003,
    E_SYNTAX_ERROR             = 2004,
    E_STATEMENT_EMPTY          = 2005,
    E_NOT_USE_SPACE            = 2006,
    E_COLUMN_NOT_FOUND         = 2007,
    E_WRONG_COLUMN_NUM         = 2008,
    E_DEFAULT_NOT_DEFINE       = 2009,
    E_VARIABLE_NOT_DEFINE      = 2010,
    E_SRC_NOT_DEFINE           = 2011,
    E_DST_NOT_DEFINE           = 2012,
    E_INVALID_PARAM            = 2013,

    // 3xxx, for storage engine errors
    E_KEY_NOT_FOUND            = 3001,
    E_KEY_HAS_EXISTS           = 3002,
    E_PART_NOT_FOUND           = 3004,
    E_CONSENSUS_ERROR          = 3005,
    E_STORAGE_LEADER_CHANGED   = 3006,
    E_FAILED_TO_CHECKPOINT     = 3007,
    E_CHECKPOINT_BLOCKED       = 3008,
    E_FILTER_OUT               = 3009,
    E_EDGE_PROP_NOT_FOUND      = 3010,
    E_TAG_PROP_NOT_FOUND       = 3011,
    E_IMPROPER_DATA_TYPE       = 3012,
    E_CHECK_POINT_BLOCKED      = 3013,

    // 4xxx, for meta service errors
    E_NO_HOSTS                 = 4001,
    E_NO_VALID_HOST            = 4002,
    E_META_LEADER_CHANGE       = 4003,
    E_HOST_NOT_FOUND           = 4004,
    E_SPACE_NOT_FOUND          = 4005,
    E_TAG_NOT_FOUND            = 4006,
    E_EDGE_NOT_FOUND           = 4007,
    E_SPACE_EXISTED            = 4008,
    E_TAG_EXISTED              = 4009,
    E_EDGE_EXISTED             = 4010,
    E_INDEX_EXISTED            = 4011,
    E_TAG_CONFLICT             = 4012,
    E_EDGE_CONFLICT            = 4013,
    E_META_LEADER_CHANGED      = 4014,
    E_INDEX_NOT_FOUND          = 4015,
    E_BALANCED                 = 4016,
    E_BALANCER_RUNNING         = 4017,
    E_BAD_BALANCE_PLAN         = 4018,
    E_NO_RUNNING_BALANCE_PLAN  = 4019,
    E_CORRUPTTED_BALANCE_PLAN  = 4020,
    E_WRONG_CLUSTER            = 4021,
    E_INVALID_PASSWORD         = 4022,
    E_IMPROPER_ROLE            = 4023,
    E_META_ERROR               = 4024,
    E_CHARSET_NOT_SUPPORT      = 4025,
    E_COLLACTION_NOT_SUPPORT   = 4026,
    E_CONFIG_NOT_FOUND         = 4027,
    E_CONFIG_IMMUTABLE         = 4028,

    // 5xxx, for user or permission error
    E_USER_NOT_FOUND           = 5001,
    E_BAD_PERMISSION           = 5002,
    E_NOT_COMPLETE             = 5003,

    // All
    E_UNSUPPORTED              = 6001,
    E_INTERNAL_ERROR           = 6002,
} (cpp.enum_strict)


struct ProfilingStats {
    // How many rows being processed in an executor.
    1: required i64  rows;
    // Duration spent in an executor.
    2: required i64  exec_duration_in_us;
    // Total duration spent in an executor, contains schedule time
    3: required i64  total_duration_in_us;
    // Other profiling stats data map
    4: optional map<binary, binary>
        (cpp.template = "std::unordered_map") other_stats;
}

// The info used for select/loop.
struct PlanNodeBranchInfo {
    // True if loop body or then branch of select
    1: required bool  is_do_branch;
    // select/loop node id
    2: required i64   condition_node_id;
}

struct Pair {
    1: required binary key;
    2: required binary value;
}

struct PlanNodeDescription {
    1: required binary                          name;
    2: required i64                             id;
    3: required binary                          output_var;
    // other description of an executor
    4: optional list<Pair>                      description;
    // If an executor would be executed multi times,
    // the profiling statistics should be multi-versioned.
    5: optional list<ProfilingStats>            profiles;
    6: optional PlanNodeBranchInfo              branch_info;
    7: optional list<i64>                       dependencies;
}

struct PlanDescription {
    1: required list<PlanNodeDescription>     plan_node_descs;
    // map from node id to index of list
    2: required map<i64, i64>
        (cpp.template = "std::unordered_map") node_index_map;
    // the print format of exec plan, lowercase string like `dot'
    3: required binary                        format;
}


struct ExecutionResponse {
    1: required ErrorCode               error_code;
    2: required i32                     latency_in_us;  // Execution time on server
    3: optional common.DataSet          data;
    4: optional binary                  space_name;
    5: optional binary                  error_msg;
    6: optional PlanDescription         plan_desc;
    7: optional binary                  comment;        // Supplementary instruction
}


struct AuthResponse {
    1: required ErrorCode   error_code;
    2: optional binary      error_msg;
    3: optional i64         session_id;
}


service GraphService {
    AuthResponse authenticate(1: binary username, 2: binary password)

    oneway void signout(1: i64 sessionId)

    ExecutionResponse execute(1: i64 sessionId, 2: binary stmt)

    // Same as execute(), but response will be a json string
    binary executeJson(1: i64 sessionId, 2: binary stmt)
}
