/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_ERRORCODE_ERRORMSG_H
#define COMMON_ERRORCODE_ERRORMSG_H

#include <unordered_map>

#include "common/errorcode/ErrorCode.h"

namespace nebula {

enum class Language : uint8_t {
    L_EN = 1,
};

static const std::unordered_map<ErrorCode, std::unordered_map<Language, std::string>>
ErrorMsgUTF8Map = {
    {
        ErrorCode::SUCCEEDED,
        {
            {
                Language::L_EN,
                "Succeeded."
            }
        }
    },
    {
        ErrorCode::E_RPC_FAILED,
        {
            {
                Language::L_EN,
                "Rpc failed in `%s': %s."
            }
        }
    },
    {
        ErrorCode::E_GRAPH_WAIT_FOR_METAD_READY,
        {
            {
                Language::L_EN,
                "Metad is not ready."
            }
        }
    },
    {
        ErrorCode::E_REGISTER_SIGNAL_ERROR,
        {
            {
                Language::L_EN,
                "Register signal %d failed: %s."
            }
        }
    },
    {
        ErrorCode::E_SYSTEM_CALL_FAILED,
        {
            {
                Language::L_EN,
                "Fail to call filesystem function: `%s'."
            }
        }
    },
    {
        ErrorCode::E_FILE_NOT_FOUND,
        {
            {
                Language::L_EN,
                "File `%s' not found."
            }
        }
    },
    {
        ErrorCode::E_FILE_NOT_PERMISSION,
        {
            {
                Language::L_EN,
                "No permission to read file `%s'."
            }
        }
    },
    {
        ErrorCode::E_FILE_CONTENT_IS_EMPTY,
        {
            {
                Language::L_EN,
                "File `%s' is empty content."
            }
        }
    },
    {
        ErrorCode::E_NET_GET_IPV4_FAILED,
        {
            {
                Language::L_EN,
                "No IPv4 address found for `%s'."
            }
        }
    },
    {
        ErrorCode::E_NET_HOST_NOT_FOUND,
        {
            {
                Language::L_EN,
                "Host `%s' not found."
            }
        }
    },
    {
        ErrorCode::E_NET_BAD_IP,
        {
            {
                Language::L_EN,
                "The ip format `'%s is bad."
            }
        }
    },
    {
        ErrorCode::E_PROCESS_RUN_COMMAND_FAILED,
        {
            {
                Language::L_EN,
                "Failed to execute the command `%s'."
            }
        }
    },
    {
        ErrorCode::E_PROCESS_READ_COMMAND_RESULT_FAILED,
        {
            {
                Language::L_EN,
                "Failed to read the output of the command `%s'."
            }
        }
    },
    {
        ErrorCode::E_PROCESS_NOT_AVAILABLE_PID,
        {
            {
                Language::L_EN,
                "The pid is not available: %s."
            }
        }
    },
    {
        ErrorCode::E_PROCESS_CREATE_PID_FAILED,
        {
            {
                Language::L_EN,
                "Failed to create pid file: %s."
            }
        }
    },
    {
        ErrorCode::E_STATS_INVALID_NAME,
        {
            {
                Language::L_EN,
                "Invalid stats name: %s."
            }
        }
    },
    {
        ErrorCode::E_NET_BAD_PORT,
        {
            {
                    Language::L_EN,
                    "The port format `'%s is bad."
            }
        }
    },
    {
        ErrorCode::E_SYSTEM_CALL_FAILED,
        {
            {
                Language::L_EN,
                "Do system call `%s' failed: %s."
            }
        }
    },
    {
        ErrorCode::E_CONFIG_ITEM_NOT_FOUND,
        {
            {
                Language::L_EN,
                "Config item `%s' not found."
            }
        }
    },
    {
        ErrorCode::E_CONFIG_ITEM_IS_NO_ARRAY,
        {
            {
                Language::L_EN,
                "Config item `%s' not an array."
            }
        }
    },
    {
        ErrorCode::E_CONFIG_INVALID_ITEM_TYPE,
        {
            {
                Language::L_EN,
                "Config item `%s' is not a `%s'."
            }
        }
    },
    {
        ErrorCode::E_CONFIG_WRONG_JSON_FORMAT,
        {
            {
                Language::L_EN,
                "Config Item `%s' is not an JSON object."
            }
        }
    },
    {
        ErrorCode::E_INVALID_TIME_FORMAT,
        {
            {
                Language::L_EN,
                "Invalid time format: %s."
            }
        }
    },
    {
        ErrorCode::E_INVALID_DATA_TYPE,
        {
            {
                Language::L_EN,
                "Invalid data type: %s."
            }
        }
    },
    {
        ErrorCode::E_EMPTY_TIMEZONE,
        {
            {
                Language::L_EN,
                "Empty timezone are not allowed."
            }
        }
    },
    {
        ErrorCode::E_CHARSET_NOT_SUPPORTED,
        {
            {
                Language::L_EN,
                "Charset `%s' is not supported."
            }
        }
    },
    {
        ErrorCode::E_COLLATION_NOT_SUPPORTED,
        {
            {
                Language::L_EN,
                "Collation `%s' is not supported."
            }
        }
    },
    {
        ErrorCode::E_CHARSET_AND_COLLATION_NOT_MATCH,
        {
            {
                Language::L_EN,
                "Charset `%s' and Collation `%s' is not match."
            }
        }
    },
    {
        ErrorCode::E_FUNCTION_NOT_SUPPORTED,
        {
            {
                Language::L_EN,
                "Function `%s' is not supported."
            }
        }
    },
    {
        ErrorCode::E_FUNCTION_DYNAMIC_NOT_SUPPORTED,
        {
            {
                Language::L_EN,
                "Dynamic function `%s' is not supported."
            }
        }
    },
    {
        ErrorCode::E_FUNCTION_ARITY_NOT_MATCH,
        {
            {
                Language::L_EN,
                "Arity not match for function `%s': provided %lu but %lu expected."
            }
        }
    },
    {
        ErrorCode::E_FUNCTION_INVALID_ARG_TYPE,
        {
            {
                Language::L_EN,
                "Invalid args for function `%s'."
            }
        }
    },
    {
        ErrorCode::E_AGG_FUNCTION_NOT_SUPPORTED,
        {
            {
                Language::L_EN,
                "Aggregation function `%s' is not supported."
            }
        }
    },
    {
        ErrorCode::E_AGG_FUNCTION_DYNAMIC_NOT_SUPPORTED,
        {
            {
                Language::L_EN,
                "Aggregation Dynamic function `%s' is not supported."
            }
        }
    },
    {
        ErrorCode::E_SPACE_INVALID_VID_TYPE,
        {
            {
                Language::L_EN,
                "Only support integer/string type vid, but now type is `%s'."
            }
        }
    },
    {
        ErrorCode::E_TTL_NOT_SET,
        {
            {
                Language::L_EN,
                "The TTL column is not set."
            }
        }
    },
    {
        ErrorCode::E_TTL_INVALID_PROPERTY,
        {
            {
                Language::L_EN,
                "The TTL column name is empty or the TTL time is invalid."
            }
        }
    },
    {
        ErrorCode::E_INDEX_SCHEMA_NAME_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The schema name `%s' is not found."
            }
        }
    },
    {
        ErrorCode::E_FULLTEXT_CLIENT_IS_EMPTY,
        {
            {
                Language::L_EN,
                "The fulltext client list is empty."
            }
        }
    },
    {
        ErrorCode::E_GRAPH_SCHEMA_TAG_ID_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The tag id `%d' does not exist or it is not synchronized to the graph yet. "
                "Double check the tag name and try again later."
            }
        }
    },
    {
        ErrorCode::E_GRAPH_SCHEMA_EDGE_ID_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The edge id `%d' does not exist or it is not synchronized to the graph yet. "
                "Double check the edge name and try again later."
            }
        }
    },
    {
        ErrorCode::E_GRAPH_SCHEMA_TAG_NAME_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The tag name `%s' does not exist or it is not synchronized to the graph yet. "
                "Double check the tag name and try again later."
            }
        }
    },
    {
        ErrorCode::E_GRAPH_SCHEMA_EDGE_NAME_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The edge name `%s' does not exist or it is not synchronized to the graph yet. "
                "Double check the edge name and try again later."
            }
        }
    },
    {
        ErrorCode::E_GRAPH_INDEX_TAG_INDEX_ID_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The tag index id `%d' does not exist or it is not synchronized to the graph yet. "
                "Double check the tag index name and try again later."
            }
        }
    },
    {
        ErrorCode::E_GRAPH_INDEX_EDGE_INDEX_ID_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The edge index id `%d' does not exist or it is not synchronized to the graph yet."
                "Double check the edge index name and try again later."
            }
        }
    },
    {
        ErrorCode::E_GRAPH_INDEX_TAG_INDEX_NAME_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The tag index name `%s' does not exist "
                "or it is not synchronized to the graph yet. "
                "Double check the tag index name and try again later."
            }
        }
    },
    {
        ErrorCode::E_GRAPH_INDEX_EDGE_INDEX_NAME_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The edge index name `%s' does not exist "
                "or it is not synchronized to the graph yet."
                "Double check the edge index name and try again later."
            }
        }
    },
    {
        ErrorCode::E_GRAPH_ES_RUN_COMMAND_FAILED,
        {
            {
                Language::L_EN,
                "Graph es run command `%s' failed."
            }
        }
    },
    {
        ErrorCode::E_META_SCHEMA_TAG_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The tag `%s' does not exist in metad."
            }
        }
    },
    {
        ErrorCode::E_META_SCHEMA_EDGE_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The edge `%s' does not exist in metad."
            }
        }
    },
    {
        ErrorCode::E_META_SCHEMA_TAG_INDEX_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The tag index `%s' does not exist in metad."
            }
        }
    },
    {
        ErrorCode::E_META_SCHEMA_EDGE_INDEX_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The edge index `%s' does not exist in metad."
            }
        }
    },
    {
        ErrorCode::E_STORAGE_SCHEMA_TAG_ID_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The tag id `%d' does not exist or it is not synchronized to the storage yet. "
                "Double check the tag name and try again later."
            }
        }
    },
    {
        ErrorCode::E_STORAGE_SCHEMA_EDGE_ID_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The edge id `%d' does not exist or it is not synchronized to the storage yet. "
                "Double check the edge name and try again later."
            }
        }
    },
    {
        ErrorCode::E_STORAGE_SCHEMA_TAG_NAME_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The tag name `%s' does not exist or it is not synchronized to the storage yet. "
                "Double check the tag name and try again later."
            }
        }
    },
    {
        ErrorCode::E_STORAGE_SCHEMA_EDGE_NAME_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The edge name `%s' does not exist or it is not synchronized to the storage yet. "
                "Double check the edge name and try again later."
            }
        }
    },
    {
        ErrorCode::E_STORAGE_INDEX_TAG_INDEX_ID_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The tag index id `%d' does not exist "
                "or it is not synchronized to the storage yet. "
                "Double check the tag index name and try again later." }
        }
    },
    {
        ErrorCode::E_STORAGE_INDEX_EDGE_INDEX_ID_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The edge index id `%d' does not exist "
                "or it is not synchronized to the storage yet. "
                "Double check the edge index name and try again later."
            }
        }
    },
    {
        ErrorCode::E_STORAGE_INDEX_TAG_INDEX_NAME_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The tag index name `%d' does not exist "
                "or it is not synchronized to the storage yet. "
                "Double check the tag index name and try again later."
            }
        }
    },
    {
        ErrorCode::E_STORAGE_INDEX_EDGE_INDEX_NAME_NOT_FOUND,
        {
            {
                Language::L_EN,
                "The edge index name `%d' does not exist "
                "or it is not synchronized to the storage yet. "
                "Double check the edge index name and try again later."
            }
        }
    },
    {
        ErrorCode::E_STORAGE_PART_ID_NOT_FOUND,
        {
            {
                Language::L_EN,
                "Part not found in cache, spaceId: %d, partId: %d "
                "or it is not synchronized to the storage yet. "
                "Double check the edge index name and try again later."
            }
        }
    },

    {
        ErrorCode::E_STORAGE_ES_RUN_COMMAND_FAILED,
        {
            {
                Language::L_EN,
                "Storage es run command `%s' failed."
            }
        }
    },
    {
        ErrorCode::E_INTERNAL_ERROR,
        {
            {
                Language::L_EN,
                "Internal error: %s."
            }
        }
    },
    {
        ErrorCode::E_UNSUPPORTED,
        {
            {
                Language::L_EN,
                "Unsupported: %s."
            }
        }
    },
};

}   // namespace nebula

#endif  // COMMON_ERRORCODE_ERRORMSG_H
