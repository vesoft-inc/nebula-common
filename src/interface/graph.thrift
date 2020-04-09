/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

namespace cpp nebula.graph
namespace java com.vesoft.nebula.graph
namespace go nebula.graph

include "common.thrift"

/*
 *
 *  Note: In order to support multiple languages, all string
 *        have to be defined as **binary** in the thrift file
 *
 */

enum ErrorCode {
    SUCCEEDED = 0,
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


struct Row {
    1: list<common.Value> columns;
}


struct DataSet {
    1: list<binary>    column_names;   // Column names
    2: list<Row>       rows;
}


struct ExecutionResponse {
    1: required ErrorCode       error_code;
    2: required i32             latency_in_us;  // Execution time on server
    3: optional list<DataSet>   data;           // Can return multiple dataset
    4: optional binary          space_name;
}


struct AuthResponse {
    1: required ErrorCode   error_code;
    2: optional i64         session_id;
}


service GraphService {
    AuthResponse authenticate(1: binary username, 2: binary password)

    oneway void signout(1: i64 sessionId)

    ExecutionResponse execute(1: i64 sessionId, 2: binary stmt)

    // Same as execute(), but response will be a json string
    binary executeJson(1: i64 sessionId, 2: binary stmt)
}
