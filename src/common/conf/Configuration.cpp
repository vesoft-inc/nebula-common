/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */
#include "common/base/Base.h"
#include "common/conf/Configuration.h"

namespace nebula {
namespace conf {

Configuration::Configuration() {
    content_ = std::make_unique<folly::dynamic>(folly::dynamic::object());
}

Configuration::Configuration(folly::dynamic content) {
    CHECK(content.isObject()) << "The content is not a valid configuration";
    content_ = std::make_unique<folly::dynamic>(std::move(content));
}


Status Configuration::parseFromFile(const std::string &filename) {
    auto fd = ::open(filename.c_str(), O_RDONLY);
    auto status = Status::OK();
    std::string content;
    do {
        if (fd == -1) {
            if (errno == ENOENT) {
                status = Status::Error(ErrorCode::E_FILE_NOT_FOUND,
                                       ERROR_FLAG(1),
                                       filename.c_str());
                break;
            }
            if (errno == EPERM) {
                status = Status::Error(ErrorCode::E_FILE_NOT_PERMISSION,
                                       ERROR_FLAG(1),
                                       filename.c_str());
                break;
            }
            status = Status::Error(ErrorCode::E_INTERNAL_ERROR,
                                   ERROR_FLAG(1),
                                   "Unknown error");
            break;
        }
        // get the file size
        auto len = ::lseek(fd, 0, SEEK_END);
        if (len == 0) {
            status = Status::Error(ErrorCode::E_FILE_CONTENT_IS_EMPTY,
                                   ERROR_FLAG(1),
                                   filename.c_str());
            break;
        }
        ::lseek(fd, 0, SEEK_SET);
        // read the whole content
        // TODO(dutor) ::read might be interrupted by signals
        auto buffer = std::make_unique<char[]>(len + 1);
        auto charsRead = ::read(fd, buffer.get(), len);
        UNUSED(charsRead);
        buffer[len] = '\0';
        // strip off all comments
        static const std::regex comment("//.*|#.*");
        content = std::regex_replace(buffer.get(), comment, "");
    } while (false);

    if (fd != -1) {
        ::close(fd);
    }

    if (!status.ok()) {
        return status;
    }

    return parseFromString(content);
}


Status Configuration::parseFromString(const std::string &content) {
    try {
        auto json = folly::parseJson(content);
        content_ = std::make_unique<folly::dynamic>(std::move(json));
    } catch (std::exception &e) {
        LOG(ERROR) << e.what();
        return Status::Error(ErrorCode::E_CONFIG_WRONG_JSON_FORMAT, ERROR_FLAG(1), e.what());
    }
    return Status::OK();
}

std::string Configuration::dumpToString() const {
    std::string json;
    if (content_ != nullptr) {
        json = folly::toJson(*content_);
    }
    return json;
}

std::string Configuration::dumpToPrettyString() const {
    std::string json;
    if (content_ != nullptr) {
        json = folly::toPrettyJson(*content_);
    }
    return json;
}


Status Configuration::fetchAsInt(const char *key, int64_t &val) const {
    DCHECK(content_ != nullptr);
    auto iter = content_->find(key);
    if (iter == content_->items().end()) {
        return Status::Error(ErrorCode::E_CONFIG_ITEM_NOT_FOUND, ERROR_FLAG(1), key);
    }
    if (!iter->second.isInt()) {
        return Status::Error(ErrorCode::E_CONFIG_INVALID_ITEM_TYPE, ERROR_FLAG(2), key, "integer");
    }
    val = iter->second.getInt();
    return Status::OK();
}


Status Configuration::fetchAsDouble(const char *key, double &val) const {
    DCHECK(content_ != nullptr);
    auto iter = content_->find(key);
    if (iter == content_->items().end()) {
        return Status::Error(ErrorCode::E_CONFIG_ITEM_NOT_FOUND, ERROR_FLAG(1), key);
    }
    if (!iter->second.isDouble()) {
        return Status::Error(ErrorCode::E_CONFIG_INVALID_ITEM_TYPE, ERROR_FLAG(2), key, "double");
    }
    val = iter->second.getDouble();
    return Status::OK();
}


Status Configuration::fetchAsBool(const char *key, bool &val) const {
    DCHECK(content_ != nullptr);
    auto iter = content_->find(key);
    if (iter == content_->items().end()) {
        return Status::Error(ErrorCode::E_CONFIG_ITEM_NOT_FOUND, ERROR_FLAG(1), key);
    }
    if (!iter->second.isBool()) {
        return Status::Error(ErrorCode::E_CONFIG_INVALID_ITEM_TYPE, ERROR_FLAG(2), key, "boolean");
    }
    val = iter->second.getBool();
    return Status::OK();
}


Status Configuration::fetchAsString(const char *key, std::string &val) const {
    DCHECK(content_ != nullptr);
    auto iter = content_->find(key);
    if (iter == content_->items().end()) {
        return Status::Error(ErrorCode::E_CONFIG_ITEM_NOT_FOUND, ERROR_FLAG(1), key);
    }
    if (!iter->second.isString()) {
        return Status::Error(ErrorCode::E_CONFIG_INVALID_ITEM_TYPE, ERROR_FLAG(2), key, "string");
    }
    val = iter->second.getString();
    return Status::OK();
}


Status Configuration::fetchAsSubConf(const char *key, Configuration &subconf) const {
    DCHECK(content_ != nullptr);
    auto iter = content_->find(key);
    if (iter == content_->items().end()) {
        return Status::Error(ErrorCode::E_CONFIG_ITEM_NOT_FOUND, ERROR_FLAG(1), key);
    }
    if (!iter->second.isObject()) {
        return Status::Error(ErrorCode::E_CONFIG_WRONG_JSON_FORMAT, ERROR_FLAG(1), key);
    }
    subconf.content_ = std::make_unique<folly::dynamic>(iter->second);
    return Status::OK();
}


Status Configuration::upsertStringField(const char* key, const std::string& val) {
    DCHECK(content_ != nullptr);
    auto iter = content_->find(key);
    if (iter == content_->items().end()) {
        return Status::Error(ErrorCode::E_CONFIG_ITEM_NOT_FOUND, ERROR_FLAG(1), key);
    }
    if (!iter->second.isString()) {
        return Status::Error(ErrorCode::E_CONFIG_INVALID_ITEM_TYPE, ERROR_FLAG(2), key, "string");
    }
    (*content_)[key] = val;
    return Status::OK();
}


Status Configuration::fetchAsIntArray(
        const char *key,
        std::vector<int64_t> &val) const {
    DCHECK(content_ != nullptr);
    auto iter = content_->find(key);
    if (iter == content_->items().end()) {
        return Status::Error(ErrorCode::E_CONFIG_ITEM_NOT_FOUND, ERROR_FLAG(1), key);
    }
    if (!iter->second.isArray()) {
        return Status::Error(ErrorCode::E_CONFIG_ITEM_NOT_FOUND, ERROR_FLAG(1), key);
    }

    for (auto& entry : iter->second) {
        try {
            val.emplace_back(entry.asInt());
        } catch (const std::exception& ex) {
            // Avoid format sercure by literal
            return Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), ex.what());
        }
    }
    return Status::OK();
}


