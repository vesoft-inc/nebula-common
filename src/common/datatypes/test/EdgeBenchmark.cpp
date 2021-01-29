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
#include <folly/String.h>

#include "common/datatypes/Edge.h"
#include "common/datatypes/Value.h"

using nebula::Edge;

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

BENCHMARK(HashStringEdgeKey, n) {
    std::vector<Edge> edges;
    BENCHMARK_SUSPEND {
        edges.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            edges.emplace_back(
                Edge(randomString(10), randomString(10), random(-200, 200), "like", 0, {}));
        }
    }
    std::unordered_set<std::string> uniqueEdge;
    for (const auto& edge : edges) {
        const auto& src = edge.src.toString();
        const auto& dst = edge.dst.toString();
        auto edgeKey = folly::stringPrintf("%s%ld%s%ld%d%ld",
                                           src.c_str(),
                                           src.size(),
                                           dst.c_str(),
                                           dst.size(),
                                           edge.type,
                                           edge.ranking);
        folly::hash::fnv64(edgeKey);
    }
}

BENCHMARK_RELATIVE(HashStringEdgeKeyOut, n) {
    std::vector<std::string> edges;
    BENCHMARK_SUSPEND {
        edges.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            auto edgeKey = folly::stringPrintf("%s%ld%s%ld%d%ld",
                                               randomString(10).c_str(),
                                               static_cast<int64_t>(10),
                                               randomString(10).c_str(),
                                               static_cast<int64_t>(10),
                                               random(-200, 200),
                                               static_cast<int64_t>(0));
            edges.emplace_back(edgeKey);
        }
    }
    std::unordered_set<std::string> uniqueEdge;
    for (const auto& edge : edges) {
        folly::hash::fnv64(edge);
    }
}

BENCHMARK_RELATIVE(HashEdge, n) {
    std::vector<Edge> edges;
    BENCHMARK_SUSPEND {
        edges.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            edges.emplace_back(
                Edge(randomString(10), randomString(10), random(-200, 200), "like", 0, {}));
        }
    }
    std::unordered_set<Edge> uniqueEdge;
    for (const auto& edge : edges) {
        std::hash<Edge>()(edge);
    }
}

BENCHMARK_RELATIVE(HashMoveEdge, n) {
    std::vector<Edge> edges;
    BENCHMARK_SUSPEND {
        edges.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            edges.emplace_back(
                Edge(randomString(10), randomString(10), random(-200, 200), "like", 0, {}));
        }
    }
    std::unordered_set<Edge> uniqueEdge;
    uniqueEdge.reserve(edges.size());
    for (auto& edge : edges) {
        uniqueEdge.emplace(std::move(edge));
    }
}

BENCHMARK_RELATIVE(HashMoveStringEdgeKey, n) {
    std::vector<Edge> edges;
    BENCHMARK_SUSPEND {
        edges.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            edges.emplace_back(
                Edge(randomString(10), randomString(10), random(-200, 200), "like", 0, {}));
        }
    }
    std::unordered_set<std::string> uniqueEdge;
    uniqueEdge.reserve(edges.size());
    for (const auto& edge : edges) {
        const auto& src = edge.src.toString();
        const auto& dst = edge.dst.toString();
        auto edgeKey = folly::stringPrintf("%s%ld%s%ld%d%ld",
                                           src.c_str(),
                                           src.size(),
                                           dst.c_str(),
                                           dst.size(),
                                           edge.type,
                                           edge.ranking);
        uniqueEdge.emplace(std::move(edgeKey));
    }
}

int main() {
    folly::runBenchmarks();
    return 0;
}

// ============================================================================
// nebula-common/src/common/datatypes/test/EdgeBenchmark  time/iter  iters/s
// ============================================================================
// HashStringEdgeKey                                          707.88ns    1.41M
// HashStringEdgeKeyOut                             312.45%   226.55ns    4.41M
// HashEdge                                         191.63%   369.40ns    2.71M
// HashMoveEdge                                      63.33%     1.12us  894.66K
// HashMoveStringEdgeKey                            101.68%   696.15ns    1.44M
// ============================================================================
