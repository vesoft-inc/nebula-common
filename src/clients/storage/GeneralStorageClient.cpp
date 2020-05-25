/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "clients/storage/GeneralStorageClient.h"

#include "base/Base.h"

namespace nebula {
namespace storage {

folly::SemiFuture<StorageRpcResponse<cpp2::KVGetResponse>> GeneralStorageClient::get(
    GraphSpaceID               space,
    std::vector<std::string>&& keys,
    bool                       returnPartly,
    folly::EventBase*          evb) {
    auto status = clusterIdsToHosts(
        space, std::move(keys), [](const std::string& v) -> const std::string& { return v; });

    if (!status.ok()) {
        return folly::makeFuture<StorageRpcResponse<cpp2::KVGetResponse>>(
            std::runtime_error(status.status().toString()));
    }

    auto&                                            clusters = status.value();
    std::unordered_map<HostAddr, cpp2::KVGetRequest> requests;
    for (auto& c : clusters) {
        auto& host = c.first;
        auto& req  = requests[host];
        req.set_space_id(space);
        req.set_parts(std::move(c.second));
        req.set_return_partly(returnPartly);
    }

    return collectResponse(
        evb,
        std::move(requests),
        [](cpp2::GeneralStorageServiceAsyncClient* client, const cpp2::KVGetRequest& r) {
            return client->future_get(r);
        },
        [](const std::pair<const PartitionID, std::vector<std::string>>& p) { return p.first; });
}

folly::SemiFuture<StorageRpcResponse<cpp2::ExecResponse>>
GeneralStorageClient::put(GraphSpaceID space, std::vector<KeyValue> kvs, folly::EventBase* evb) {
    auto status = clusterIdsToHosts(
        space, std::move(kvs), [](const KeyValue& v) -> const std::string& { return v.key; });

    if (!status.ok()) {
        return folly::makeFuture<StorageRpcResponse<cpp2::ExecResponse>>(
            std::runtime_error(status.status().toString()));
    }

    auto&                                            clusters = status.value();
    std::unordered_map<HostAddr, cpp2::KVPutRequest> requests;
    for (auto& c : clusters) {
        auto& host = c.first;
        auto& req  = requests[host];
        req.set_space_id(space);
        req.set_parts(std::move(c.second));
    }

    return collectResponse(
        evb,
        std::move(requests),
        [](cpp2::GeneralStorageServiceAsyncClient* client, const cpp2::KVPutRequest& r) {
            return client->future_put(r);
        },
        [](const std::pair<const PartitionID, std::vector<KeyValue>>& p) { return p.first; });
}

folly::SemiFuture<StorageRpcResponse<cpp2::ExecResponse>> GeneralStorageClient::remove(
    GraphSpaceID             space,
    std::vector<std::string> keys,
    folly::EventBase*        evb) {
    auto status = clusterIdsToHosts(
        space, std::move(keys), [](const std::string& v) -> const std::string& { return v; });

    if (!status.ok()) {
        return folly::makeFuture<StorageRpcResponse<cpp2::ExecResponse>>(
            std::runtime_error(status.status().toString()));
    }

    auto&                                               clusters = status.value();
    std::unordered_map<HostAddr, cpp2::KVRemoveRequest> requests;
    for (auto& c : clusters) {
        auto& host = c.first;
        auto& req  = requests[host];
        req.set_space_id(space);
        req.set_parts(std::move(c.second));
    }

    return collectResponse(
        evb,
        std::move(requests),
        [](cpp2::GeneralStorageServiceAsyncClient* client, const cpp2::KVRemoveRequest& r) {
            return client->future_remove(r);
        },
        [](const std::pair<const PartitionID, std::vector<std::string>>& p) { return p.first; });
}

}  // namespace storage
}  // namespace nebula
