/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_CLIENTS_STORAGE_STORAGECLIENTBASE_H_
#define COMMON_CLIENTS_STORAGE_STORAGECLIENTBASE_H_

#include "common/base/Base.h"
#include <folly/futures/Future.h>
#include <folly/executors/IOThreadPoolExecutor.h>
#include "common/base/StatusOr.h"
#include "common/meta/Common.h"
#include "common/thrift/ThriftClientManager.h"
#include "common/clients/meta/MetaClient.h"
#include "common/interface/gen-cpp2/storage_types.h"

DECLARE_int32(storage_client_timeout_ms);
DECLARE_uint32(storage_client_retry_interval_ms);

constexpr int32_t kInternalPortOffset = -2;

namespace nebula {
namespace storage {

template<class Response>
class StorageRpcResponse final {
public:
    enum class Result {
        ALL_SUCCEEDED = 0,
        PARTIAL_SUCCEEDED = 1,
    };

    explicit StorageRpcResponse(size_t reqsSent) : totalReqsSent_(reqsSent) {}

    void increase(size_t size) {
        totalReqsSent_ += size;
    }

    bool succeeded() const {
        return result_ == Result::ALL_SUCCEEDED;
    }

    int32_t maxLatency() const {
        return maxLatency_;
    }

    void setLatency(HostAddr host, int32_t latency, int32_t e2eLatency) {
        if (latency > maxLatency_) {
            maxLatency_ = latency;
        }
        hostLatency_.emplace_back(std::make_tuple(host, latency, e2eLatency));
    }

    void markFailure() {
        result_ = Result::PARTIAL_SUCCEEDED;
        ++failedReqs_;
    }

    // A value between [0, 100], representing a precentage
    int32_t completeness() const {
        DCHECK_NE(totalReqsSent_, 0);
        return totalReqsSent_ == 0 ? 0 : (totalReqsSent_ - failedReqs_) * 100 / totalReqsSent_;
    }

    const std::unordered_map<PartitionID, storage::cpp2::ErrorCode>& failedParts() const {
        return failedParts_;
    }

    void emplaceFailedPart(PartitionID partId, storage::cpp2::ErrorCode errorCode) {
        failedParts_.emplace(partId, errorCode);
    }

    void appendFailedParts(const std::vector<PartitionID> &partsId,
                           storage::cpp2::ErrorCode errorCode) {
        failedParts_.reserve(failedParts_.size() + partsId.size());
        for (const auto &partId : partsId) {
            emplaceFailedPart(partId, errorCode);
        }
    }

    std::vector<Response>& responses() {
        return responses_;
    }

    const std::vector<std::tuple<HostAddr, int32_t, int32_t>>& hostLatency() const {
        return hostLatency_;
    }

private:
    size_t totalReqsSent_;
    size_t failedReqs_{0};

    Result result_{Result::ALL_SUCCEEDED};
    std::unordered_map<PartitionID, storage::cpp2::ErrorCode> failedParts_;
    int32_t maxLatency_{0};
    std::vector<Response> responses_;
    std::vector<std::tuple<HostAddr, int32_t, int32_t>> hostLatency_;
};

template<class Request, class RemoteFunc, class Response>
struct ResponseContext {
public:
    ResponseContext(size_t reqsSent, RemoteFunc remoteFunc)
        : resp(reqsSent)
        , serverMethod(remoteFunc) {}

    // Return true if processed all responses
    bool finishSending() {
        std::lock_guard<std::mutex> g(lock_);
        finishSending_ = true;
        if (ongoingRequests_.empty() && !fulfilled_) {
            fulfilled_ = true;
            return true;
        } else {
            return false;
        }
    }

    void increase(size_t size) {
        resp.increase(size);
    }

    std::pair<const Request*, bool> insertRequest(HostAddr host, Request&& req) {
        std::lock_guard<std::mutex> g(lock_);
        auto res = ongoingRequests_.emplace(host, std::move(req));
        return std::make_pair(&res.first->second, res.second);
    }

