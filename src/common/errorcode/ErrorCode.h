/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_ERRORCODE_ERRORCODE_H
#define COMMON_ERRORCODE_ERRORCODE_H

/**
 * name format: E_<ModuleName>_<ErrorName>
 * rpc error format: 001 * 10000 + ErrorId(0001~9999)
 * net error format: 002 * 10000 + ErrorId(0001~9999)
 * file error format: 003 * 10000 + ErrorId(0001~9999)
 * config error format: 004 * 10000 + ErrorId(0001~9999)
 * process error format: 005 * 10000 + ErrorId(0001~9999)
 * time value error format: 006 * 10000 + ErrorId(0001~9999)
 * function expression error format: 007 * 10000 + ErrorId(0001~9999)
 * fulltext function error format: 008 * 10000 + ErrorId(0001~9999)
 * system error format: 009 * 10000 + ErrorId(0001~9999)
 * space error format: 010 * 10000 + ErrorId(0001~9999)
 * index error format: 011 * 10000 + ErrorId(0001~9999)
 * ttl error format: 012 * 10000 + ErrorId(0001~9999)
 * charset error format: 013 * 10000 + ErrorId(0001~9999)
 * stats error format: 014 * 10000 + ErrorId(0001~9999)
 * the kv interface of metad error format: 015 * 10000 + ErrorId(0001~9999)
 *
 * name format: E_<ServiceName>_<ModuleName>_<ErrorName>
 * id format: ServiceId(1~3) * 10000000 + ModuleId(001~999) * 10000 + ErrorId(0001~9999)
 * The serviceId of graph is 1
 * The serviceId of meta is 2
 * The serviceId of storage is 3
 * 
 * The moduleId of rpc is 001
 * The moduleId of admin is 002
 * The moduleId of config is 003
 * The moduleId of index is 004
 * The moduleId of job is 005
 * The moduleId of listener is 006
 * The moduleId of schema is 007
 * The moduleId of session is 008
 * The moduleId of user is 009
 * The moduleId of zone is 010
 * The moduleId of rpc is 011
 * The moduleId of raft is 012
 * The moduleId of parser is 013
 * The moduleId of semantic is 014
 * The moduleId of data is 015
 * The moduleId of permission is 016
 * The moduleId of rocksdb is 017
 * The moduleId of stat is 018
 * The moduleId of es is 019
 */

namespace nebula {
enum class ErrorCode : int32_t {
    SUCCEEDED = 0,

    E_RPC_FAILED = 10001,
    // metad or storaged can be happen leader change
    E_LEADER_CHANGED = 10002,

    // some unsupported features
    E_UNSUPPORTED = 10003,

    // The net error 0020001~0029999
    E_NET_GET_IPV4_FAILED = 20001,
    E_NET_HOST_NOT_FOUND = 20002,
    E_NET_BAD_IP = 20003,
    E_NET_BAD_PORT = 20004,

    // The file error 0030001~0039999
    E_FILE_NOT_FOUND = 30001,
    E_FILE_NOT_PERMISSION = 30002,
    E_FILE_CONTENT_IS_EMPTY = 30003,

    E_CONFIG_ITEM_NOT_FOUND = 40001,
    E_CONFIG_ITEM_IS_NO_ARRAY = 40002,
    E_CONFIG_INVALID_ITEM_TYPE = 40003,
    E_CONFIG_WRONG_JSON_FORMAT = 40004,

    // Process format error, 0050001~0059999
    E_PROCESS_RUN_COMMAND_FAILED = 50001,
    E_PROCESS_READ_COMMAND_RESULT_FAILED = 50002,
    E_PROCESS_NOT_AVAILABLE_PID = 50003,
    E_PROCESS_CREATE_PID_FAILED = 50004,

    // Time format error, 0060001~0069999
    E_INVALID_TIME_FORMAT = 60001,
    E_INVALID_DATA_TYPE = 60002,
    E_EMPTY_TIMEZONE = 60003,
    E_INVALID_TIMEZONE = 60004,

    // Function error
    E_FUNCTION_NOT_SUPPORTED = 70001,
    E_FUNCTION_DYNAMIC_NOT_SUPPORTED = 70002,
    E_FUNCTION_ARITY_NOT_MATCH = 70003,
    E_FUNCTION_INVALID_ARG_TYPE = 70004,
    // Agg Function error
    E_AGG_FUNCTION_NOT_SUPPORTED = 70005,
    E_AGG_FUNCTION_DYNAMIC_NOT_SUPPORTED = 70006,

    // The file error 0080001~0089999
    E_FULLTEXT_INDEX_ID_NOT_FOUND = 80001,
    E_FULLTEXT_INDEX_NAME_NOT_FOUND = 80002,
    E_FULLTEXT_CLIENT_IS_EMPTY = 80003,
    E_LISTENER_NOT_FOUND = 80004,

