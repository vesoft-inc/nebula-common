/* vim: ft=proto
 * Copyright (c) 2020 vesoft inc. All rights reserved.
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
        Language.L_EN: "The leader of `%s' has changed. Try again later",
    },
    common.ErrorCode.E_USERNAME_NOT_FOUND: {
        Language.L_EN: "The username does not exist. Double check and try again.",
    },
    common.ErrorCode.E_SESSION_NOT_EXIST: {
        Language.L_EN: "Session id `%ld' is not exist",
    },
    common.ErrorCode.E_SESSION_EXPIRED: {
        Language.L_EN: "Session id `%ld' has expired",
    },
    common.ErrorCode.E_SYNTAX_ERROR: {
        Language.L_EN: "SyntaxError: %s.",
    },
    common.ErrorCode.E_SEMANTIC_ERROR: {
        Language.L_EN: "SemanticError: %s.",
    },
    common.ErrorCode.E_NOT_USE_SPACE: {
        Language.L_EN: "No graph space is specified. Choose a graph space with `USE spaceName' firstly.",
    },
    common.ErrorCode.E_INVALID_RANK: {
        Language.L_EN: "The rank value is not recognized. It must be a 64-bit integer.",
    },
    common.ErrorCode.E_INVALID_EDGE_TYPE: {
        Language.L_EN: "The edge type value is not recognized.",
    },
    common.ErrorCode.E_OUT_OF_LEN_OF_USERNAME: {
        Language.L_EN: "The user name is too long. It cannot be longer than 16 characters.",
    },
    common.ErrorCode.E_OUT_OF_LEN_OF_PASSWORD: {
        Language.L_EN: "The password is too long. It cannot be longer than 24 characters.",
    },
    common.ErrorCode.E_DUPLICATE_COLUMN_NAME: {
        Language.L_EN: "Duplicate column name `%s'.",
    },
    common.ErrorCode.E_COLUMN_COUNT_NOT_MATCH: {
        Language.L_EN: "Column count doesn't match value count.",
    },
    common.ErrorCode.E_COLUMN_NOT_FOUND: {
        Language.L_EN: "Column name `%s' was not found.",
    },
    common.ErrorCode.E_UNSUPPORTED_EXPR: {
        Language.L_EN: "Expression `%s' is not supported here.",
    },
    common.ErrorCode.E_INVALID_AUTH_TYPE: {
        Language.L_EN: "The auth type `%s' is not supported here. Check the configuration `auth_type'.",
    },
    common.ErrorCode.E_OUT_OF_MAX_STATEMENTS: {
        Language.L_EN: "The maximum number of statements is reached. Execute less than 1000 statements a time.",
    },
    common.ErrorCode.E_PERMISSION_DENIED: {
        Language.L_EN: "The role of the user does not have the privilege to do this. Make sure the user is set as the correct role.",
    },
    common.ErrorCode.E_NO_TAGS: {
        Language.L_EN: "No tag exists in the space.",
    },
    common.ErrorCode.E_NO_EDGES: {
        Language.L_EN: "No edge exists in the space.",
    },
    common.ErrorCode.E_INVALID_EXPR: {
        Language.L_EN: "Expression `%s' is not valid.",
    },
    common.ErrorCode.E_NO_HOSTS: {
        Language.L_EN: "There are not enough hosts available.",
    },
    common.ErrorCode.E_SPACE_EXISTED: {
        Language.L_EN: "Space name `%s' is already in use. Try another name.",
    },
    common.ErrorCode.E_TAG_EXISTED: {
        Language.L_EN: "Tag name `%s' is already in use. Try another name.",
    },
    common.ErrorCode.E_EDGE_EXISTED: {
        Language.L_EN: "Edge name `%s' is already in use. Try another name.",
    },
    common.ErrorCode.E_INDEX_EXISTED: {
        Language.L_EN: "Index name `%s' is already in use. Try another name.",
    },
    common.ErrorCode.E_UNSUPPORTED: {
        Language.L_EN: "It is not supported yet. You may provide a feedback on our forum or GitHub page.",
    },
    common.ErrorCode.E_NOT_DROP_PROP: {
        Language.L_EN: "The property with an index or TLL value cannot be removed. To do this, remove the index or TTL value first.",
    },
    common.ErrorCode.E_CONFIG_IMMUTABLE: {
        Language.L_EN: "This configuration `%s' cannot be modified.",
    },
    common.ErrorCode.E_CONFLICT: {
        Language.L_EN: "`%s' conflits with the name of other tags, edges, or indexes.",
    },
    common.ErrorCode.E_INVALID_PARAM: {
        Language.L_EN: "Parameter `%s' is not valid.",
    },
    common.ErrorCode.E_STORE_FAILED: {
        Language.L_EN: "Unable to write data into the storage. Check if the disk is full. If it is not, go to our forum or GitHub page for help.",
    },
    common.ErrorCode.E_STORE_SEGMENT_ILLEGAL: {
        Language.L_EN: "Unable to write data into the storage segment. Check if the disk is full. If it is not, go to our forum or GitHub page for help.",
    },
    common.ErrorCode.E_BALANCER_RUNNING: {
        Language.L_EN: "The balance plan already exists. You cannot run it again.",
    },
    common.ErrorCode.E_BAD_BALANCE_PLAN: {
        Language.L_EN: "Unable to read the balance plan.",
    },
    common.ErrorCode.E_BALANCED: {
        Language.L_EN: "The cluster is already balanced.",
    },
    common.ErrorCode.E_NO_RUNNING_BALANCE_PLAN: {
        Language.L_EN: "There is no balance plan running.",
    },
    common.ErrorCode.E_NO_VALID_HOST: {
        Language.L_EN: "No valid host .",
    },
    common.ErrorCode.E_INVALID_PASSWORD: {
        Language.L_EN: "The password does not match the old one.",
    },
    common.ErrorCode.E_IMPROPER_ROLE: {
        Language.L_EN: "The role of the user do not have the privilege to use the GRANT ROLE command.",
    },
    common.ErrorCode.E_INVALID_PARTITION_NUM: {
        Language.L_EN: "The partition number is out of range. It must a positive integer.",
    },
    common.ErrorCode.E_INVALID_REPLICA_FACTOR: {
        Language.L_EN: "The number of replicas cannot be larger than the number of hosts.",
    },
    common.ErrorCode.E_INVALID_CHARSET: {
        Language.L_EN: "This type of charset is not supported. You may use utf8.",
    },
    common.ErrorCode.E_INVALID_COLLATE: {
        Language.L_EN: "This type of collate is not supported. You may use utf8_bin.",
    },
    common.ErrorCode.E_CHARSET_COLLATE_NOT_MATCH: {
        Language.L_EN: "The charset and collate does not match. You may use utf8 and utf8_bin.",
    },
    common.ErrorCode.E_SNAPSHOT_FAILED: {
        Language.L_EN: "Cannot create a snapshot. You may go to our forum or GitHub page for help.",
    },
    common.ErrorCode.E_BLOCK_WRITE_FAILED: {
        Language.L_EN: "Block write failed when creating a snapshot. You may check the storaged is ok or go to our forum or GitHub page for help.",
    },
    common.ErrorCode.E_REBUILD_INDEX_FAILED: {
        Language.L_EN: "Cannot rebuild the index. You may go to our forum or GitHub page for help.",
    },
    common.ErrorCode.E_INDEX_WITH_TTL: {
        Language.L_EN: "Cannot create an index on a property with a TTL value. To do this, remove the TTL value firstly.",
    },
    common.ErrorCode.E_ADD_JOB_FAILED: {
        Language.L_EN: "Unable to store the job information. You may go to our forum or GitHub page for help.",
    },
    common.ErrorCode.E_STOP_JOB_FAILED: {
        Language.L_EN: "Unable to stop the job. Check if the job ID is correct. If it is, check if the disk is working properly. If it is, go to our forum or GitHub page for help.",
    },
    common.ErrorCode.E_SAVE_JOB_FAILED: {
        Language.L_EN: "Cannot start the job. You may go to our forum or GitHub page for help.",
    },
    common.ErrorCode.E_PART_NOT_FOUND: {
        Language.L_EN: "Cannot find the partion number on the host. You may go to our forum or GitHub page for help.",
    },
    common.ErrorCode.E_KEY_NOT_FOUND: {
        Language.L_EN: "Key not found.",
    },
    common.ErrorCode.E_CONSENSUS_ERROR: {
        Language.L_EN: "The storage engine is fully occupied and cannot deal with more requests for now. Limit the request frequency and try again later.",
    },
    common.ErrorCode.E_DATA_TYPE_MISMATCH: {
        Language.L_EN: "The data type does not meet the requirements. Use the correct type of data.",
    },
    common.ErrorCode.E_INVALID_FIELD_VALUE: {
        Language.L_EN: "Invalid field value.",
    },
    common.ErrorCode.E_INVALID_OPERATION: {
        Language.L_EN: "Sorry. A compatibility issue has occurred. You may go to our forum or GitHub page for help.",
    },
    common.ErrorCode.E_NOT_NULLABLE: {
        Language.L_EN: "This field cannot be null. Set a valid value for the field.",
    },
    common.ErrorCode.E_OUT_OF_RANGE: {
        Language.L_EN: "The timestamp value is out of range. It must be between 0 and 9223372036.",
    },
    common.ErrorCode.E_ATOMIC_OP_FAILED: {
        Language.L_EN: "Incomplete atomic operation. You may go to our forum or GitHub page for help.",
    },
    common.ErrorCode.E_IMPROPER_DATA_TYPE: {
        Language.L_EN: "Improper data type.",
    },
    common.ErrorCode.E_INVALID_SPACEVIDLEN: {
        Language.L_EN: "The VID length is out of range.",
    },
    common.ErrorCode.E_INVALID_FILTER: {
        Language.L_EN: "The filter condition is not supported. Double check your command or go to our forum or GitHub page for help.",
    },
    common.ErrorCode.E_INVALID_FIELD: {
        Language.L_EN: "The property field is not supported. Double check the field your entered and try again.",
    },
    common.ErrorCode.E_INVALID_RETURN: {
        Language.L_EN: "The return field is not supported. Double check the field your entered and try again.",
    },
    common.ErrorCode.E_INVALID_STORE: {
        Language.L_EN: "The storage engine is not supported.",
    },
    common.ErrorCode.E_INVALID_STAT_TYPE: {
        Language.L_EN: "There is a compatibility issue between the graphd and the storaged service. Make sure they have the same version.",
    },
    common.ErrorCode.E_FAILED_TO_CHECKPOINT: {
        Language.L_EN: "Unable to create the snapshot. Check the status of the network, the disk and the storage engine. You may also go to our forum or GitHub page for help.",
    },
    common.ErrorCode.E_CHECKPOINT_BLOCKED: {
        Language.L_EN: "Cannot write data while creating a snapshot. Try again later.",
    },
    common.ErrorCode.E_ILLEGAL_TTL_COLUMN_TYPE: {
        Language.L_EN: "The TTL value must be a timestamp or a 64-bit interger.",
    },
    common.ErrorCode.E_USER_CANCEL: {
        Language.L_EN: "User cancel.",
    },
    common.ErrorCode.E_SPACE_NOT_FOUND: {
        Language.L_EN: "The space does not exist or the it not synchronized to the storage yet. Double check the space name and try again later.",
    },
    common.ErrorCode.E_TAG_NOT_FOUND: {
        Language.L_EN: "The tag does not exist or it is not synchronized to the storage yet. Double check the tag name and try again later.",
    },
    common.ErrorCode.E_EDGE_NOT_FOUND: {
        Language.L_EN: "The edge does not exist or it is not synchronized to the storage yet. Double check the edge name and try again later.",
    },
    common.ErrorCode.E_INDEX_NOT_FOUND: {
        Language.L_EN: "The index does not exist or it is not synchronized to the storage yet. Double check the index name and try again later.",
    },
    common.ErrorCode.E_TAG_PROP_NOT_FOUND: {
        Language.L_EN: "The tag property does not exist or it is not synchronized to the storage yet. Double check the property name and try again later.",
    },
    common.ErrorCode.E_EDGE_PROP_NOT_FOUND: {
        Language.L_EN: "The edge property does not exist or it is not synchronized to the storage yet. Double check the property name and try again later.",
    },
    common.ErrorCode.E_INVALID_VID: {
        Language.L_EN: "The VID must be a 64-bit interger or a string.",
    },
    common.ErrorCode.E_INTERNAL_ERROR: {
        Language.L_EN: "InternalError: `%s'.",
    },
}