    Request& findRequest(HostAddr host) {
        std::lock_guard<std::mutex> g(lock_);
        auto it = ongoingRequests_.find(host);
        DCHECK(it != ongoingRequests_.end());
        return it->second;
    }

    // Return true if processed all responses
    bool removeRequest(HostAddr host) {
        std::lock_guard<std::mutex> g(lock_);
        ongoingRequests_.erase(host);
        if (finishSending_ && !fulfilled_ && ongoingRequests_.empty()) {
            fulfilled_ = true;
            return true;
        } else {
            return false;
        }
    }

public:
    folly::Promise<StorageRpcResponse<Response>> promise;
    StorageRpcResponse<Response> resp;
    RemoteFunc serverMethod;

private:
    std::mutex lock_;
    std::unordered_map<HostAddr, Request> ongoingRequests_;
    bool finishSending_{false};
    bool fulfilled_{false};
};


/**
 * A base class for all storage clients
 */
template<typename ClientType>
class StorageClientBase {
protected:
    StorageClientBase(std::shared_ptr<folly::IOThreadPoolExecutor> ioThreadPool,
                      meta::MetaClient* metaClient);
    virtual ~StorageClientBase();

    virtual void loadLeader() const;
    const HostAddr getLeader(const meta::PartHosts& partHosts) const;
    void updateLeader(GraphSpaceID spaceId, PartitionID partId, const HostAddr& leader);
    void invalidLeader(GraphSpaceID spaceId, PartitionID partId);
    void invalidLeader(GraphSpaceID spaceId, std::vector<PartitionID> &partsId);

    template<class Request,
             class RemoteFunc,
             class Response =
                typename std::result_of<
                    RemoteFunc(ClientType*, const Request&)
                >::type::value_type
            >
    folly::SemiFuture<StorageRpcResponse<Response>> collectResponse(
        folly::EventBase* evb,
        std::unordered_map<HostAddr, Request> requests,
        RemoteFunc&& remoteFunc,
        int32_t portOffsetIfRetry = 0,
        std::size_t retry = 0,
        std::size_t retryLimit = 3,
        std::shared_ptr<ResponseContext<Request, RemoteFunc, Response>> context = nullptr);


    // getResponse is only used in single part scenarios
    template<class Request,
             class RemoteFunc,
             class Response =
                typename std::result_of<
                    RemoteFunc(ClientType* client, const Request&)
                >::type::value_type
            >
    folly::Future<StatusOr<Response>> getResponse(
            folly::EventBase* evb,
            std::pair<HostAddr, Request>&& request,
            RemoteFunc&& remoteFunc,
            int32_t leaderPortOffset = 0,
            folly::Promise<StatusOr<Response>> pro = folly::Promise<StatusOr<Response>>(),
            std::size_t retry = 0,
            std::size_t retryLimit = 3);

    // Remove the extra part of the request
    // only include the part of the specified partId
    template<class Request>
    Request getSinglePartReqest(PartitionID part, Request& request);

    template<class Request,
             class RemoteFunc,
             class Response =
                typename std::result_of<
                    RemoteFunc(ClientType* client, const Request&)
                >::type::value_type
            >
    void getResponseImpl(
            folly::EventBase* evb,
            std::pair<HostAddr, Request> request,
            RemoteFunc remoteFunc,
            int32_t leaderPortOffset,
            folly::Promise<StatusOr<Response>> pro,
            std::size_t retry,
            std::size_t retryLimit);

    // Cluster given ids into the host they belong to
    // The method returns a map
    //  host_addr (A host, but in most case, the leader will be chosen)
    //      => (partition -> [ids that belong to the shard])
    template<class Container, class GetIdFunc>
    StatusOr<
        std::unordered_map<
            HostAddr,
            std::unordered_map<
                PartitionID,
                std::vector<typename Container::value_type>
            >
        >
    >
    clusterIdsToHosts(GraphSpaceID spaceId, const Container& ids, GetIdFunc f) const;

