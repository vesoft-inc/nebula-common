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
cpp_include "common/datatypes/DateOps.h"
cpp_include "common/datatypes/VertexOps.h"
cpp_include "common/datatypes/EdgeOps.h"
cpp_include "common/datatypes/PathOps.h"
cpp_include "common/datatypes/ValueOps.h"
cpp_include "common/datatypes/MapOps.h"
cpp_include "common/datatypes/ListOps.h"
cpp_include "common/datatypes/SetOps.h"
cpp_include "common/datatypes/DataSetOps.h"
cpp_include "common/datatypes/KeyValueOps.h"
cpp_include "common/datatypes/HostAddrOps.h"

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
typedef binary (cpp.type = "nebula::VertexID") VertexID

typedef i64 (cpp.type = "nebula::Timestamp") Timestamp

typedef i32 (cpp.type = "nebula::IndexID") IndexID

typedef i32 (cpp.type = "nebula::Port") Port

// !! Struct Date has a shadow data type defined in the ThriftTypes.h
// So any change here needs to be reflected to the shadow type there
struct Date {
    1: i16 year;    // Calendar year, such as 2019
    2: byte month;    // Calendar month: 1 - 12
    3: byte day;      // Calendar day: 1 -31
} (cpp.type = "nebula::Date")


// !! Struct DateTime has a shadow data type defined in the ThriftTypes.h
// So any change here needs to be reflected to the shadow type there
struct DateTime {
    1: i16 year;
    2: byte month;
    3: byte day;
    4: byte hour;         // Hour: 0 - 23
    5: byte minute;       // Minute: 0 - 59
    6: byte sec;          // Second: 0 - 59
    7: i32 microsec;    // Micro-second: 0 - 999,999
    8: i32 timezone;    // Time difference in seconds
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
} (cpp.enum_strict cpp.type = "nebula::NullType")


// The type to hold any supported values during the query
union Value {
    1: NullType                                 nVal;
    2: bool                                     bVal;
    3: i64                                      iVal;
    4: double                                   fVal;
    5: binary                                   sVal;
    6: Date                                     dVal;
    7: DateTime                                 tVal;
    8: Vertex (cpp.type = "nebula::Vertex")     vVal (cpp.ref_type = "unique");
    9: Edge (cpp.type = "nebula::Edge")         eVal (cpp.ref_type = "unique");
    10: Path (cpp.type = "nebula::Path")        pVal (cpp.ref_type = "unique");
    11: List (cpp.type = "nebula::List")        lVal (cpp.ref_type = "unique");
    12: Map (cpp.type = "nebula::Map")          mVal (cpp.ref_type = "unique");
    13: Set (cpp.type = "nebula::Set")          uVal (cpp.ref_type = "unique");
    14: DataSet (cpp.type = "nebula::DataSet")  gVal (cpp.ref_type = "unique");
} (cpp.type = "nebula::Value")


// Ordered list
struct List {
    1: list<Value> values;
} (cpp.type = "nebula::List")


// Unordered key/values pairs
struct Map {
    1: map<binary, Value> (cpp.template = "std::unordered_map") kvs;
} (cpp.type = "nebula::Map")


// Unordered and unique values
struct Set {
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
    1: VertexID vid,
    2: list<Tag> tags,
} (cpp.type = "nebula::Vertex")


struct Edge {
    1: VertexID src,
    2: VertexID dst,
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


enum ErrorCode {
    SUCCEEDED                     = 0,

    // 1xxxx, for rpc Failure
    E_RPC_FAILED                  = 10002,

    // raft
    E_LEADER_CHANGED              = 10003,


    // 2xxxx, for graph Failure
    // Authentication error
    E_USERNAME_NOT_FOUND          = 20001,

    E_SESSION_NOT_EXIST           = 20002,
    E_SESSION_EXPIRED             = 20003,

    E_SYNTAX_ERROR                = 20004,
    // Nothing is executed When command is comment
    E_STATEMENT_EMTPY             = 20005,
    E_SEMANTIC_ERROR              = 20006,
    E_NOT_USE_SPACE               = 20007,
    E_INVALID_RANK                = 20008,
    E_INVALID_EDGE_TYPE           = 20009,
    E_OUT_OF_LEN_OF_USERNAME      = 20010,
    E_OUT_OF_LEN_OF_PASSWORD      = 20011,
    E_DUPLICATE_COLUMN_NAME       = 20012,
    E_COLUMN_COUNT_NOT_MATCH      = 20013,
    E_COLUMN_NOT_FOUND            = 20014,  // for query var or input ref
    E_UNSUPPORTED_EXPR            = 20015,
    E_OUT_OF_MAX_STATEMENTS       = 20016,  // the maximum number of statements
    E_PERMISSION_DENIED           = 20017,
    E_NO_TAGS                     = 20018,
    E_NO_EDGES                    = 20019,
    E_INVALID_EXPR                = 20020,
    E_INVALID_AUTH_TYPE           = 20021,

    // 3xxxx, for meta Failure
    E_NO_HOSTS                    = 30001,