Status Configuration::fetchAsDoubleArray(
        const char *key,
        std::vector<double> &val) const {
    DCHECK(content_ != nullptr);
    auto iter = content_->find(key);
    if (iter == content_->items().end()) {
        return Status::Error(ErrorCode::E_CONFIG_ITEM_NOT_FOUND, ERROR_FLAG(1), key);
    }
    if (!iter->second.isArray()) {
        return Status::Error(ErrorCode::E_CONFIG_ITEM_NOT_FOUND, ERROR_FLAG(1), key);
    }

    for (auto& entry : iter->second) {
        try {
            val.emplace_back(entry.asDouble());
        } catch (const std::exception& ex) {
            // Avoid format sercure by literal
            return Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), ex.what());
        }
    }
    return Status::OK();
}


Status Configuration::fetchAsBoolArray(
        const char *key,
        std::vector<bool> &val) const {
    DCHECK(content_ != nullptr);
    auto iter = content_->find(key);
    if (iter == content_->items().end()) {
        return Status::Error(ErrorCode::E_CONFIG_ITEM_NOT_FOUND, ERROR_FLAG(1), key);
    }
    if (!iter->second.isArray()) {
        return Status::Error(ErrorCode::E_CONFIG_ITEM_IS_NO_ARRAY, ERROR_FLAG(1), key);
    }

    for (auto& entry : iter->second) {
        try {
            val.emplace_back(entry.asBool());
        } catch (const std::exception& ex) {
            // Avoid format sercure by literal
            return Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), ex.what());
        }
    }
    return Status::OK();
}


Status Configuration::fetchAsStringArray(
        const char *key,
        std::vector<std::string> &val) const {
    DCHECK(content_ != nullptr);
    auto iter = content_->find(key);
    if (iter == content_->items().end()) {
        return Status::Error(ErrorCode::E_CONFIG_ITEM_NOT_FOUND, ERROR_FLAG(1), key);
    }
    if (!iter->second.isArray()) {
        return Status::Error(ErrorCode::E_CONFIG_ITEM_NOT_FOUND, ERROR_FLAG(1), key);
    }

    for (auto& entry : iter->second) {
        try {
            val.emplace_back(entry.asString());
        } catch (const std::exception& ex) {
            // Avoid format sercure by literal
            return Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), ex.what());
        }
    }
    return Status::OK();
}


Status Configuration::forEachKey(std::function<void(const std::string&)> processor) const {
    DCHECK(content_ != nullptr);
    for (auto& key : content_->keys()) {
        try {
            processor(key.asString());
        } catch (const std::exception& ex) {
            // Avoid format sercure by literal
            return Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), ex.what());
        }
    }
    return Status::OK();
}


Status Configuration::forEachItem(
        std::function<void(const std::string&, const folly::dynamic&)> processor) const {
    DCHECK(content_ != nullptr);
    for (auto& item : content_->items()) {
        try {
            processor(item.first.asString(), item.second);
        } catch (const std::exception& ex) {
            // Avoid format sercure by literal
            return Status::Error(ErrorCode::E_INTERNAL_ERROR, ERROR_FLAG(1), ex.what());
        }
    }
    return Status::OK();
}

}   // namespace conf
}   // namespace nebula
