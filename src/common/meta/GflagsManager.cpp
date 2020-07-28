/* Copyright (c) 2019 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/meta/GflagsManager.h"
#include "common/conf/Configuration.h"
#include "common/fs/FileUtils.h"

DEFINE_string(gflags_mode_json, "share/resources/gflags.json", "gflags mode json for service");

namespace nebula {
namespace meta {

template <typename ValueType>
Value GflagsManager::gflagsValueToThriftValue(const gflags::CommandLineFlagInfo& flag) {
    return Value(folly::to<ValueType>(flag.current_value));
}

std::unordered_map<std::string, std::pair<cpp2::ConfigMode, bool>>
GflagsManager::parseConfigJson(const std::string& path) {
    std::unordered_map<std::string, std::pair<cpp2::ConfigMode, bool>> configModeMap;
    conf::Configuration conf;
    if (!conf.parseFromFile(path).ok()) {
        LOG(ERROR) << "Load gflags json failed";
        return configModeMap;
    }
    static std::vector<std::string> keys = {"MUTABLE"};
    static std::vector<cpp2::ConfigMode> modes = {cpp2::ConfigMode::MUTABLE};
    for (size_t i = 0; i < keys.size(); i++) {
        std::vector<std::string> values;
        if (!conf.fetchAsStringArray(keys[i].c_str(), values).ok()) {
            continue;
        }
        cpp2::ConfigMode mode = modes[i];
        for (const auto& name : values) {
            configModeMap[name] = {mode, false};
        }
    }
    static std::string nested = "NESTED";
    std::vector<std::string> values;
    if (conf.fetchAsStringArray(nested.c_str(), values).ok()) {
        for (const auto& name : values) {
            // all nested gflags regard as mutable ones
            configModeMap[name] = {cpp2::ConfigMode::MUTABLE, true};
        }
    }

    return configModeMap;
}

std::vector<cpp2::ConfigItem> GflagsManager::declareGflags(const cpp2::ConfigModule& module) {
    std::vector<cpp2::ConfigItem> configItems;
    if (module == cpp2::ConfigModule::UNKNOWN) {
        return configItems;
    }
    auto mutableConfig = parseConfigJson(FLAGS_gflags_mode_json);
    // Get all flags by listing all defined gflags
    std::vector<gflags::CommandLineFlagInfo> flags;
    gflags::GetAllFlags(&flags);
    for (auto& flag : flags) {
        auto& name = flag.name;
        auto& type = flag.type;
        Value value;

        // We only register mutable configs to meta
        cpp2::ConfigMode mode = cpp2::ConfigMode::MUTABLE;
        bool isNested = false;
        auto iter = mutableConfig.find(name);
        if (iter != mutableConfig.end()) {
            isNested = iter->second.second;
        } else {
            continue;
        }

        // TODO: all int32/uint32/uint64 gflags are converted to int64 for now
        if (type == "uint32" || type == "int32" || type == "int64" || type == "uint64") {
            value = gflagsValueToThriftValue<int64_t>(flag);
        } else if (type == "double") {
            value = gflagsValueToThriftValue<double>(flag);
        } else if (type == "bool") {
            value = gflagsValueToThriftValue<bool>(flag);
        } else if (type == "string") {
            value = gflagsValueToThriftValue<std::string>(flag);
            // only string gflags can be nested
            if (isNested) {
                VLOG(1) << "Nested value: " << value;
                // transform to map value
                conf::Configuration conf;
                auto status = conf.parseFromString(value.getStr());
                if (!status.ok()) {
                    LOG(ERROR) << "Parse nested gflags: " << name
                               << ", value: " << value
                               << " failed: " << status;
                    continue;
                }
                Map map;
                conf.forEachItem([&map] (const std::string& key, const folly::dynamic& val) {
                    map.kvs.emplace(key, val.asString());
                });
                value.setMap(std::move(map));
            }
        } else {
            LOG(INFO) << "Not able to declare " << name << " of " << type;
            continue;
        }
        cpp2::ConfigItem item;
        item.name = name;
        item.module = module;
        item.mode = mode;
        item.value = std::move(value);
        configItems.emplace_back(std::move(item));
    }
    LOG(INFO) << "Prepare to register " << configItems.size() << " gflags to meta";
    return configItems;
}

void GflagsManager::getGflagsModule(cpp2::ConfigModule& gflagsModule) {
    // get current process according to gflags pid_file
    gflags::CommandLineFlagInfo pid;
    if (gflags::GetCommandLineFlagInfo("pid_file", &pid)) {
        auto defaultPid = pid.default_value;
        if (defaultPid.find("nebula-graphd") != std::string::npos) {
            gflagsModule = cpp2::ConfigModule::GRAPH;
        } else if (defaultPid.find("nebula-storaged") != std::string::npos) {
            gflagsModule = cpp2::ConfigModule::STORAGE;
        } else if (defaultPid.find("nebula-metad") != std::string::npos) {
            gflagsModule = cpp2::ConfigModule::META;
        } else {
            LOG(ERROR) << "Should not reach here";
        }
    } else {
        LOG(INFO) << "Unknown config module";
    }
}

std::string GflagsManager::trimAllWhitespace(const std::string &inStr) {
    std::string outStr;
    outStr.reserve(inStr.size());
    uint32_t count = 0;
    for (auto i = 0u; i < inStr.size(); i++) {
        if (inStr[i] == ' ' || inStr[i] == '\n' || inStr[i] == '\t' || inStr[i] == '\r') {
            continue;
        }
        outStr.push_back(inStr[i]);
        count++;
    }
    outStr.resize(count);
    return outStr;
}

}   // namespace meta
}   // namespace nebula
