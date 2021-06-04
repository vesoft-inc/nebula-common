/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <folly/Benchmark.h>
#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/futures/Future.h>
#include <folly/init/Init.h>
#include <jemalloc/jemalloc.h>

int64_t thread_pool_size = 1000;
int64_t thread_count = 300;
auto executor_pool = std::make_unique<folly::CPUThreadPoolExecutor>(thread_pool_size);
std::vector<folly::Future<int>> futs;

size_t mallctlStats(size_t iters) {
    for (size_t i = 0; i < iters; ++i) {
        size_t sz = 8;
        uint64_t stats = 0;
        mallctl("stats.allocated", &stats, &sz, NULL, 0);
    }
    return iters;
}

size_t mallctlEpoch(size_t iters) {
    for (size_t i = 0; i < iters; ++i) {
        uint64_t epoch = 1;
        size_t sz = 8;
        mallctl("epoch", 0, 0, &epoch, sz);
    }
    return iters;
}

folly::Future<int> doSomeThing() {
    for (int i = 0; i < 10000; ++i) {
        auto* pm = malloc(1024 * 2048);
        free(pm);
    }
    return 0;
}

BENCHMARK_NAMED_PARAM_MULTI(mallctlStats, mallctlBM)
BENCHMARK_NAMED_PARAM_MULTI(mallctlEpoch, mallctlBM)

int main(int argc, char* argv[]) {
    folly::init(&argc, &argv, true);
    for (uint32_t i = 0; i < thread_count; i++) {
        futs.emplace_back(doSomeThing().via(executor_pool.get()));
    }

    folly::runBenchmarks();
    folly::collectAll(futs);

    return 0;
}
