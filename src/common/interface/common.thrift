/* vim: ft=proto
 * Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */


namespace cpp nebula
namespace java com.vesoft.nebula
namespace go nebula
namespace js nebula
namespace csharp nebula
namespace py nebula2.common

cpp_include "common/thrift/ThriftTypes.h"
cpp_include "common/datatypes/DateOps.inl"
cpp_include "common/datatypes/VertexOps.inl"
cpp_include "common/datatypes/EdgeOps.inl"
cpp_include "common/datatypes/PathOps.inl"
cpp_include "common/datatypes/ValueOps.inl"
cpp_include "common/datatypes/MapOps.inl"
cpp_include "common/datatypes/ListOps.inl"
cpp_include "common/datatypes/SetOps.inl"
cpp_include "common/datatypes/DataSetOps.inl"
cpp_include "common/datatypes/KeyValueOps.inl"
cpp_include "common/datatypes/HostAddrOps.inl"

/*
 *
 *  Note: In order to support multiple languages, all strings
 *        have to be defined as **binary** in the thrift file
 *
 */

typedef i32 (cpp.type = "nebula::GraphSpaceID") GraphSpaceID
typedef i32 (cpp.type = "nebula::PartitionID") PartitionID
typedef i32 (cpp.type = "nebula::TagID") TagID
typedef i32 (cpp.type = "nebula::EdgeType") EdgeType
typedef i64 (cpp.type = "nebula::EdgeRanking") EdgeRanking
typedef i64 (cpp.type = "nebula::LogID") LogID
typedef i64 (cpp.type = "nebula::TermID") TermID

typedef i64 (cpp.type = "nebula::Timestamp") Timestamp

typedef i32 (cpp.type = "nebula::IndexID") IndexID

typedef i32 (cpp.type = "nebula::Port") Port

typedef i64 (cpp.type = "nebula::SessionID") SessionID

// !! Struct Date has a shadow data type defined in the Date.h
// So any change here needs to be reflected to the shadow type there
struct Date {
    1: i16 year;    // Calendar year, such as 2019
    2: byte month;    // Calendar month: 1 - 12
    3: byte day;      // Calendar day: 1 -31
} (cpp.type = "nebula::Date")

// !! Struct Time has a shadow data type defined in the Date.h
// So any change here needs to be reflected to the shadow type there
struct Time {
    1: byte hour;         // Hour: 0 - 23
    2: byte minute;       // Minute: 0 - 59
    3: byte sec;          // Second: 0 - 59
    4: i32 microsec;    // Micro-second: 0 - 999,999
} (cpp.type = "nebula::Time")

// !! Struct DateTime has a shadow data type defined in the Date.h
// So any change here needs to be reflected to the shadow type there
struct DateTime {
    1: i16 year;
    2: byte month;
    3: byte day;
    4: byte hour;         // Hour: 0 - 23
    5: byte minute;       // Minute: 0 - 59
    6: byte sec;          // Second: 0 - 59
    7: i32 microsec;    // Micro-second: 0 - 999,999
} (cpp.type = "nebula::DateTime")


enum NullType {
    __NULL__ = 0,
    NaN      = 1,
    BAD_DATA = 2,
    BAD_TYPE = 3,
    ERR_OVERFLOW = 4,
    UNKNOWN_PROP = 5,
    DIV_BY_ZERO = 6,
    OUT_OF_RANGE = 7,
} (cpp.enum_strict, cpp.type = "nebula::NullType")


// The type to hold any supported values during the query
union Value {
    1: NullType                                 nVal;
    2: bool                                     bVal;
    3: i64                                      iVal;
    4: double                                   fVal;
    5: binary                                   sVal;
    6: Date                                     dVal;
    7: Time                                     tVal;
    8: DateTime                                 dtVal;
    9: Vertex (cpp.type = "nebula::Vertex")     vVal (cpp.ref_type = "unique");
    10: Edge (cpp.type = "nebula::Edge")        eVal (cpp.ref_type = "unique");
    11: Path (cpp.type = "nebula::Path")        pVal (cpp.ref_type = "unique");
    12: NList (cpp.type = "nebula::List")       lVal (cpp.ref_type = "unique");
    13: NMap (cpp.type = "nebula::Map")         mVal (cpp.ref_type = "unique");
    14: NSet (cpp.type = "nebula::Set")         uVal (cpp.ref_type = "unique");
    15: DataSet (cpp.type = "nebula::DataSet")  gVal (cpp.ref_type = "unique");
} (cpp.type = "nebula::Value")


// Ordered list
struct NList {
    1: list<Value> values;
} (cpp.type = "nebula::List")


// Unordered key/values pairs
struct NMap {
    1: map<binary, Value> (cpp.template = "std::unordered_map") kvs;
} (cpp.type = "nebula::Map")


// Unordered and unique values
struct NSet {
    1: set<Value> (cpp.template = "std::unordered_set") values;
} (cpp.type = "nebula::Set")