    virtual StatusOr<meta::PartHosts> getPartHosts(GraphSpaceID spaceId,
                                                   PartitionID partId) const {
        CHECK(metaClient_ != nullptr);
        return metaClient_->getPartHostsFromCache(spaceId, partId);
    }

    virtual StatusOr<std::unordered_map<HostAddr, std::vector<PartitionID>>>
    getHostParts(GraphSpaceID spaceId) const;

    // from map
    template <typename K>
    std::vector<PartitionID> getReqPartsIdFromContainer(
        const std::unordered_map<PartitionID, K> &t) const {
        std::vector<PartitionID> partsId;
        partsId.reserve(t.size());
        for (const auto &part : t) {
            partsId.emplace_back(part.first);
        }
        return partsId;
    }

    template <typename K>
    std::unordered_map<PartitionID, K> getReqPartsIdFromContainer(
        const std::unordered_map<PartitionID, K> &t, PartitionID partId) const {
        std::unordered_map<PartitionID, K> ret;
        ret.emplace(partId, t.at(partId));
        return ret;
    }

    template <typename K>
    void addPartRequest(std::unordered_map<PartitionID, K> &oldMap,
                        std::unordered_map<PartitionID, K> &newMap,
                        PartitionID partId) {
        auto it = newMap.find(partId);
        if (it != newMap.end()) {
            it->second.insert(newMap.at(partId).end(),
                              oldMap.at(partId).begin(), oldMap.at(partId).end());
        } else {
            newMap[partId] = oldMap.at(partId);
        }
    }

    // from list
    std::vector<PartitionID> getReqPartsIdFromContainer(const std::vector<PartitionID> &t) const {
        return t;
    }

    std::vector<PartitionID> getReqPartsIdFromContainer(const std::vector<PartitionID>&,
                                                        PartitionID partId) const {
        return {partId};
    }

    template <typename K>
    void addPartRequest(std::vector<PartitionID, K>&,
                        std::vector<PartitionID, K> &newMap,
                        PartitionID partId) {
        newMap.push_back(partId);
    }

    template <typename Request>
    std::vector<PartitionID> getReqPartsId(const Request &req) const {
        return getReqPartsIdFromContainer(req.get_parts());
    }

    std::vector<PartitionID> getReqPartsId(const cpp2::UpdateVertexRequest &req) const {
        return {req.get_part_id()};
    }

    std::vector<PartitionID> getReqPartsId(const cpp2::UpdateEdgeRequest &req) const {
        return {req.get_part_id()};
    }

    std::vector<PartitionID> getReqPartsId(const cpp2::GetUUIDReq &req) const {
        return {req.get_part_id()};
    }

    std::vector<PartitionID> getReqPartsId(const cpp2::InternalTxnRequest &req) const {
        return {req.get_part_id()};
    }

    std::vector<PartitionID> getReqPartsId(const cpp2::GetValueRequest &req) const {
        return {req.get_part_id()};
    }

    bool isValidHostPtr(const HostAddr* addr) {
        return addr != nullptr && !addr->host.empty() && addr->port != 0;
    }

    template<class Request>
    void addPartRetryRequest(HostAddr& leader,
                             PartitionID partId,
                             Request& oldRequest,
                             std::unordered_map<HostAddr, Request> &retryRequests);

protected:
    meta::MetaClient* metaClient_{nullptr};

private:
    std::shared_ptr<folly::IOThreadPoolExecutor> ioThreadPool_;
    std::unique_ptr<thrift::ThriftClientManager<ClientType>> clientsMan_;

    mutable folly::RWSpinLock leadersLock_;
    mutable std::unordered_map<std::pair<GraphSpaceID, PartitionID>, HostAddr> leaders_;
    mutable std::atomic_bool loadLeaderBefore_{false};
    mutable std::atomic_bool isLoadingLeader_{false};
};

}   // namespace storage
}   // namespace nebula

#include "common/clients/storage/StorageClientBase.inl"

#endif  // COMMON_CLIENTS_STORAGE_STORAGECLIENTBASE_H_
