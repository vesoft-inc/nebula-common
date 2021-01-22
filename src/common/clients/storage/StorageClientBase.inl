/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <folly/Try.h>
#include "common/time/WallClock.h"

namespace nebula {
namespace storage {


template<typename ClientType>
StorageClientBase<ClientType>::StorageClientBase(
    std::shared_ptr<folly::IOThreadPoolExecutor> threadPool,
    meta::MetaClient* metaClient)
        : metaClient_(metaClient)
        , ioThreadPool_(threadPool) {
    clientsMan_ = std::make_unique<thrift::ThriftClientManager<ClientType>>();
}


template<typename ClientType>
StorageClientBase<ClientType>::~StorageClientBase() {
    VLOG(3) << "Destructing StorageClientBase";
    if (nullptr != metaClient_) {
        metaClient_ = nullptr;
    }
}


template<typename ClientType>
void StorageClientBase<ClientType>::loadLeader() const {
    if (loadLeaderBefore_) {
        return;
    }
    bool expected = false;
    if (isLoadingLeader_.compare_exchange_strong(expected, true)) {
        CHECK(metaClient_ != nullptr);
        auto status = metaClient_->loadLeader();
        if (status.ok()) {
            folly::RWSpinLock::WriteHolder wh(leadersLock_);
            leaders_ = std::move(status).value();
            loadLeaderBefore_ = true;
        }
        isLoadingLeader_ = false;
    }
}


template<typename ClientType>
const HostAddr
StorageClientBase<ClientType>::getLeader(const meta::PartHosts& partHosts) const {
    loadLeader();
    auto part = std::make_pair(partHosts.spaceId_, partHosts.partId_);
    {
        folly::RWSpinLock::ReadHolder rh(leadersLock_);
        auto it = leaders_.find(part);
        if (it != leaders_.end()) {
            return it->second;
        }
    }
    {
        folly::RWSpinLock::WriteHolder wh(leadersLock_);
        VLOG(1) << "No leader exists. Choose one random.";
        const auto& random = partHosts.hosts_[folly::Random::rand32(partHosts.hosts_.size())];
        leaders_[part] = random;
        return random;
    }
}


template<typename ClientType>
void StorageClientBase<ClientType>::updateLeader(GraphSpaceID spaceId,
                                                 PartitionID partId,
                                                 const HostAddr& leader) {
    LOG(INFO) << "Update the leader for [" << spaceId << ", " << partId << "] to " << leader;
    folly::RWSpinLock::WriteHolder wh(leadersLock_);
    leaders_[std::make_pair(spaceId, partId)] = leader;
}


template<typename ClientType>
void StorageClientBase<ClientType>::invalidLeader(GraphSpaceID spaceId,
                                                  PartitionID partId) {
    LOG(INFO) << "Invalidate the leader for [" << spaceId << ", " << partId << "]";
    folly::RWSpinLock::WriteHolder wh(leadersLock_);
    auto it = leaders_.find(std::make_pair(spaceId, partId));
    if (it != leaders_.end()) {
        leaders_.erase(it);
    }
}

template<typename ClientType>
void StorageClientBase<ClientType>::invalidLeader(GraphSpaceID spaceId,
                                                  std::vector<PartitionID> &partsId) {
    folly::RWSpinLock::WriteHolder wh(leadersLock_);
    for (const auto &partId : partsId) {
        LOG(INFO) << "Invalidate the leader for [" << spaceId << ", " << partId << "]";
        auto it = leaders_.find(std::make_pair(spaceId, partId));
        if (it != leaders_.end()) {
            leaders_.erase(it);
        }
    }
}


template<typename ClientType>
template<class Request, class RemoteFunc, class Response>
folly::SemiFuture<StorageRpcResponse<Response>>
StorageClientBase<ClientType>::collectResponse(
        folly::EventBase* evb,
        std::unordered_map<HostAddr, Request> requests,
        RemoteFunc&& remoteFunc,
        int32_t portOffsetIfRetry,
        std::size_t retry,
        std::size_t retryLimit) {
    auto context = std::make_shared<ResponseContext<Request, RemoteFunc, Response>>(
        requests.size(), std::forward<RemoteFunc>(remoteFunc));
    collectResponseImpl(evb,
                std::move(requests),
                portOffsetIfRetry,
                context,
                retry,
                retryLimit);
    return context->promise.getSemiFuture();
}

template<typename ClientType>
template<class Request, class RemoteFunc, class Response>
void StorageClientBase<ClientType>::collectResponseImpl(
    folly::EventBase* evb,
    std::unordered_map<HostAddr, Request> requests,
    int32_t portOffsetIfRetry,
    std::shared_ptr<ResponseContext<Request, RemoteFunc, Response>> context,
    std::size_t retry,
    std::size_t retryLimit) {

    if (evb == nullptr) {
        DCHECK(!!ioThreadPool_);
        evb = ioThreadPool_->getEventBase();
    }

    folly::via(evb, [this,
                     evb,
                     context,
                     requests = std::move(requests),
                     retry,
                     retryLimit,
                     portOffsetIfRetry] () mutable {
        std::vector<folly::Future<Response>> responses;
        responses.reserve(requests.size());
        std::vector<int64_t> startsMS;
        startsMS.reserve(requests.size());
        std::vector<Request> reqs;
        reqs.reserve(requests.size());
        std::vector<HostAddr> hosts;
        hosts.reserve(requests.size());
        std::vector<GraphSpaceID> spaceIds;
        spaceIds.reserve(requests.size());
        for (auto& req : requests) {
            auto& host = req.first;
            auto spaceId = req.second.get_space_id();
            auto client = clientsMan_->client(host,
                                                evb,
                                                false,
                                                FLAGS_storage_client_timeout_ms);
            startsMS.emplace_back(time::WallClock::fastNowInMicroSec());
            // Invoke the remote method
            // Future process code will be executed on the IO thread
            // Since all requests are sent using the same eventbase, all then-callback
            // will be executed on the same IO thread
            responses.emplace_back(context->serverMethod(client.get(), req.second).via(evb));
            reqs.emplace_back(std::move(req.second));
            hosts.emplace_back(std::move(host));
            spaceIds.emplace_back(spaceId);
        }

        folly::collectAll(responses).then([this,
                                        evb,
                                        context,
                                        hosts = std::move(hosts),
                                        spaceIds = std::move(spaceIds),
                                        startsMS = std::move(startsMS),
                                        reqs = std::move(reqs),
                                        portOffsetIfRetry,
                                        retry,
                                        retryLimit] (std::vector<folly::Try<Response>>&& vals) {
            for (std::size_t i = 0; i < vals.size(); ++i) {
                auto spaceId = spaceIds[i];
                auto &host = hosts[i];
                auto &val = vals[i];
                auto &req = reqs[i];
                if (val.hasException()) {
                    LOG(ERROR) << "Request to " << host
                                << " failed: " << val.exception().what();
                    auto parts = getReqPartsId(req);
                    context->resp.appendFailedParts(parts, storage::cpp2::ErrorCode::E_RPC_FAILURE);
                    invalidLeader(spaceId, parts);
                    context->resp.markFailure();
                } else {
                    std::unordered_map<HostAddr, Request> retryRequests;
                    auto resp = std::move(val.value());
                    auto& result = resp.result;
                    bool hasFailure{false};
                    decltype(result.failed_parts) notRetryParts;
                    for (auto& code : result.get_failed_parts()) {
                        VLOG(3) << "Failure! Failed part " << code.get_part_id()
                                << ", failed code " << static_cast<int32_t>(code.get_code());
                        if (code.get_code() == storage::cpp2::ErrorCode::E_LEADER_CHANGED) {
                            auto* leader = code.get_leader();
                            if (isValidHostPtr(leader)) {
                                updateLeader(spaceId, code.get_part_id(), *leader);
                            } else {
                                invalidLeader(spaceId, code.get_part_id());
                            }
                            auto newHost = retryRequests.find(*leader);
                            if (newHost == retryRequests.end()) {
                                retryRequests.emplace(*leader,
                                                       getReqWithPartId(req, code.get_part_id()));
                            } else {
                                appendPart(newHost->second.parts,
                                           req.get_parts(),
                                           code.get_part_id());
                            }
                        } else if (code.get_code() == cpp2::ErrorCode::E_PART_NOT_FOUND ||
                                code.get_code() == cpp2::ErrorCode::E_SPACE_NOT_FOUND) {
                            invalidLeader(spaceId, code.get_part_id());
                            hasFailure = true;
                            notRetryParts.emplace_back(code);
                        } else {
                            // Simply keep the result
                            context->resp.emplaceFailedPart(code.get_part_id(), code.get_code());
                            hasFailure = true;
                            notRetryParts.emplace_back(code);
                        }
                    }
                    if (retry < retryLimit) {
                        result.failed_parts = notRetryParts;
                    }
                    if (hasFailure) {
                        context->resp.markFailure();
                    }

                    // Adjust the latency
                    auto latency = result.get_latency_in_us();
                    context->resp.setLatency(host,
                                            latency,
                                            time::WallClock::fastNowInMicroSec() - startsMS[i]);

                    // Keep the response
                    context->resp.responses().emplace_back(std::move(resp));

                    if (!retryRequests.empty()) {
                        if (retry < retryLimit) {
                            evb->runAfterDelay([this, evb, requests = std::move(retryRequests),
                                                context,
                                                retry, retryLimit,
                                                portOffsetIfRetry] () mutable {
                                collectResponseImpl(evb,
                                                    std::move(requests),
                                                    portOffsetIfRetry,
                                                    context,
                                                    retry + 1,
                                                    retryLimit);
                                }, FLAGS_storage_client_retry_interval_ms);
                        } else {
                            // retry failed
                            context->resp.markFailure();
                            context->promise.setValue(std::move(context->resp));
                        }
                    } else {
                        // retry succeeded
                        context->promise.setValue(std::move(context->resp));
                    }
                }  // no except
            }  // for
        });  // then
    });  // via
}

template<typename ClientType>
template<class Request, class RemoteFunc, class Response>
folly::Future<StatusOr<Response>> StorageClientBase<ClientType>::getResponse(
        folly::EventBase* evb,
        std::pair<HostAddr, Request>&& request,
        RemoteFunc&& remoteFunc,
        int32_t leaderPortOffset,
        folly::Promise<StatusOr<Response>> pro,
        std::size_t retry,
        std::size_t retryLimit) {
    auto f = pro.getFuture();
    getResponseImpl(evb,
                std::forward<decltype(request)>(request),
                std::forward<RemoteFunc>(remoteFunc),
                leaderPortOffset,
                std::move(pro),
                retry,
                retryLimit);
    return f;
}

template<typename ClientType>
template<class Request, class RemoteFunc, class Response>
void StorageClientBase<ClientType>::getResponseImpl(
        folly::EventBase* evb,
        std::pair<HostAddr, Request> request,
        RemoteFunc remoteFunc,
        int32_t leaderPortOffset,
        folly::Promise<StatusOr<Response>> pro,
        std::size_t retry,
        std::size_t retryLimit) {
    if (evb == nullptr) {
        DCHECK(!!ioThreadPool_);
        evb = ioThreadPool_->getEventBase();
    }
    folly::via(evb, [evb, request = std::move(request), remoteFunc = std::move(remoteFunc),
                     leaderPortOffset, pro = std::move(pro), retry, retryLimit, this] () mutable {
        auto host = request.first;
        auto client = clientsMan_->client(host, evb, false, FLAGS_storage_client_timeout_ms);
        auto spaceId = request.second.get_space_id();
        auto partsId = getReqPartsId(request.second);
        LOG(INFO) << "Send request to storage " << host;
        remoteFunc(client.get(), request.second).via(evb)
             .then([spaceId,
                    partsId = std::move(partsId),
                    p = std::move(pro),
                    request = std::move(request),
                    remoteFunc = std::move(remoteFunc),
                    evb,
                    retry,
                    retryLimit,
                    leaderPortOffset,
                    this] (folly::Try<Response>&& t) mutable {
            // exception occurred during RPC
            if (t.hasException()) {
                p.setValue(
                    Status::Error(
                        folly::stringPrintf("RPC failure in StorageClient: %s",
                                            t.exception().what().c_str())));
                invalidLeader(spaceId, partsId);
                return;
            }
            auto&& resp = std::move(t.value());
            // leader changed
            auto& result = resp.get_result();
            for (auto& code : result.get_failed_parts()) {
                VLOG(3) << "Failure! Failed part " << code.get_part_id()
                        << ", failed code " << static_cast<int32_t>(code.get_code());
                if (code.get_code() == storage::cpp2::ErrorCode::E_LEADER_CHANGED) {
                    auto* leader = code.get_leader();
                    if (isValidHostPtr(leader)) {
                        updateLeader(spaceId, code.get_part_id(), *leader);
                    } else {
                        invalidLeader(spaceId, code.get_part_id());
                    }
                    if (retry < retryLimit && isValidHostPtr(leader)) {
                        evb->runAfterDelay([this, evb, leader = *leader,
                                req = std::move(request.second),
                                remoteFunc = std::move(remoteFunc), p = std::move(p),
                                leaderPortOffset,
                                retry, retryLimit] () mutable {
                                leader.port += leaderPortOffset;
                                getResponseImpl(evb,
                                        std::pair<HostAddr, Request>(std::move(leader),
                                        std::move(req)),
                                        std::move(remoteFunc),
                                        leaderPortOffset,
                                        std::move(p),
                                        retry + 1,
                                        retryLimit);
                                }, FLAGS_storage_client_retry_interval_ms);
                        return;
                    } else {
                        p.setValue(Status::LeaderChanged("Request to storage retry failed."));
                        return;
                    }
                } else if (code.get_code() == storage::cpp2::ErrorCode::E_PART_NOT_FOUND ||
                           code.get_code() == storage::cpp2::ErrorCode::E_SPACE_NOT_FOUND) {
                    invalidLeader(spaceId, code.get_part_id());
                }
            }
            p.setValue(std::move(resp));
        });
    });  // via
}


template<typename ClientType>
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
StorageClientBase<ClientType>::clusterIdsToHosts(GraphSpaceID spaceId,
                                                 const Container& ids,
                                                 GetIdFunc f) const {
    std::unordered_map<
        HostAddr,
        std::unordered_map<
            PartitionID,
            std::vector<typename Container::value_type>
        >
    > clusters;
    for (auto& id : ids) {
        CHECK(!!metaClient_);
        auto status = metaClient_->partId(spaceId, f(id));
        if (!status.ok()) {
            return status.status();
        }

        auto part = status.value();
        auto metaStatus = getPartHosts(spaceId, part);
        if (!metaStatus.ok()) {
            return status.status();
        }

        auto partHosts = metaStatus.value();
        CHECK_GT(partHosts.hosts_.size(), 0U);
        const auto leader = this->getLeader(partHosts);
        clusters[leader][part].emplace_back(std::move(id));
    }
    return clusters;
}


template<typename ClientType>
StatusOr<std::unordered_map<HostAddr, std::vector<PartitionID>>>
StorageClientBase<ClientType>::getHostParts(GraphSpaceID spaceId) const {
    std::unordered_map<HostAddr, std::vector<PartitionID>> hostParts;
    auto status = metaClient_->partsNum(spaceId);
    if (!status.ok()) {
        return Status::Error("Space not found, spaceid: %d", spaceId);
    }

    auto parts = status.value();
    for (auto partId = 1; partId <= parts; partId++) {
        auto metaStatus = getPartHosts(spaceId, partId);
        if (!metaStatus.ok()) {
            return metaStatus.status();
        }
        auto partHosts = std::move(metaStatus).value();
        DCHECK_GT(partHosts.hosts_.size(), 0U);
        const auto leader = getLeader(partHosts);
        hostParts[leader].emplace_back(partId);
    }
    return hostParts;
}

}   // namespace storage
}   // namespace nebula
