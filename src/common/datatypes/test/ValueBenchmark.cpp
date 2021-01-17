/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <string>
#include <unordered_set>
#include <vector>

#include <folly/Benchmark.h>
#include <folly/Random.h>

#include "common/datatypes/Value.h"

using nebula::Value;

static const int seed = folly::randomNumberSeed();
using RandomT = std::mt19937;
static RandomT rng(seed);

template <class Integral1, class Integral2>
Integral2 random(Integral1 low, Integral2 up) {
    std::uniform_int_distribution<> range(low, up);
    return range(rng);
}

std::string randomString(size_t size = 1000) {
    std::string str(size, ' ');
    for (size_t p = 0; p < size; p++) {
        str[p] = random('a', 'z');
    }
    return str;
}

BENCHMARK(HashString, n) {
    std::vector<Value> values;
    BENCHMARK_SUSPEND {
        values.reserve(n);
        for (size_t i = 0; i < n; i++) {
            values.emplace_back(randomString(10));
        }
    }
    std::unordered_set<std::string> set;
    for (const auto &value : values) {
        set.emplace(value.toString());
    }
}

BENCHMARK_RELATIVE(HashValue, n) {
    std::vector<Value> values;
    BENCHMARK_SUSPEND {
        values.reserve(n);
        for (size_t i = 0; i < n; i++) {
            values.emplace_back(randomString(10));
        }
    }
    std::unordered_set<Value> set;
    for (const auto &value : values) {
        set.emplace(value);
    }
}

int main() {
    folly::runBenchmarks();
    return 0;
}

// ============================================================================
// nebula-common/src/common/datatypes/test/ValueBenchmark.cpp relative  time/iter  iters/s
// ============================================================================
// HashString                                                 481.34ns    2.08M
// HashValue                                         64.35%   747.95ns    1.34M
// ============================================================================