struct Row {
    1: list<Value> values;
} (cpp.type = "nebula::Row")


struct DataSet {
    1: list<binary>    column_names;   // Column names
    2: list<Row>       rows;
} (cpp.type = "nebula::DataSet")


struct Tag {
    1: binary name,
    // List of <prop_name, prop_value>
    2: map<binary, Value> (cpp.template = "std::unordered_map") props,
} (cpp.type = "nebula::Tag")


struct Vertex {
    1: Value     vid,
    2: list<Tag> tags,
} (cpp.type = "nebula::Vertex")


struct Edge {
    1: Value src,
    2: Value dst,
    3: EdgeType type,
    4: binary name,
    5: EdgeRanking ranking,
    // List of <prop_name, prop_value>
    6: map<binary, Value> (cpp.template = "std::unordered_map") props,
} (cpp.type = "nebula::Edge")


struct Step {
    1: Vertex dst,
    2: EdgeType type,
    3: binary name,
    4: EdgeRanking ranking,
    5: map<binary, Value> (cpp.template = "std::unordered_map") props,
} (cpp.type = "nebula::Step")


// Special type to support path during the query
struct Path {
    1: Vertex src,
    2: list<Step> steps;
} (cpp.type = "nebula::Path")


struct HostAddr {
    // Host could be a valid IPv4 or IPv6 address, or a valid domain name
    1: string   host,
    2: Port     port,
} (cpp.type = "nebula::HostAddr")


struct KeyValue {
    1: binary key,
    2: binary value,
} (cpp.type = "nebula::KeyValue")

struct LogInfo {
    1: LogID  log_id;
    2: TermID term_id;
}

struct PartitionBackupInfo {
    1: map<PartitionID, LogInfo> (cpp.template = "std::unordered_map")  info,
}


/*
 * ErrorCode for graphd, metad, storaged,raftd
 * -1xxx for graphd
 * -2xxx for metad
 * -3xxx for storaged
 */
enum ErrorCode {
    // for common code
    SUCCEEDED                         = 0,

    E_DISCONNECTED                    = -1,        // RPC Failure
    E_RPC_FAILED                      = -2,
    E_LEADER_CHANGED                  = -3,
    E_FAIL_TO_CONNECT                 = -4,

    // only unify metad and storaged error code
    E_SPACE_NOT_FOUND                 = -5,
    E_TAG_NOT_FOUND                   = -6,
    E_EDGE_NOT_FOUND                  = -7,
    E_INDEX_NOT_FOUND                 = -8,
    E_EDGE_PROP_NOT_FOUND             = -9,
    E_TAG_PROP_NOT_FOUND              = -10,
    E_ROLE_NOT_FOUND                  = -11,
    E_CONFIG_NOT_FOUND                = -12,
    E_USER_NOT_FOUND                  = -13,       // User and permission error

    // backup failed
    E_BACKUP_FAILED                   = -14,
    E_BACKUP_EMPTY_TABLE              = -15,
    E_BACKUP_TABLE_FAILED             = -16,
    E_PARTIAL_RESULT                  = -17,
    E_REBUILD_INDEX_FAILED            = -18,
    E_PART_NOT_FOUND                  = -19,
    E_KEY_NOT_FOUND                   = -20,
    E_INVALID_PASSWORD                = -21,


    // 1xxx for graphd
    E_BAD_USERNAME_PASSWORD           = -1004,     // Authentication error
    E_SESSION_INVALID                 = -1005,     // Execution errors
    E_SESSION_TIMEOUT                 = -1006,
    E_SYNTAX_ERROR                    = -1007,
    E_EXECUTION_ERROR                 = -1008,
    E_STATEMENT_EMPTY                 = -1009,     // Nothing is executed When command is comment
    E_BAD_PERMISSION                  = -1011,
    E_SEMANTIC_ERROR                  = -1012,     // semantic error
    E_TOO_MANY_CONNECTIONS            = -1013,     // Exceeding the maximum number of connections
    E_PARTIAL_SUCCEEDED               = -1014,


    // 2xxx for metad
    E_NO_HOSTS                        = -2021,     // Operation Failure
    // it will not return by graphd, it just use by internal, used by heartbeat
    E_INVALID_HOST                    = -2022,
    E_WRONG_CLUSTER                   = -2023,

    E_SPACE_EXISTED                   = -2024,
    E_TAG_EXISTED                     = -2025,
    E_EDGE_EXISTED                    = -2026,
    E_INDEX_EXISTED                   = -2027,
    E_TAG_PROP_EXISTED                = -2028,
    E_EDGE_PROP_EXISTED               = -2029,
    E_USER_EXISTED                    = -2030,
    E_GROUP_EXISTED                   = -2031,
    E_ZONE_EXISTED                    = -2032,
    E_LISTENER_EXISTED                = -2033,

