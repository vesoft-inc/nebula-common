/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/base/Base.h"
#include "common/base/Status.h"

namespace nebula {

Status::Status() {
    auto state = std::unique_ptr<char[]>(new char[kHeaderSize]);
    auto *header = reinterpret_cast<Header*>(state.get());
    header->size_ = 0;
    header->errorCode_ = ErrorCode::SUCCEEDED;
    state_ = std::move(state);
}

Status::Status(ErrorCode errorCode, folly::StringPiece msg) {
    const uint16_t size = msg.size();
    auto state = std::unique_ptr<char[]>(new char[size + kHeaderSize]);
    auto *header = reinterpret_cast<Header*>(state.get());
    header->size_ = size;
    header->errorCode_ = errorCode;
    ::memcpy(&state[kHeaderSize], msg.data(), size);
    state_ = std::move(state);
}

std::string Status::message() const {
    if (this->errorCode() == ErrorCode::SUCCEEDED) {
        return "SUCCEEDED";
    }
    return std::string(&state_[kHeaderSize], size());
}

std::string Status::toString() const {
    if (this->errorCode() == ErrorCode::SUCCEEDED) {
        return "SUCCEEDED";
    }
    std::string result;
    result.reserve(size());
    result.append(std::to_string(static_cast<int32_t>(this->errorCode())));
    result.append(":");
    result.append(&state_[kHeaderSize], size());
    return result;
}

std::unique_ptr<const char[]> Status::copyState(const char *state) {
    const auto size = *reinterpret_cast<const uint16_t*>(state);
    const auto total = size + kHeaderSize;
    auto result = std::unique_ptr<char[]>(new char[total]);
    ::memcpy(&result[0], state, total);
    return result;
}


std::string Status::format(const char *fmt, va_list args) {
    char result[256];
    vsnprintf(result, sizeof(result), fmt, args);
    return result;
}

}   // namespace nebula
