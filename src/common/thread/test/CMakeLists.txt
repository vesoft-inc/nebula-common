# Copyright (c) 2020 vesoft inc. All rights reserved.
#
# This source code is licensed under Apache 2.0 License,
# attached with Common Clause Condition 1.0, found in the LICENSES directory.

nebula_add_test(
    NAME
        thread_test
    SOURCES
        ThreadTest.cpp
        GenericWorkerTest.cpp
        GenericThreadPoolTest.cpp
    OBJECTS
        $<TARGET_OBJECTS:thread_obj>
        $<TARGET_OBJECTS:concurrent_obj>
        $<TARGET_OBJECTS:time_obj>
    LIBRARIES
        gtest
        gtest_main
)

# When ASan enabled, there will be a linking error if folly itself was not sanitized.
# There is an open PR to address this issue: https://github.com/facebook/folly/pull/1510
# For the time being, we disable the use of folly::fibers when ASan.
if(NOT ENABLE_ASAN)
    nebula_add_executable(
        NAME time_sharing_execute_showcase
        SOURCES TimeSharingExecuteShowcase.cpp
        OBJECTS $<TARGET_OBJECTS:time_obj>
    )
endif()
