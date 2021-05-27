/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/kafka/KafkaClient.h"

namespace nebula {
namespace kafka {

KafkaClient::KafkaClient(const std::string& brokers) {
    std::string err;
    auto *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

    if (conf->set("bootstrap.servers", brokers, err) != RdKafka::Conf::CONF_OK) {
        LOG(ERROR) << "Brokers is illegal: " << brokers;
        throw std::invalid_argument("Brokers is illegal");
    } else {
        NebulaDeliveryReportCb nebulaCB;
        if (conf->set("dr_cb", &nebulaCB, err) != RdKafka::Conf::CONF_OK) {
            LOG(ERROR) << "Nebula Delivery Report Callback failed";
            throw std::invalid_argument("Nebula Delivery Report Callback failed");
        } else {
            producer.reset(RdKafka::Producer::create(conf, err));
        }
    }
}

nebula::cpp2::ErrorCode KafkaClient::produce(std::string topic,
                                             int32_t partition,
                                             const std::string& key,
                                             const std::string& value) {
    LOG(INFO) << "Send Message: "
              << " topic: " << topic
              << " partition: " << partition
              << " value: " << value;
    auto code = producer->produce(topic,
                                  partition,
                                  RdKafka::Producer::RK_MSG_COPY,
                                  const_cast<char *>(value.c_str()),
                                  value.size(),
                                  const_cast<char *>(key.c_str()),
                                  key.size(),
                                  0,
                                  NULL);
    if (code == RdKafka::ErrorCode::ERR_NO_ERROR) {
        return nebula::cpp2::ErrorCode::SUCCEEDED;
    }
    return nebula::cpp2::ErrorCode::E_KAFKA_CLIENT_FAILED;
}

nebula::cpp2::ErrorCode KafkaClient::flush(int32_t interval) {
    auto code = producer->flush(interval);
    if (code == RdKafka::ErrorCode::ERR_NO_ERROR) {
        return nebula::cpp2::ErrorCode::SUCCEEDED;
    }
    return nebula::cpp2::ErrorCode::E_KAFKA_CLIENT_FAILED;
}

}   // namespace kafka
}   // namespace nebula
