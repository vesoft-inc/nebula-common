/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef UTIL_OBJECTPOOL_H_
#define UTIL_OBJECTPOOL_H_

#include <functional>
#include <list>

#include <folly/SpinLock.h>

#include "cpp/helpers.h"

namespace nebula {

class ObjectPool final : private cpp::NonCopyable, private cpp::NonMovable {
public:
    ObjectPool() = default;

    ~ObjectPool() {
        clear();
    }

    void clear() {
        folly::SpinLockGuard g(lock_);
        for (auto &e : objects_) {
            e.deleteFn(e.obj);
        }
        objects_.clear();
    }

    template <typename T>
    T *add(T *obj) {
        folly::SpinLockGuard g(lock_);
        objects_.emplace_back(Element{obj, [](void *p) { delete reinterpret_cast<T *>(p); }});
        return obj;
    }

private:
    struct Element {
        void *obj;
        std::function<void(void *)> deleteFn;
    };

    folly::SpinLock lock_;
    std::list<Element> objects_;
};

}   // namespace nebula

#endif   // UTIL_OBJECTPOOL_H_
