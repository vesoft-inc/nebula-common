/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

namespace cpp nebula.graph

include "graph.thrift"

/*
 *
 *  Note: In order to support multiple languages, all string
 *        have to be defined as **binary** in the thrift file
 *
 */

enum Language {
    UNKNOWN = 0,
    L_EN = 1,
    L_ZH = 2,

}(cpp.enum_strict)

enum Encoding {
    UNKNOWN = 0,
    E_UTF8 = 1,
}(cpp.enum_strict)

const map<graph.ErrorCode, map<Language, binary>(cpp.template = "std::unordered_map")>
(cpp.template = "std::unordered_map") ErrorMsgUTF8Map = {
    graph.ErrorCode.E_DISCONNECTED: {
        Language.L_EN: "Disconnected",
    },
    graph.ErrorCode.E_RPC_FAILURE: {
        Language.L_EN: "Rpc failure: %s",
    },
    graph.ErrorCode.E_RPC_FAILURE: {
        Language.L_EN: "Rpc failure: %s",
    },
    graph.ErrorCode.E_NOT_USE_SPACE: {
        Language.L_EN: "Please choose a graph space with `USE spaceName' firstly",
    },
    graph.ErrorCode.E_INVALID_PARAM: {
        Language.L_EN: "Invalid param `%s'",
    },
    graph.ErrorCode.E_NO_HOSTS: {
        Language.L_EN: "No hosts",
    },
    graph.ErrorCode.E_NO_VALID_HOST: {
        Language.L_EN: "No valid host",
    },
    graph.ErrorCode.E_SPACE_NOT_FOUND: {
        Language.L_EN: "Space `%s' not found",
    },
    graph.ErrorCode.E_TAG_NOT_FOUND: {
        Language.L_EN: "Tag `%s' not found",
    },
    graph.ErrorCode.E_EDGE_NOT_FOUND: {
        Language.L_EN: "Edge `%s' not found",
    },
    graph.ErrorCode.E_INDEX_NOT_FOUND: {
        Language.L_EN: "Index `%s' not found",
    },
    graph.ErrorCode.E_USER_NOT_FOUND: {
        Language.L_EN: "User `%s' not found",
    },
    graph.ErrorCode.E_CONFIG_NOT_FOUND: {
        Language.L_EN: "Config `%s' not found",
    },
    graph.ErrorCode.E_COLUMN_NOT_FOUND: {
        Language.L_EN: "Column name `%s' not found",
    },
    graph.ErrorCode.E_WRONG_COLUMN_NUM: {
        Language.L_EN: "Wrong column number",
    },
    graph.ErrorCode.E_SPACE_EXISTED: {
        Language.L_EN: "Space `%s' existed",
    },
    graph.ErrorCode.E_TAG_EXISTED: {
        Language.L_EN: "Tag `%s' existed",
    },
    graph.ErrorCode.E_EDGE_EXISTED: {
        Language.L_EN: "Edge `%s' existed",
    },
    graph.ErrorCode.E_INDEX_EXISTED: {
        Language.L_EN: "Index `%s' existed",
    },
    graph.ErrorCode.E_TAG_CONFLICT: {
        Language.L_EN: "Tag `%s' conflict",
    },
    graph.ErrorCode.E_EDGE_CONFLICT: {
        Language.L_EN: "Edge `%s' conflict",
    },
    graph.ErrorCode.E_DEFAULT_NOT_DEFINE: {
        Language.L_EN: "Column `%s' has not default value",
    },
    graph.ErrorCode.E_VARIABLE_NOT_DEFINE: {
        Language.L_EN: "Variable `%s' not defined",
    },
    graph.ErrorCode.E_SRC_NOT_DEFINE: {
        Language.L_EN: "Src not defined",
    },
    graph.ErrorCode.E_DST_NOT_DEFINE: {
        Language.L_EN: "Dst not defined",
    },
    graph.ErrorCode.E_INVALID_PARAM: {
        Language.L_EN: "Invalid param `%s'",
    },
    graph.ErrorCode.E_CONFIG_IMMUTABLE: {
        Language.L_EN: "Config immutable `%s'",
    },
    graph.ErrorCode.E_CHARSET_NOT_SUPPORT: {
        Language.L_EN: "Charset `%s' not support",
    },
    graph.ErrorCode.E_COLLACTION_NOT_SUPPORT: {
        Language.L_EN: "Collation `%s' not support",
    },
    graph.ErrorCode.E_NOT_COMPLETE: {
        Language.L_EN: "Not complete, completeness: %d",
    },
    graph.ErrorCode.E_INTERNAL_ERROR: {
        Language.L_EN: "Internal error: %s",
    },
    graph.ErrorCode.E_BALANCED: {
        Language.L_EN: "The cluster is balanced",
    },
    graph.ErrorCode.E_BALANCER_RUNNING: {
        Language.L_EN: "The balancer is running",
    },
    graph.ErrorCode.E_BAD_BALANCE_PLAN: {
        Language.L_EN: "Bad balance plan",
    },
    graph.ErrorCode.E_NO_RUNNING_BALANCE_PLAN: {
        Language.L_EN: "No running balance plan",
    },
    graph.ErrorCode.E_CORRUPTTED_BALANCE_PLAN: {
        Language.L_EN: "Corruptted balance plan",
    },
}

