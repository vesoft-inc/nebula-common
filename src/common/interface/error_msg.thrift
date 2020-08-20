/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

namespace cpp nebula.graph

include "common.thrift"

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

const map<common.ErrorCode, map<Language, binary>(cpp.template = "std::unordered_map")>
(cpp.template = "std::unordered_map") ErrorMsgUTF8Map = {
    common.ErrorCode.E_RPC_FAILED: {
        Language.L_EN: "RPC failed from `%s'.",
    },
    common.ErrorCode.E_LEADER_CHANGED: {
        Language.L_EN: "`%s' has leader change.",
    },
    common.ErrorCode.E_BAD_USERNAME_PASSWORD: {
        Language.L_EN: "Wrong username or password.",
    },
    common.ErrorCode.E_SESSION_INVALID: {
        Language.L_EN: "Session id `%ld' is invalid.",
    },
    common.ErrorCode.E_SYNTAX_ERROR: {
        Language.L_EN: "Syntax Error: %s.",
    },
    common.ErrorCode.E_SEMANTIC_ERROR: {
        Language.L_EN: "Semantic Error: %s.",
    },
    common.ErrorCode.E_NO_HOSTS: {
        Language.L_EN: "Not hosts.",
    },
    common.ErrorCode.E_SPACE_EXISTED: {
        Language.L_EN: "Space `%s' existed.",
    },
    common.ErrorCode.E_TAG_EXISTED: {
        Language.L_EN: "Tag `%s' existed.",
    },
    common.ErrorCode.E_EDGE_EXISTED: {
        Language.L_EN: "Edge `%s' existed.",
    },
    common.ErrorCode.E_INDEX_EXISTED: {
        Language.L_EN: "Index `%s' existed.",
    },
    common.ErrorCode.E_UNSUPPORTED: {
        Language.L_EN: "Unsupported.",
    },
    common.ErrorCode.E_NOT_DROP_PROP: {
        Language.L_EN: "Couldn't drop property.",
    },
    common.ErrorCode.E_CONFIG_IMMUTABLE: {
        Language.L_EN: "Config `%s' is immutable.",
    },
    common.ErrorCode.E_CONFLICT: {
        Language.L_EN: "`%s' is conflict.",
    },
    common.ErrorCode.E_INVALID_PARM: {
        Language.L_EN: "Invalid Param `%s'.",
    },
    common.ErrorCode.E_STORE_FAILED: {
        Language.L_EN: "Strore failed.",
    },
    common.ErrorCode.E_STORE_SEGMENT_ILLEGAL: {
        Language.L_EN: "Storage segment illegal.",
    },
    common.ErrorCode.E_BALANCER_RUNNING: {
        Language.L_EN: "Balancer is running.",
    },
    common.ErrorCode.E_BAD_BALANCE_PLAN: {
        Language.L_EN: "Bad balance plan.",
    },
    common.ErrorCode.E_BALANCED: {
        Language.L_EN: "Balance in progress.",
    },
    common.ErrorCode.E_NO_RUNNING_BALANCE_PLAN: {
        Language.L_EN: "No running blance paln.",
    },
    common.ErrorCode.E_CORRUPTTED_BALANCE_PLAN: {
        Language.L_EN: "Corruptted balance plan.",
    },
    common.ErrorCode.E_NO_VALID_HOST: {
        Language.L_EN: "No valid host .",
    },
    common.ErrorCode.E_INVALID_PASSWORD: {
        Language.L_EN: "Invalid password.",
    },
    common.ErrorCode.E_IMPROPER_ROLE: {
        Language.L_EN: "Improper password.",
    },
    common.ErrorCode.E_INVALID_PARTITION_NUM: {
        Language.L_EN: "Invalid parttion number.",
    },
    common.ErrorCode.E_INVALID_REPLICA_FACTOR: {
        Language.L_EN: "Invalid replica factor.",
    },
    common.ErrorCode.E_INVALID_CHARSET: {
        Language.L_EN: "Invalid charset.",
    },
    common.ErrorCode.E_INVALID_COLLATE: {
        Language.L_EN: "Invalid collate.",
    },
    common.ErrorCode.E_CHARSET_COLLATE_NOT_MATCH: {
        Language.L_EN: "Charset and collate is not match.",
    },
    common.ErrorCode.E_SNAPSHOT_FAILED: {
        Language.L_EN: "Create snapshot failed.",
    },
    common.ErrorCode.E_BLOCK_WRITE_FAILED: {
        Language.L_EN: "Block write failed.",
    },
    common.ErrorCode.E_REBUILD_INDEX_FAILED: {
        Language.L_EN: "Rebuild index failed.",
    },
    common.ErrorCode.E_INDEX_WITH_TTL: {
        Language.L_EN: "Index with ttl.",
    },
    common.ErrorCode.E_ADD_JOB_FAILED: {
        Language.L_EN: "Add job failed.",
    },
    common.ErrorCode.E_STOP_JOB_FAILED: {
        Language.L_EN: "Stop job failed.",
    },
    common.ErrorCode.E_SAVE_JOB_FAILED: {
        Language.L_EN: "Save job failed.",
    },
    common.ErrorCode.E_KEY_HAS_EXISTS: {
        Language.L_EN: "Key has exists.",
    },
    common.ErrorCode.E_PART_NOT_FOUND: {
        Language.L_EN: "Part not found.",
    },
    common.ErrorCode.E_KEY_NOT_FOUND: {
        Language.L_EN: "Key not found.",
    },
    common.ErrorCode.E_CONSENSUS_ERROR: {
        Language.L_EN: "Consensus error.",
    },
    common.ErrorCode.E_DATA_TYPE_MISMATCH: {
        Language.L_EN: "Data type not match.",
    },
    common.ErrorCode.E_INVALID_FIELD_VALUE: {
        Language.L_EN: "Invalid field value.",
    },
    common.ErrorCode.E_INVALID_OPERATION: {
        Language.L_EN: "Invalid operation.",
    },
    common.ErrorCode.E_NOT_NULLABLE: {
        Language.L_EN: "Field not nullable.",
    },
    common.ErrorCode.E_FIELD_UNSET: {
        Language.L_EN: "Field not set value.",
    },
    common.ErrorCode.E_OUT_OF_RANGE: {
        Language.L_EN: "Field out of range.",
    },
    common.ErrorCode.E_ATOMIC_OP_FAILED: {
        Language.L_EN: "Storage op failed.",
    },
    common.ErrorCode.E_IMPROPER_DATA_TYPE: {
        Language.L_EN: "Improper data type.",
    },
    common.ErrorCode.E_INVALID_SPACEVIDLEN: {
        Language.L_EN: "Invalid vid length.",
    },
    common.ErrorCode.E_INVALID_FILTER: {
        Language.L_EN: "Invalid filter.",
    },
    common.ErrorCode.E_INVALID_UPDATER: {
        Language.L_EN: "Invalid update item.",
    },
    common.ErrorCode.E_INVALID_STORE: {
        Language.L_EN: "Invalid store.",
    },
    common.ErrorCode.E_INVALID_PEER: {
        Language.L_EN: "Invalid peer.",
    },
    common.ErrorCode.E_RETRY_EXHAUSTED: {
        Language.L_EN: "Reach the maximum number of retries for storage.",
    },
    common.ErrorCode.E_TRANSFER_LEADER_FAILED: {
        Language.L_EN: "Transfer leader failed.",
    },
    common.ErrorCode.E_INVALID_STAT_TYPE: {
        Language.L_EN: "Invalid stat type.",
    },
    common.ErrorCode.E_FAILED_TO_CHECKPOINT: {
        Language.L_EN: "Checkpoint failed.",
    },
    common.ErrorCode.E_CHECKPOINT_BLOCKED: {
        Language.L_EN: "Checkpoint blocked.",
    },
    common.ErrorCode.E_FILTER_OUT: {
        Language.L_EN: "Update filter out.",
    },
    common.ErrorCode.E_INVALID_DATA: {
        Language.L_EN: "Invalid data.",
    },
    common.ErrorCode.E_INVALID_TASK_PARA: {
        Language.L_EN: "Invalid task param.",
    },
    common.ErrorCode.E_USER_CANCEL: {
        Language.L_EN: "User cancel.",
    },
    common.ErrorCode.E_SPACE_NOT_FOUND: {
        Language.L_EN: "Space not found.",
    },
    common.ErrorCode.E_TAG_NOT_FOUND: {
        Language.L_EN: "Tag not found.",
    },
    common.ErrorCode.E_EDGE_NOT_FOUND: {
        Language.L_EN: "Edge not found.",
    },
    common.ErrorCode.E_INDEX_NOT_FOUND: {
        Language.L_EN: "Index not found.",
    },
    common.ErrorCode.E_TAG_PROP_NOT_FOUND: {
        Language.L_EN: "Tag prop not found.",
    },
    common.ErrorCode.E_EDGE_PROP_NOT_FOUND: {
        Language.L_EN: "Edge prop not found.",
    },
    common.ErrorCode.E_INVALID_VID: {
        Language.L_EN: "Invalid vid.",
    },
    common.ErrorCode.E_INTERNAL_ERROR: {
        Language.L_EN: "Internal error `%s'.",
    },
}


