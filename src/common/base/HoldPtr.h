/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_BASE_HOLDPTR_H_
#define COMMON_BASE_HOLDPTR_H_

#include <type_traits>

// Means the object is hold by someone already
// It's the mark of ownership
// In some typical cases:
// 1. avoid mistake hold by other, e.g:
// ```
//     auto fc = new RelationalExpression(kind, expr->clone().release());
// ```
// if the expr is HoldPtr typed instead of raw pointer, it will prevent the ownership mistake lead
// to double-free
// 2. member of some struct, e.g:
// ```
//     struct XXX {
//         HoldPtr<Type> f;
//     };
// ```
// We could know that f is hold by someone clearly, if it's raw pointer, its ownership is confused.
template <typename T>
struct HoldPtr {
public:
    static_assert(!std::is_reference_v<T>);
    explicit HoldPtr<T>(T* p) : ptr_{p} {}
    HoldPtr<T>(const HoldPtr<T> &p) : ptr_(p.ptr_) {}
    HoldPtr<T>(HoldPtr<T> &&p) : ptr_(p.ptr_) {
        p.ptr_ = nullptr;
    }
    ~HoldPtr<T>() = default;

    // Take care the ownership
    // Don't own the retrun raw pointer
    T* get() const {
        return ptr_;
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const noexcept {
        return ptr_;
    }

private:
    T* ptr_{nullptr};
};

#endif  // COMMON_BASE_HOLDPTR_H_