    // The system error 0090001~0099999
    E_REGISTER_SIGNAL_ERROR = 90001,
    E_SYSTEM_CALL_FAILED = 90002,

    // space vid error, 0100001~0109999
    E_SPACE_INVALID_VID_LEN = 100001,
    E_SPACE_INVALID_VID_TYPE = 100002,

    // cache schema error, 0110001~0110001
    E_INDEX_SCHEMA_ID_NOT_FOUND = 110001,
    E_INDEX_SCHEMA_NAME_NOT_FOUND = 110002,

    // ttl error, 0120001~0129999
    E_TTL_NOT_SET = 120001,
    E_TTL_INVALID_PROPERTY = 120002,


    // Charset error, 0130001~0139999
    E_CHARSET_NOT_SUPPORTED = 130001,
    E_COLLATION_NOT_SUPPORTED = 130002,
    E_CHARSET_AND_COLLATION_NOT_MATCH = 130003,

    // stats error
    E_STATS_INVALID_NAME = 140001,

    // the error of meta client's kv interfaces
    E_KV_ARGUMENTS_INVALID = 150001,

    // The ErrorCode for graph
    // RPC prefix: 1001, graphd want to get info from cache,
    // but the cache is not ready load from metad
    E_GRAPH_WAIT_FOR_METAD_READY = 10010001,

    // prefix: 1007
    E_GRAPH_SCHEMA_TAG_ID_NOT_FOUND = 10070001,
    E_GRAPH_SCHEMA_EDGE_ID_NOT_FOUND = 10070002,
    // prefix: 1007
    E_GRAPH_SCHEMA_TAG_NAME_NOT_FOUND = 10070003,
    E_GRAPH_SCHEMA_EDGE_NAME_NOT_FOUND = 10070004,

    // prefix: 1004
    E_GRAPH_INDEX_TAG_INDEX_ID_NOT_FOUND = 10040001,
    E_GRAPH_INDEX_EDGE_INDEX_ID_NOT_FOUND = 10040002,
    E_GRAPH_INDEX_TAG_INDEX_NAME_NOT_FOUND = 10040003,
    E_GRAPH_INDEX_EDGE_INDEX_NAME_NOT_FOUND = 10040004,

    // prefix: 1002
    E_GRAPH_SPACE_ID_NOT_FOUND = 10020001,
    E_GRAPH_SPACE_NAME_NOT_FOUND = 10020002,

    // prefix: 1019
    E_GRAPH_ES_RUN_COMMAND_FAILED = 10190001,

    // The ErrorCode for meta
    // prefix: 2007
    E_META_SCHEMA_TAG_NOT_FOUND = 20070001,
    E_META_SCHEMA_EDGE_NOT_FOUND = 20070002,

    // prefix: 2004
    E_META_SCHEMA_TAG_INDEX_NOT_FOUND = 20070001,
    E_META_SCHEMA_EDGE_INDEX_NOT_FOUND = 20070002,

    // prefix: 2002
    E_GRAPH_SPACE_NOT_FOUND = 20020001,

    // The ErrorCode for storage
    // prefix: 3007
    E_STORAGE_SCHEMA_TAG_ID_NOT_FOUND = 30010001,
    E_STORAGE_SCHEMA_EDGE_ID_NOT_FOUND = 30010002,
    E_STORAGE_SCHEMA_TAG_NAME_NOT_FOUND = 30010003,
    E_STORAGE_SCHEMA_EDGE_NAME_NOT_FOUND = 30010004,

    // prefix: 3004
    E_STORAGE_INDEX_TAG_INDEX_ID_NOT_FOUND = 30070001,
    E_STORAGE_INDEX_EDGE_INDEX_ID_NOT_FOUND = 30070002,
    E_STORAGE_INDEX_TAG_INDEX_NAME_NOT_FOUND = 30070003,
    E_STORAGE_INDEX_EDGE_INDEX_NAME_NOT_FOUND = 30070004,

    // prefix: 3002
    E_STORAGE_SPACE_ID_NOT_FOUND = 30020001,
    E_STORAGE_SPACE_NAME_NOT_FOUND = 30020002,
    E_STORAGE_PART_ID_NOT_FOUND = 30020003,
    E_STORAGE_HOST_NOT_FOUND = 30020004,

    // prefix: 1019
    E_STORAGE_ES_RUN_COMMAND_FAILED = 30190001,

    // Unexpected error
    E_INTERNAL_ERROR = -1,
};

}   // namespace nebula
#endif  // COMMON_ERRORCODE_ERRORCODE_H
