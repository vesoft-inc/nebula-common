/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_BASE_STATUS_H_
#define COMMON_BASE_STATUS_H_

#include "common/base/Base.h"
#include "common/errorcode/ErrorCode.h"
#include "common/errorcode/ErrorMsg.h"

/**
 * Status is modeled on the one from levelDB, beyond that,
 * this one adds support on move semantics and formatted error messages.
 *
 * Status is as cheap as raw pointers in the successful case,
 * without any heap memory allocations.
 */

namespace nebula {


typedef int errorFlag;

#define ERROR_FLAG(v) (errorFlag)(v)

template <typename T>
class StatusOr;

class Status final {
public:
    Status();

    ~Status() = default;

    Status(const Status &rhs) {
        CHECK(rhs.state_);
        state_ = copyState(rhs.state_.get());
    }

    Status& operator=(const Status &rhs) {
        // `state_ == rhs.state_' means either `this == &rhs',
        // or both `*this' and `rhs' are OK
        if (state_ != rhs.state_) {
            CHECK(rhs.state_);
            state_ = copyState(rhs.state_.get());
        }
        return *this;
    }

    Status(Status &&rhs) noexcept {
        state_ = std::move(rhs.state_);
    }

    Status& operator=(Status &&rhs) noexcept {
        // `state_ == rhs.state_' means either `this == &rhs',
        // or both `*this' and `rhs' are OK
        if (state_ != rhs.state_) {
            state_ = std::move(rhs.state_);
        }
        return *this;
    }

    static Status from(const Status &s) {
        return s;
    }

    template <typename T>
    static Status from(StatusOr<T> &&s) {
        return std::move(s).status();
    }

    template <typename T>
    static Status from(const StatusOr<T> &s) {
        return s.status();
    }

    bool operator==(const Status &rhs) const {
        if (state_ == rhs.state_) {
            return true;
        }
        return errorCode() == rhs.errorCode();
    }

    bool operator!=(const Status &rhs) const {
        return !(*this == rhs);
    }

    bool ok() const {
        return errorCode() == ErrorCode::SUCCEEDED;
    }

    static Status OK() {
        return Status(ErrorCode::SUCCEEDED, "");
    }

    static Status Error(ErrorCode errorCode, int argNum, ...) {
        if (errorCode == ErrorCode::SUCCEEDED) {
            return Status(errorCode, "");
        }
        auto findCode = ErrorMsgUTF8Map.find(errorCode);
        std::string fmt;
        if (findCode == ErrorMsgUTF8Map.end()) {
            fmt = "Unknown errorCode.";
            return Status(errorCode, fmt);
        } else {
            auto findMsg = findCode->second.find(Language::L_EN);
            if (findMsg == findCode->second.end()) {
                fmt = "Use the unknown language type.";
                return Status(errorCode, fmt);
            }
            fmt = findMsg->second;
        }
        va_list args;
        va_start(args, argNum);
        auto msg = format(fmt.c_str(), args);
        va_end(args);
        return Status(errorCode, msg);
    }

    static Status ErrorMsg(ErrorCode errorCode, const std::string &errorMsg) {
        return Status(errorCode, errorMsg);
    }

    std::string toString() const;

    friend std::ostream& operator<<(std::ostream &os, const Status &status);

    ErrorCode errorCode() const {
        if (state_ == nullptr) {
            return ErrorCode::SUCCEEDED;
        }
        return reinterpret_cast<const Header*>(state_.get())->errorCode_;
    }

    // Gets the error message corresponding to the error code
    std::string message() const;

private:
    // REQUIRES: stat_ != nullptr
    uint16_t size() const {
        return reinterpret_cast<const Header*>(state_.get())->size_;
    }

    Status(ErrorCode errorCode, folly::StringPiece msg);

    static std::unique_ptr<const char[]> copyState(const char *state);

    static std::string format(const char *fmt, va_list args);

private:
    struct Header {
        uint16_t                    size_;
        ErrorCode                   errorCode_;
    };
    static constexpr auto kHeaderSize = sizeof(Header);
    // the buffer layout is:
    // state_[0..1]                 length of the error msg, i.e. size() - kHeaderSize
    // state_[2..5]                 errorCode
    // state_[6...]                 verbose error message
    std::unique_ptr<const char[]>   state_;
};


inline std::ostream& operator<<(std::ostream &os, const Status &status) {
    return os << status.toString();
}

}   // namespace nebula

#define NG_RETURN_IF_ERROR(s)                                                                      \
    do {                                                                                           \
        const auto &__s = (s);                                                                     \
        if (UNLIKELY(!__s.ok())) {                                                                 \
            return ::nebula::Status::from(__s);                                                    \
        }                                                                                          \
    } while (0)

#define NG_LOG_AND_RETURN_IF_ERROR(s)                                                              \
    do {                                                                                           \
        const auto &__s = (s);                                                                     \
        if (UNLIKELY(!__s.ok())) {                                                                 \
            ::nebula::Status __status = ::nebula::Status::from(__s);                               \
            LOG(ERROR) << __status;                                                                \
            return __status;                                                                       \
        }                                                                                          \
    } while (0)

#endif  // COMMON_BASE_STATUS_H_
