/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/base/Base.h"
#include "common/time/Duration.h"
#include "common/thread/FiberExecutor.h"

using nebula::time::Duration;
using nebula::thread::FiberExecutor;

DEFINE_uint64(tick, 1, "us");
DEFINE_uint64(small, 100, "");
DEFINE_uint64(tasks, 10000, "");
DEFINE_uint64(time_slice, 100, "us");
DEFINE_uint32(threads, 1, "");
DEFINE_uint32(big_factor, 10, "");
DEFINE_double(big_rate, 0.01, "");

void delay(uint64_t us) {
    Duration duration;
    while (duration.elapsedInUSec() < us) {
    }
}

void run(bool yield) {
    FiberExecutor executor(FLAGS_threads);
    Duration totalDuration;
    std::atomic<uint64_t> totalUs{0};

    auto add = [&] (bool isBig) {
        if (isBig) {
            auto big = [=, &totalUs, duration = Duration()] () {
                Duration slicer;
                for (auto i = 0u; i < FLAGS_big_factor * FLAGS_small / FLAGS_tick; i++) {
                    delay(FLAGS_tick);
                    if (yield && slicer.elapsedInUSec() >= FLAGS_time_slice) {
                        folly::fibers::yield();
                        slicer.reset();
                    }
                }
                totalUs += duration.elapsedInUSec();
            };
            executor.add(big);
        } else {
            auto small = [=, &totalUs, duration = Duration()] () {
                delay(FLAGS_small);
                totalUs += duration.elapsedInUSec();
            };
            executor.add(small);
        }
    };

    constexpr auto MAXRAND = 10000UL;
    for (auto i = 1u; i < FLAGS_tasks; i++) {
        delay(FLAGS_small / FLAGS_threads);
        if (folly::Random::rand64(MAXRAND) < MAXRAND * FLAGS_big_rate) {
            add(true);
        } else {
            add(false);
        }
    }

    executor.stop();
    fprintf(stderr, "Time sharing: %s, "
                    "Finished after: %lums, "
                    "Average latency: %luus\n",
                    yield ? "yes" : "false",
                    totalDuration.elapsedInMSec(),
                    totalUs / FLAGS_tasks);
}

int main(int argc, char **argv) {
    folly::init(&argc, &argv, false);
    run(false);
    /**
     * Time sharing: false, Finished after: 1109ms, Average latency: 52096us
     */
    run(true);
    /**
     * Time sharing: yes, Finished after: 1120ms, Average latency: 9179us
     */
    return 0;
}
