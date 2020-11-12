/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_GRAPH_RESPONSE_H
#define COMMON_GRAPH_RESPONSE_H

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

#include "common/datatypes/DataSet.h"

namespace nebula {

enum class ErrorCode {
    SUCCEEDED = 0,
    E_DISCONNECTED = -1,
    E_FAIL_TO_CONNECT = -2,
    E_RPC_FAILURE = -3,
    E_BAD_USERNAME_PASSWORD = -4,
    E_SESSION_INVALID = -5,
    E_SESSION_TIMEOUT = -6,
    E_SYNTAX_ERROR = -7,
    E_EXECUTION_ERROR = -8,
    E_STATEMENT_EMTPY = -9,
    E_USER_NOT_FOUND = -10,
    E_BAD_PERMISSION = -11,
    E_SEMANTIC_ERROR = -12,
};

template <typename T>
bool inline checkPointer(const T *lhs, const T *rhs) {
    if (lhs == rhs) {
        return true;
    } else if (lhs != nullptr && rhs != nullptr) {
        return *lhs == *rhs;
    } else {
        return false;
    }
}

// TODO(shylock) use optional for optional in thrift instead of pointer

struct AuthResponse {
    void clear() {
        error_code = ErrorCode::SUCCEEDED;
        session_id = nullptr;
        error_msg = nullptr;
    }

    bool operator==(const AuthResponse &rhs) const {
        if (error_code != rhs.error_code) {
            return false;
        }
        if (!checkPointer(session_id.get(), rhs.session_id.get())) {
            return false;
        }
        return checkPointer(error_msg.get(), rhs.error_msg.get());
    }

    ErrorCode error_code{ErrorCode::SUCCEEDED};
    std::unique_ptr<int64_t> session_id{nullptr};
    std::unique_ptr<std::string> error_msg{nullptr};
};


struct ProfilingStats {
    void clear() {
        rows = 0;
        exec_duration_in_us = 0;
        total_duration_in_us = 0;
        other_stats = nullptr;
    }

    bool operator==(const ProfilingStats &rhs) const {
        if (rows != rhs.rows) {
            return false;
        }
        if (exec_duration_in_us != rhs.exec_duration_in_us) {
            return false;
        }
        if (total_duration_in_us != rhs.total_duration_in_us) {
            return false;
        }
        return checkPointer(other_stats.get(), rhs.other_stats.get());
    }

    // How many rows being processed in an executor.
    int64_t rows{0};
    // Duration spent in an executor.
    int64_t exec_duration_in_us{0};
    // Total duration spent in an executor, contains schedule time
    int64_t total_duration_in_us{0};
    // Other profiling stats data map
    std::unique_ptr<std::unordered_map<std::string, std::string>> other_stats;
};

// The info used for select/loop.
struct PlanNodeBranchInfo {
    void clear() {
        is_do_branch = false;
        condition_node_id = -1;
    }

    bool operator==(const PlanNodeBranchInfo &rhs) const {
        return is_do_branch == rhs.is_do_branch && condition_node_id == rhs.condition_node_id;
    }

    // True if loop body or then branch of select
    bool  is_do_branch{0};
    // select/loop node id
    int64_t  condition_node_id{-1};
};

struct Pair {
    void clear() {
        key.clear();
        value.clear();
    }

    bool operator==(const Pair &rhs) const {
        return key == rhs.key && value == rhs.value;
    }

    std::string key;
    std::string value;
};

struct PlanNodeDescription {
    void clear() {
        name.clear();
        id = -1;
        output_var.clear();
        description = nullptr;
        profiles = nullptr;
        branch_info = nullptr;
        dependencies = nullptr;
    }

    bool operator==(const PlanNodeDescription &rhs) const;

    std::string                                   name;
    int64_t                                       id{-1};
    std::string                                   output_var;
    // other description of an executor
    std::unique_ptr<std::vector<Pair>>            description{nullptr};
    // If an executor would be executed multi times,
    // the profiling statistics should be multi-versioned.
    std::unique_ptr<std::vector<ProfilingStats>>   profiles{nullptr};
    std::unique_ptr<PlanNodeBranchInfo>            branch_info{nullptr};
    std::unique_ptr<std::vector<int64_t>>          dependencies{nullptr};
};

struct PlanDescription {
    void clear() {
        plan_node_descs.clear();
        node_index_map.clear();
        format.clear();
    }

    bool operator==(const PlanDescription &rhs) const {
        return plan_node_descs == rhs.plan_node_descs &&
            node_index_map == rhs.node_index_map &&
            format == rhs.format;
    }

    std::vector<PlanNodeDescription>     plan_node_descs;
    // map from node id to index of list
    std::unordered_map<int64_t, int64_t> node_index_map;
    // the print format of exec plan, lowercase string like `dot'
    std::string                          format;
};

struct ExecutionResponse {
    void clear() {
        error_code = ErrorCode::SUCCEEDED;
        latency_in_us = 0;
        data = nullptr;
        space_name = nullptr;
        error_msg = nullptr;
        plan_desc = nullptr;
        comment = nullptr;
    }

    bool operator==(const ExecutionResponse &rhs) const {
        if (error_code != rhs.error_code) {
            return false;
        }
        if (latency_in_us != rhs.latency_in_us) {
            return false;
        }
        if (!checkPointer(data.get(), rhs.data.get())) {
            return false;
        }
        if (!checkPointer(space_name.get(), rhs.space_name.get())) {
            return false;
        }
        if (!checkPointer(error_msg.get(), rhs.error_msg.get())) {
            return false;
        }
        if (!checkPointer(plan_desc.get(), rhs.plan_desc.get())) {
            return false;
        }
        return checkPointer(comment.get(), rhs.comment.get());
    }

    ErrorCode error_code{ErrorCode::SUCCEEDED};
    int32_t latency_in_us{0};
    std::unique_ptr<nebula::DataSet> data{nullptr};
    std::unique_ptr<std::string> space_name{nullptr};
    std::unique_ptr<std::string> error_msg{nullptr};
    std::unique_ptr<PlanDescription> plan_desc{nullptr};
    std::unique_ptr<std::string> comment{nullptr};
};

}   // namespace nebula

#endif  // COMMON_GRAPH_RESPONSE_H
