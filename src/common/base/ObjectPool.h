/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef UTIL_OBJECTPOOL_H_
#define UTIL_OBJECTPOOL_H_

#include <functional>
#include <unordered_map>

#include <folly/SpinLock.h>

#include "common/base/StatusOr.h"
#include "common/cpp/helpers.h"

namespace nebula {

class ObjectPool final : private cpp::NonCopyable, private cpp::NonMovable {
public:
    using DeleteFn = std::function<void(void *)>;

    ObjectPool() = default;
    ~ObjectPool() {
        clear();
    }

    void clear() {
        folly::SpinLockGuard g(lock_);
        for (auto &obj : objects_) {
            obj.second(obj.first);
        }
        objects_.clear();
    }

    template <typename T>
    T *add(T *obj) {
        auto ptr = add(obj, [](void *p) { delete reinterpret_cast<T *>(p); });
        return reinterpret_cast<T *>(ptr);
    }

    template <typename T, typename... Args>
    T *makeAndAdd(Args &&... args) {
        return add(new T(std::forward<Args>(args)...));
    }

    StatusOr<DeleteFn> release(void *obj) {
        folly::SpinLockGuard g(lock_);
        auto iter = objects_.find(obj);
        if (iter == objects_.end()) {
            return Status::Error("Not found the pointer!");
        }
        auto fn = iter->second;
        objects_.erase(obj);
        return fn;
    }

    void *add(void *obj, DeleteFn fn) {
        folly::SpinLockGuard g(lock_);
        objects_.emplace(obj, std::move(fn));
        return obj;
    }

private:
    folly::SpinLock lock_;
    std::unordered_map<void *, DeleteFn> objects_;
};

}   // namespace nebula

#endif   // UTIL_OBJECTPOOL_H_
