/* Copyright (c) 2019 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_META_GFLAGSMANAGER_H_
#define COMMON_META_GFLAGSMANAGER_H_

#include "common/base/Base.h"
#include "common/base/Status.h"
#include "common/base/StatusOr.h"
#include "common/interface/gen-cpp2/MetaServiceAsyncClient.h"

namespace nebula {
namespace meta {

class GflagsManager {
public:
    // methods for consoles, reg/set/get/list configs from meta server
    virtual folly::Future<StatusOr<cpp2::ExecResp>> setConfig(const cpp2::ConfigModule& module,
                                                              const std::string& name,
                                                              const cpp2::ConfigType& type,
                                                              const VariantType& value) = 0;

    virtual folly::Future<StatusOr<cpp2::GetConfigResp>>
    getConfig(const cpp2::ConfigModule& module, const std::string& name) = 0;

    virtual folly::Future<StatusOr<cpp2::ListConfigsResp>>
    listConfigs(const cpp2::ConfigModule& module) = 0;

    static void getGflagsModule(cpp2::ConfigModule& gflagsModule);

    static std::vector<cpp2::ConfigItem> declareGflags(const cpp2::ConfigModule& module);

protected:
    virtual ~GflagsManager() = default;

    static std::unordered_map<std::string, std::pair<cpp2::ConfigMode, bool>>
           parseConfigJson(const std::string& json);

    template<typename ValueType>
    static std::string gflagsValueToThriftValue(const gflags::CommandLineFlagInfo& flag);
};

std::string toThriftValueStr(const cpp2::ConfigType& type, const VariantType& value);

cpp2::ConfigMode toThriftConfigMode(const std::string& key);

cpp2::ConfigItem toThriftConfigItem(const cpp2::ConfigModule& module, const std::string& name,
                                    const cpp2::ConfigType& type, const cpp2::ConfigMode& mode,
                                    const std::string& value);


}  // namespace meta
}  // namespace nebula
#endif  // COMMON_META_GFLAGSMANAGER_H_
