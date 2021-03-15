/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */
#ifndef COMMON_THREAD_FIBEREXECUTOR_H_
#define COMMON_THREAD_FIBEREXECUTOR_H_

#include <folly/executors/IOThreadPoolExecutor.h>
#include <folly/executors/FiberIOExecutor.h>

namespace nebula {
namespace thread {

class FiberExecutor final : public folly::Executor {
public:
    FiberExecutor(size_t n,
                  std::string name = "",
                  folly::fibers::FiberManager::Options opts = {}) {
        if (!name.empty()) {
            auto tf = std::make_shared<folly::NamedThreadFactory>(name);
            io_ = std::make_shared<folly::IOThreadPoolExecutor>(n, std::move(tf));
        } else {
            io_ = std::make_shared<folly::IOThreadPoolExecutor>(n);
        }
        fiber_ = std::make_shared<folly::FiberIOExecutor>(io_, opts);
    }

    ~FiberExecutor() override {
        stop();
        join();
    }

    void add(folly::Func func) override {
        fiber_->add(std::move(func));
    }

    void stop() {
        if (io_ != nullptr) {
            io_->stop();
        }
    }

    void join() {
        if (io_ != nullptr) {
            io_->join();
        }
        io_ = nullptr;
        fiber_ = nullptr;
    }

private:
    std::shared_ptr<folly::IOThreadPoolExecutor>    io_;
    std::shared_ptr<folly::FiberIOExecutor>         fiber_;
};

}   // namespace thread
}   // namespace nebula

#endif  // COMMON_THREAD_FIBEREXECUTOR_H_
