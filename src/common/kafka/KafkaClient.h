/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_KAFKACLIENT_H
#define COMMON_KAFKACLIENT_H

#include "common/base/Base.h"
#include "common/interface/gen-cpp2/common_types.h"
#include <librdkafka/rdkafkacpp.h>

namespace nebula {
namespace kafka {

class NebulaDeliveryReportCb : public RdKafka::DeliveryReportCb {
public:
  void dr_cb(RdKafka::Message &message) {
      if (message.err()) {
          LOG(ERROR) << "Message delivery failed: "
                     << message.errstr();
      }
  }
};

class KafkaClient {
public:
    explicit KafkaClient(const std::string& brokers);

    ~KafkaClient() = default;

    nebula::cpp2::ErrorCode produce(std::string topic,
                                    int32_t partition,
                                    const std::string& key,
                                    const std::string& value);

    nebula::cpp2::ErrorCode flush(int32_t interval = 3000);

private:
    std::unique_ptr<RdKafka::Producer> producer;
};

}   // namespace kafka
}   // namespace nebula

#endif  // COMMON_KAFKACLIENT_H
