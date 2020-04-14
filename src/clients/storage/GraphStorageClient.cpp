/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "base/Base.h"
#include "clients/storage/GraphStorageClient.h"

namespace nebula {
namespace storage {

folly::SemiFuture<StorageRpcResponse<cpp2::GetNeighborsResponse>>
GraphStorageClient::getNeighbors(GraphSpaceID space,
                                 std::vector<VertexID> vertices,
                                 const std::vector<EdgeType>& edgeTypes,
                                 const std::vector<cpp2::VertexProp>& vertexProps,
                                 const std::vector<cpp2::EdgeProp>& edgeProps,
                                 const std::vector<cpp2::StatProp>& statProps,
                                 const std::string &filter,
                                 folly::EventBase* evb) {
    auto status = clusterIdsToHosts(
        space, std::move(vertices), [](const VertexID& v) -> const VertexID& {
            return v;
        });

    if (!status.ok()) {
        return folly::makeFuture<StorageRpcResponse<cpp2::GetNeighborsResponse>>(
            std::runtime_error(status.status().toString()));
    }

    auto& clusters = status.value();
    std::unordered_map<HostAddr, cpp2::GetNeighborsRequest> requests;
    for (auto& c : clusters) {
        auto& host = c.first;
        auto& req = requests[host];
        req.set_space_id(space);
        req.set_parts(std::move(c.second));
        req.set_edge_types(edgeTypes);
        req.set_vertex_props(vertexProps);
        req.set_edge_props(edgeProps);
        req.set_stat_props(statProps);
        if (filter.size() > 0) {
            req.set_filter(filter);
        }
    }

    return collectResponse(
        evb, std::move(requests),
        [] (cpp2::GraphStorageServiceAsyncClient* client,
            const cpp2::GetNeighborsRequest& r) {
            return client->future_getNeighbors(r);
        },
        [] (const std::pair<const PartitionID, std::vector<VertexID>>& p) {
            return p.first;
        });
}


folly::SemiFuture<StorageRpcResponse<cpp2::ExecResponse>>
GraphStorageClient::addVertices(GraphSpaceID space,
                                std::vector<cpp2::NewVertex> vertices,
                                bool overwritable,
                                folly::EventBase* evb) {
    auto status = clusterIdsToHosts(space,
                                    std::move(vertices),
                                    [](const cpp2::NewVertex& v) -> const VertexID& {
        return v.get_id();
    });

    if (!status.ok()) {
        return folly::makeFuture<StorageRpcResponse<cpp2::ExecResponse>>(
            std::runtime_error(status.status().toString()));
    }

    auto& clusters = status.value();
    std::unordered_map<HostAddr, cpp2::AddVerticesRequest> requests;
    for (auto& c : clusters) {
        auto& host = c.first;
        auto& req = requests[host];
        req.set_space_id(space);
        req.set_overwritable(overwritable);
        req.set_parts(std::move(c.second));
    }

    VLOG(3) << "requests size " << requests.size();
    return collectResponse(
        evb,
        std::move(requests),
        [] (cpp2::GraphStorageServiceAsyncClient* client,
            const cpp2::AddVerticesRequest& r) {
            return client->future_addVertices(r);
        },
        [] (const std::pair<const PartitionID, std::vector<cpp2::NewVertex>>& p) {
            return p.first;
        });
}


folly::SemiFuture<StorageRpcResponse<cpp2::ExecResponse>>
GraphStorageClient::addEdges(GraphSpaceID space,
                             std::vector<cpp2::NewEdge> edges,
                             bool overwritable,
                             folly::EventBase* evb) {
    auto status = clusterIdsToHosts(space,
                                    std::move(edges),
                                    [](const cpp2::NewEdge& e) -> const VertexID& {
        return e.get_key().get_src();
    });

    if (!status.ok()) {
        return folly::makeFuture<StorageRpcResponse<cpp2::ExecResponse>>(
            std::runtime_error(status.status().toString()));
    }

    auto& clusters = status.value();
    std::unordered_map<HostAddr, cpp2::AddEdgesRequest> requests;
    for (auto& c : clusters) {
        auto& host = c.first;
        auto& req = requests[host];
        req.set_space_id(space);
        req.set_overwritable(overwritable);
        req.set_parts(std::move(c.second));
    }

    return collectResponse(
        evb,
        std::move(requests),
        [] (cpp2::GraphStorageServiceAsyncClient* client,
            const cpp2::AddEdgesRequest& r) {
            return client->future_addEdges(r);
        },
        [] (const std::pair<const PartitionID, std::vector<cpp2::NewEdge>>& p) {
            return p.first;
        });
}


folly::SemiFuture<StorageRpcResponse<cpp2::VertexPropResponse>>
GraphStorageClient::getVertexProps(GraphSpaceID space,
                                   std::vector<VertexID> vertices,
                                   const std::vector<cpp2::VertexProp> &props,
                                   const std::string &filter,
                                   folly::EventBase* evb) {
    auto status = clusterIdsToHosts(space,
                                    std::move(vertices),
                                    [](const VertexID& v) -> const VertexID& {
        return v;
    });

    if (!status.ok()) {
        return folly::makeFuture<StorageRpcResponse<cpp2::VertexPropResponse>>(
            std::runtime_error(status.status().toString()));
    }

    auto& clusters = status.value();
    std::unordered_map<HostAddr, cpp2::VertexPropRequest> requests;
    for (auto& c : clusters) {
        auto& host = c.first;
        auto& req = requests[host];
        req.set_space_id(space);
        req.set_parts(std::move(c.second));
        req.set_vertex_props(props);
        if (filter.size() > 0) {
            req.set_filter(filter);
        }
    }

    return collectResponse(
        evb,
        std::move(requests),
        [] (cpp2::GraphStorageServiceAsyncClient* client,
            const cpp2::VertexPropRequest& r) {
            return client->future_getVertexProps(r);
        },
        [] (const std::pair<const PartitionID, std::vector<VertexID>>& p) {
            return p.first;
        });
}


folly::SemiFuture<StorageRpcResponse<cpp2::EdgePropResponse>>
GraphStorageClient::getEdgeProps(GraphSpaceID space,
                                 std::vector<cpp2::EdgeKey> edges,
                                 const std::vector<cpp2::EdgeProp> &props,
                                 const std::string &filter,
                                 folly::EventBase* evb) {
    auto status = clusterIdsToHosts(space,
                                    std::move(edges),
                                    [](const cpp2::EdgeKey& v) -> const VertexID& {
        return v.get_src();
    });

    if (!status.ok()) {
        return folly::makeFuture<StorageRpcResponse<cpp2::EdgePropResponse>>(
            std::runtime_error(status.status().toString()));
    }

    auto& clusters = status.value();
    std::unordered_map<HostAddr, cpp2::EdgePropRequest> requests;
    for (auto& c : clusters) {
        auto& host = c.first;
        auto& req = requests[host];
        req.set_space_id(space);
        req.set_parts(std::move(c.second));
        req.set_edge_props(props);
        if (filter.size() > 0) {
            req.set_filter(filter);
        }
    }

    return collectResponse(
        evb, std::move(requests),
        [] (cpp2::GraphStorageServiceAsyncClient* client,
            const cpp2::EdgePropRequest& r) {
            return client->future_getEdgeProps(r);
        },
        [] (const std::pair<const PartitionID, std::vector<cpp2::EdgeKey>>& p) {
            return p.first;
        });
}


folly::SemiFuture<StorageRpcResponse<cpp2::ExecResponse>>
GraphStorageClient::deleteEdges(GraphSpaceID space,
                                std::vector<cpp2::EdgeKey> edges,
                                folly::EventBase* evb) {
    auto status = clusterIdsToHosts(space,
                                    std::move(edges),
                                    [](const cpp2::EdgeKey& v) -> const VertexID& {
        return v.get_src();
    });

    if (!status.ok()) {
        return folly::makeFuture<StorageRpcResponse<cpp2::ExecResponse>>(
            std::runtime_error(status.status().toString()));
    }

    auto& clusters = status.value();
    std::unordered_map<HostAddr, cpp2::DeleteEdgesRequest> requests;
    for (auto& c : clusters) {
        auto& host = c.first;
        auto& req = requests[host];
        req.set_space_id(space);
        req.set_parts(std::move(c.second));
    }

    return collectResponse(
        evb,
        std::move(requests),
        [] (cpp2::GraphStorageServiceAsyncClient* client,
            const cpp2::DeleteEdgesRequest& r) {
            return client->future_deleteEdges(r);
        },
        [] (const std::pair<const PartitionID, std::vector<cpp2::EdgeKey>>& p) {
            return p.first;
        });
}


folly::SemiFuture<StorageRpcResponse<cpp2::ExecResponse>>
GraphStorageClient::deleteVertices(GraphSpaceID space,
                                   std::vector<VertexID> ids,
                                   folly::EventBase* evb) {
    auto status = clusterIdsToHosts(space,
                                    std::move(ids),
                                    [](const VertexID& v) -> const VertexID& {
        return v;
    });

    if (!status.ok()) {
        return folly::makeFuture<StorageRpcResponse<cpp2::ExecResponse>>(
            std::runtime_error(status.status().toString()));
    }

    auto& clusters = status.value();
    std::unordered_map<HostAddr, cpp2::DeleteVerticesRequest> requests;
    for (auto& c : clusters) {
        auto& host = c.first;
        auto& req = requests[host];
        req.set_space_id(space);
        req.set_parts(std::move(c.second));
    }

    return collectResponse(
        evb,
        std::move(requests),
        [] (cpp2::GraphStorageServiceAsyncClient* client,
            const cpp2::DeleteVerticesRequest& r) {
            return client->future_deleteVertices(r);
        },
        [] (const std::pair<const PartitionID, std::vector<VertexID>>& p) {
            return p.first;
        });
}


folly::Future<StatusOr<storage::cpp2::UpdateResponse>>
GraphStorageClient::updateVertex(GraphSpaceID space,
                                 const VertexID &vertexId,
                                 std::vector<cpp2::UpdatedVertexProp> updatedProps,
                                 bool insertable,
                                 std::vector<cpp2::VertexProp> returnProps,
                                 std::string condition,
                                 folly::EventBase* evb) {
    std::pair<HostAddr, cpp2::UpdateVertexRequest> request;

    DCHECK(!!metaClient_);
    auto status = metaClient_->partId(space, vertexId);
    if (!status.ok()) {
        return folly::makeFuture<StatusOr<storage::cpp2::UpdateResponse>>(status.status());
    }

    auto part = status.value();
    auto hStatus = getPartHosts(space, part);
    if (!hStatus.ok()) {
        return folly::makeFuture<StatusOr<storage::cpp2::UpdateResponse>>(hStatus.status());
    }

    auto partHosts = hStatus.value();
    CHECK_GT(partHosts.hosts_.size(), 0U);
    const auto& host = this->getLeader(partHosts);
    request.first = std::move(host);
    cpp2::UpdateVertexRequest req;
    req.set_space_id(space);
    req.set_vertex_id(vertexId);
    req.set_part_id(part);
    req.set_updated_props(std::move(updatedProps));
    req.set_return_props(std::move(returnProps));
    req.set_insertable(insertable);
    if (condition.size() > 0) {
        req.set_condition(std::move(condition));
    }
    request.second = std::move(req);

    return getResponse(
        evb, std::move(request),
        [] (cpp2::GraphStorageServiceAsyncClient* client,
            const cpp2::UpdateVertexRequest& r) {
            return client->future_updateVertex(r);
        });
}


folly::Future<StatusOr<storage::cpp2::UpdateResponse>>
GraphStorageClient::updateEdge(GraphSpaceID space,
                               const storage::cpp2::EdgeKey &edgeKey,
                               std::vector<cpp2::UpdatedEdgeProp> updatedProps,
                               bool insertable,
                               std::vector<cpp2::EdgeProp> returnProps,
                               std::string condition,
                               folly::EventBase* evb) {
    std::pair<HostAddr, cpp2::UpdateEdgeRequest> request;

    DCHECK(!!metaClient_);
    auto status = metaClient_->partId(space, edgeKey.get_src());
    if (!status.ok()) {
        return folly::makeFuture<StatusOr<storage::cpp2::UpdateResponse>>(status.status());
    }

    auto part = status.value();
    auto hStatus = getPartHosts(space, part);
    if (!hStatus.ok()) {
        return folly::makeFuture<StatusOr<storage::cpp2::UpdateResponse>>(hStatus.status());
    }
    auto partHosts = hStatus.value();
    CHECK_GT(partHosts.hosts_.size(), 0U);

    const auto& host = this->getLeader(partHosts);
    request.first = std::move(host);
    cpp2::UpdateEdgeRequest req;
    req.set_space_id(space);
    req.set_edge_key(edgeKey);
    req.set_part_id(part);
    req.set_updated_props(std::move(updatedProps));
    req.set_return_props(std::move(returnProps));
    req.set_insertable(insertable);
    if (condition.size() > 0) {
        req.set_condition(std::move(condition));
    }
    request.second = std::move(req);

    return getResponse(
        evb, std::move(request),
        [] (cpp2::GraphStorageServiceAsyncClient* client,
            const cpp2::UpdateEdgeRequest& r) {
            return client->future_updateEdge(r);
        });
}


folly::Future<StatusOr<cpp2::GetUUIDResp>>
GraphStorageClient::getUUID(GraphSpaceID space,
                            const std::string& name,
                            folly::EventBase* evb) {
    std::pair<HostAddr, cpp2::GetUUIDReq> request;
    DCHECK(!!metaClient_);
    auto status = metaClient_->partId(space, name);
    if (!status.ok()) {
        return folly::makeFuture<StatusOr<cpp2::GetUUIDResp>>(status.status());
    }

    auto part = status.value();
    auto hStatus = getPartHosts(space, part);
    if (!hStatus.ok()) {
        return folly::makeFuture<StatusOr<cpp2::GetUUIDResp>>(hStatus.status());
    }
    auto partHosts = hStatus.value();
    CHECK_GT(partHosts.hosts_.size(), 0U);

    const auto& leader = this->getLeader(partHosts);
    request.first = leader;
    cpp2::GetUUIDReq req;
    req.set_space_id(space);
    req.set_part_id(part);
    req.set_name(name);
    request.second = std::move(req);

    return getResponse(
        evb,
        std::move(request),
        [] (cpp2::GraphStorageServiceAsyncClient* client,
            const cpp2::GetUUIDReq& r) {
            return client->future_getUUID(r);
    });
}


folly::SemiFuture<StorageRpcResponse<cpp2::LookUpVertexIndexResp>>
GraphStorageClient::lookUpVertexIndex(GraphSpaceID space,
                                      IndexID indexId,
                                      const std::string &filter,
                                      const std::vector<std::string> &returnCols,
                                      folly::EventBase *evb) {
    auto status = getHostParts(space);
    if (!status.ok()) {
        return folly::makeFuture<StorageRpcResponse<cpp2::LookUpVertexIndexResp>>(
            std::runtime_error(status.status().toString()));
    }

    auto& clusters = status.value();
    std::unordered_map<HostAddr, cpp2::LookUpIndexRequest> requests;
    for (auto& c : clusters) {
        auto& host = c.first;
        auto& req = requests[host];
        req.set_space_id(space);
        req.set_parts(std::move(c.second));
        req.set_index_id(indexId);
        req.set_filter(filter);
        req.set_return_columns(returnCols);
    }
    return collectResponse(evb,
                           std::move(requests),
                           [] (cpp2::GraphStorageServiceAsyncClient* client,
                               const cpp2::LookUpIndexRequest& r) {
                               return client->future_lookUpVertexIndex(r); },
                           [] (const PartitionID& part) {
                               return part;
                           });
}


folly::SemiFuture<StorageRpcResponse<cpp2::LookUpEdgeIndexResp>>
GraphStorageClient::lookUpEdgeIndex(GraphSpaceID space,
                                    IndexID indexId,
                                    const std::string &filter,
                                    const std::vector<std::string> &returnCols,
                                    folly::EventBase *evb) {
    auto status = getHostParts(space);
    if (!status.ok()) {
        return folly::makeFuture<StorageRpcResponse<cpp2::LookUpEdgeIndexResp>>(
            std::runtime_error(status.status().toString()));
    }

    auto& clusters = status.value();
    std::unordered_map<HostAddr, cpp2::LookUpIndexRequest> requests;
    for (auto& c : clusters) {
        auto& host = c.first;
        auto& req = requests[host];
        req.set_space_id(space);
        req.set_parts(std::move(c.second));
        req.set_index_id(indexId);
        req.set_filter(filter);
        req.set_return_columns(returnCols);
    }
    return collectResponse(evb,
                           std::move(requests),
                           [] (cpp2::GraphStorageServiceAsyncClient* client,
                               const cpp2::LookUpIndexRequest& r) {
                               return client->future_lookUpEdgeIndex(r); },
                           [] (const PartitionID& part) {
                               return part;
                           });
}

}   // namespace storage
}   // namespace nebula
