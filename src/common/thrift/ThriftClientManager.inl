/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <thrift/lib/cpp2/async/HeaderClientChannel.h>
#include <thrift/lib/cpp/async/TAsyncSocket.h>
#include <folly/io/async/AsyncSocket.h>
#include <folly/system/ThreadName.h>
#include "common/network/NetworkUtils.h"

DECLARE_int32(conn_timeout_ms);

namespace nebula {
namespace thrift {

template<class ClientType>
std::shared_ptr<ClientType> ThriftClientManager<ClientType>::client(
        const HostAddr& host, folly::EventBase* evb, bool compatibility, uint32_t timeout) {
    if (evb == nullptr) {
        evb = folly::EventBaseManager::get()->getEventBase();
    }
    // Get client from client manager if it is ok.
    auto it = clientMap_->find(std::make_pair(host, evb));
    if (it != clientMap_->end()) {
        do {
            auto channel =
                dynamic_cast<apache::thrift::HeaderClientChannel*>(it->second->getChannel());
            if (channel == nullptr || !channel->good()) {
                // Remove bad connection to create a new one.
                clientMap_->erase(it);
                LOG(ERROR) << "Invalid Channel: " << channel << " for host: " << host;
                break;
            }
            auto transport = dynamic_cast<folly::AsyncSocket*>(channel->getTransport());
            if (transport == nullptr || transport->hangup()) {
                clientMap_->erase(it);
                LOG(ERROR) << "Transport is closed by peers " << transport << " for host: " << host;
                break;
            }
            VLOG(2) << "Getting a client to " << host;
            return it->second;
        } while (false);
    }

    // Need to create a new client and insert it to client map.
    VLOG(2) << "There is no existing client to " << host << ", trying to create one";
    static thread_local int connectionCount = 0;
    /*
     * TODO(liuyu): folly said 'resolve' may take second to finish
     *              if this really happen, we will add a cache here.
     * */
    HostAddr tempHost = host;
    if (!folly::IPAddress::validate(tempHost.host)) {
        try {
            folly::SocketAddress socketAddr(tempHost.host, tempHost.port, true);
            std::ostringstream oss;
            oss << "resolve " << tempHost << " as ";
            tempHost.host = socketAddr.getAddressStr();
            oss << tempHost;
            LOG(INFO) << oss.str();
        } catch(const std::exception& e) {
            LOG(ERROR) << e.what();
            return nullptr;
        }
    }

    VLOG(2) << "Connecting to " << host << " for " << ++connectionCount << " times";
    std::shared_ptr<apache::thrift::async::TAsyncSocket> socket;
    evb->runImmediatelyOrRunInEventBaseThreadAndWait(
        [&socket, evb, host]() {
            socket = apache::thrift::async::TAsyncSocket::newSocket(
                evb, host.host, host.port, FLAGS_conn_timeout_ms);
        });
    auto headerClientChannel = apache::thrift::HeaderClientChannel::newChannel(socket);
    if (timeout > 0) {
        headerClientChannel->setTimeout(timeout);
    }
    if (compatibility) {
        headerClientChannel->setProtocolId(apache::thrift::protocol::T_BINARY_PROTOCOL);
        headerClientChannel->setClientType(THRIFT_UNFRAMED_DEPRECATED);
    }
    std::shared_ptr<ClientType> client(
        new ClientType(std::move(headerClientChannel)),
        [evb](auto* p) { evb->runImmediatelyOrRunInEventBaseThreadAndWait([p] { delete p; }); });
    clientMap_->emplace(std::make_pair(host, evb), client);
    return client;
}

}  // namespace thrift
}  // namespace nebula