    // existed
    E_SPACE_EXISTED               = 30002,
    E_TAG_EXISTED                 = 30003,
    E_EDGE_EXISTED                = 30004,
    E_INDEX_EXISTED               = 30005,
    E_PROP_EXISTED                = 30006,
    E_USER_EXISTED                = 30007,

    // it will not return by graphd, it just use by internal
    E_INVALID_HOST                = 30008,

    E_UNSUPPORTED                 = 30009,
    E_NOT_DROP_PROP               = 30010,
    E_BALANCER_RUNNING            = 30011,
    E_CONFIG_IMMUTABLE            = 30012,
    E_CONFLICT                    = 30013,
    E_INVALID_PARAM               = 30014,
    // it will not return by graphd, it just use by internal
    E_WRONGCLUSTER                = 30015,

    E_STORE_FAILED                = 30016,
    E_STORE_SEGMENT_ILLEGAL       = 30017,
    E_BAD_BALANCE_PLAN            = 30018,
    E_BALANCED                    = 30019,
    E_NO_RUNNING_BALANCE_PLAN     = 30020,
    E_NO_VALID_HOST               = 30021,

    // Authentication Failure
    E_INVALID_PASSWORD            = 30023,    // use for change the password
    E_IMPROPER_ROLE               = 30024,
    E_INVALID_PARTITION_NUM       = 30025,
    E_INVALID_REPLICA_FACTOR      = 30026,
    E_INVALID_CHARSET             = 30027,
    E_INVALID_COLLATE             = 30028,
    E_CHARSET_COLLATE_NOT_MATCH   = 30029,

    // Admin Failure
    E_SNAPSHOT_FAILED             = 30030,   // Create snapshot plan failed
    E_BLOCK_WRITE_FAILED          = 30031,
    E_INDEX_WITH_TTL              = 30032,
    E_PROP_WITH_TTL               = 30033,
    E_ILLEGAL_TTL_COLUMN_TYPE     = 30034
    E_ADD_JOB_FAILED              = 30035,
    E_STOP_JOB_FAILED             = 30036,
    E_SAVE_JOB_FAILED             = 30037,

    // 4xxxx, for storage Failure
    E_PART_NOT_FOUND              = 40001,
    E_KEY_NOT_FOUND               = 40002,
    E_CONSENSUS_ERROR             = 40003,
    E_DATA_TYPE_MISMATCH          = 40004,
    E_INVALID_FIELD_VALUE         = 40005,
    E_INVALID_OPERATION           = 40006,
    E_NOT_NULLABLE                = 40007,   // Not allowed to be null
    E_NO_DEFAULT_VALUE            = 40008,   // The field neither can be NULL, nor has a default value
    E_OUT_OF_RANGE                = 40009,   // Value exceeds the range of type
    E_ATOMIC_OP_FAILED            = 40010,   // Atomic operation failed

    // meta failures
    E_IMPROPER_DATA_TYPE          = 40011,
    E_INVALID_SPACEVIDLEN         = 40012,

    // Invalid request
    E_INVALID_FILTER              = 40013,
    E_INVALID_FIELD               = 40014,
    E_INVALID_RETURN              = 40015,
    E_INVALID_STORE               = 40016,
    E_INVALID_PEER                = 40017,    // Internal error code, storage return ro meta, graph will not receive
    E_RETRY_EXHAUSTED             = 40018,    // Internal error code, storage return ro meta, graph will not receive
    E_TRANSFER_LEADER_FAILED      = 40019,    // Internal error code, storage return ro meta, graph will not receive
    E_INVALID_STAT_TYPE           = 40020,

    // checkpoint failed
    E_FAILED_TO_CHECKPOINT        = 40021,
    E_CHECKPOINT_BLOCKED          = 40022,

    // partial result, used for kv interfaces
    E_PARTIAL_RESULT              = 40023,

    // Filter out
    E_FILTER_OUT                  = 40024,     // Internal error code, graph will return secceeded

    E_INVALID_DATA                = 40025,

    // task manager failed
    E_USER_CANCEL                 = 40026,     // Internal error code, storage return ro meta, graph will not receive
    E_REBUILD_INDEX_FAILED        = 40027,
    E_INVALID_TASK_PARAM          = 40028,     // Internal error code, storage return ro meta, graph will not receive


    // 5xxxx, for common Failure
    E_SPACE_NOT_FOUND             = 50001,
    E_TAG_NOT_FOUND               = 50002,
    E_EDGE_NOT_FOUND              = 50003,
    E_INDEX_NOT_FOUND             = 50004,
    E_EDGE_PROP_NOT_FOUND         = 50005,
    E_TAG_PROP_NOT_FOUND          = 50007,
    E_PROP_NOT_FOUND              = 50008,
    E_USER_NOT_FOUND              = 50009,
    E_ROLE_NOT_FOUND              = 50010,
    E_CONFIG_NOT_FOUND            = 50012,

    E_INVALID_VID                 = 50013,

    // internal error
    E_INTERNAL_ERROR              = 100000,
} (cpp.enum_strict)


