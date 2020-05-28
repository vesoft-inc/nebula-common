# Copyright (c) 2020 vesoft inc. All rights reserved.
#
# This source code is licensed under Apache 2.0 License,
# attached with Common Clause Condition 1.0, found in the LICENSES directory.
set(JIT_TEST_LIBS
    gtest
    ${THRIFT_LIBRARIES}
    ${llvm_libs}
    ${Folly_LIBRARY}
    ${Libevent_LIBRARY}
    ${DoubleConversion_LIBRARY}
    boost_context
    gtest
    rt
    dl
    tinfo
    pthread
    z
   )


nebula_add_test(
    NAME
        jit_func_manager_test
    SOURCES
        JITFuncManagerTest.cpp
        AdHocSchemaManager.cpp
        ../../expression/test/ExpressionContextMock.cpp
    OBJECTS
        $<TARGET_OBJECTS:meta_client_obj>
        $<TARGET_OBJECTS:file_based_cluster_id_man_obj>
        $<TARGET_OBJECTS:meta_obj>
        $<TARGET_OBJECTS:meta_thrift_obj>
        $<TARGET_OBJECTS:common_thrift_obj>
        $<TARGET_OBJECTS:conf_obj>
        $<TARGET_OBJECTS:thread_obj>
        $<TARGET_OBJECTS:thrift_obj>
        $<TARGET_OBJECTS:jit_obj>
        $<TARGET_OBJECTS:expression_obj>
        $<TARGET_OBJECTS:fs_obj>
        $<TARGET_OBJECTS:datatypes_obj>
        $<TARGET_OBJECTS:base_obj>
    LIBRARIES
        ${JIT_TEST_LIBS}
        wangle
)


nebula_add_test(
    NAME
        expr_jit_test
    SOURCES
        ExprJITTest.cpp
        AdHocSchemaManager.cpp
        ../../expression/test/ExpressionContextMock.cpp
    OBJECTS
        $<TARGET_OBJECTS:meta_client_obj>
        $<TARGET_OBJECTS:file_based_cluster_id_man_obj>
        $<TARGET_OBJECTS:meta_obj>
        $<TARGET_OBJECTS:meta_thrift_obj>
        $<TARGET_OBJECTS:common_thrift_obj>
        $<TARGET_OBJECTS:conf_obj>
        $<TARGET_OBJECTS:thread_obj>
        $<TARGET_OBJECTS:thrift_obj>
        $<TARGET_OBJECTS:jit_obj>
        $<TARGET_OBJECTS:expression_obj>
        $<TARGET_OBJECTS:fs_obj>
        $<TARGET_OBJECTS:datatypes_obj>
        $<TARGET_OBJECTS:base_obj>
    LIBRARIES
        ${JIT_TEST_LIBS}
        wangle
)