    E_GROUP_NOT_FOUND                 = -2034,
    E_ZONE_NOT_FOUND                  = -2035,
    E_LISTENER_NOT_FOUND              = -2036,
    E_NOT_DROP_GROUP                  = -2037,
    E_NOT_DROP_ZONE                   = -2038,


    E_UNSUPPORTED                     = -2039,
    E_NOT_DROP_PROP                   = -2040,

    E_CONFIG_IMMUTABLE                = -2041,
    E_INVALID_PARAM                   = -2042,

    E_STORE_FAILED                    = -2043,
    E_STORE_SEGMENT_ILLEGAL           = -2044,
    E_BAD_BALANCE_PLAN                = -2045,
    E_BALANCER_RUNNING                = -2046,
    E_BALANCED                        = -2047,  // balance is in progress
    E_NO_INVALID_BALANCE_PLAN         = -2048,
    E_INVALID_BALANCE_HOST            = -2049,

    // Authentication Failure
    E_IMPROPER_ROLE                   = -2050,
    E_INVALID_PARTITION_NUM           = -2051,
    E_INVALID_REPLICA_FACTOR          = -2052,
    E_INVALID_CHARSET                 = -2053,
    E_INVALID_COLLATE                 = -2054,
    E_CHARSET_COLLATE_NOT_MATCH       = -2055,

    // Admin Failure
    E_SNAPSHOT_FAILED                 = -2056,   // Create snapshot plan failed
    E_BLOCK_WRITE_FAILED              = -2057,
    E_ALTER_WITH_INDEX_TTL_CONFLICT   = -2058,
    E_ADD_JOB_FAILED                  = -2059,
    E_STOP_JOB_FAILED                 = -2060,
    E_SAVE_JOB_FAILED                 = -2061,
    E_BALANCER_FAILED                 = -2062,
    E_JOB_NOT_FINISHED                = -2063,
    E_TASK_REPORT_OUT_DATE            = -2064,
    E_INVALID_JOB                     = -2065,

    // Backup Failure
    E_BACKUP_BUILDING_INDEX           = -2066,
    E_BACKUP_SPACE_NOT_FOUND          = -2067,
    E_RESTORE_FAILED                  = -2068,

    // conflict
    E_ZONE_CONFLICT                   = -2069,
    E_HOST_CONFLICT                   = -2070,
    E_INDEX_CONFLICT                  = -2071,
    E_PROP_NAME_CONFLICT              = -2072,
    E_TOO_MANY_PROPS_IN_INDEX         = -2073,
    E_SCHEMA_TAG_CONFLICT             = -2074,
    E_SCHEMA_EDGE_CONFLICT            = -2075,
    E_ALTER_WITH_TTL                  = -2076,


    // 3xxx for storaged
    E_CONSENSUS_ERROR                 = -3001,
    E_DATA_TYPE_MISMATCH              = -3002,
    E_INVALID_FIELD_VALUE             = -3003,
    E_INVALID_OPERATION               = -3004,
    // Not allowed to be null
    E_NOT_NULLABLE                    = -3005,
    // The field neither can be NULL, nor has a default value
    E_NO_DEFAULT_VALUE                = -3006,
    // Value exceeds the range of type
    E_OUT_OF_RANGE                    = -3007,
    // Atomic operation failed
    E_ATOMIC_OP_FAILED                = -3008,

    // meta failures
    E_IMPROPER_DATA_TYPE              = -3009,
    E_INVALID_SPACEVIDLEN             = -3010,

    // Invalid request
    E_INVALID_FILTER                  = -3011,
    E_INVALID_FIELD                   = -3012,
    E_INVALID_RETURN                  = -3013,
    E_INVALID_STORE                   = -3014,
    E_INVALID_PEER                    = -3015,
    E_RETRY_EXHAUSTED                 = -3016,
    E_TRANSFER_LEADER_FAILED          = -3017,
    E_INVALID_STAT_TYPE               = -3018,

    // checkpoint failed
    E_CHECKPOINT_FAILED               = -3019,
    E_CHECKPOINT_BLOCKED              = -3020,

    // task manager failed
    E_USER_CANCEL                     = -3021,
    E_INVALID_TASK_PARAM              = -3022,

    E_INVALID_VID                     = -3023,
    // Filter out
    E_FILTER_OUT                      = -3024,
    E_INVALID_DATA                    = -3025,

    // transaction
    E_OUTDATED_LOCK                   = -3027,
    E_DATA_CONFLICT                   = -3028,
    E_MUTATE_EDGE_CONFLICT            = -3029,
    E_MULTI_TAG_CONFLICT              = -3030,
    E_MULTI_EDGE_CONFLICT             = -3031,

    // internal error
    E_INTERNAL_ERROR                  = -4001,
    E_INVALID_EXPR_FORMAT             = -4002,

    E_UNKNOWN                         = -4003,
} (cpp.enum_strict)
