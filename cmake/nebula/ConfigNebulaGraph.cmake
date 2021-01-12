# Copyright (c) 2020 vesoft inc. All rights reserved.
#
# This source code is licensed under Apache 2.0 License,
# attached with Common Clause Condition 1.0, found in the LICENSES directory.
#

macro(config_nebula_graph)
    message(">>>> Configuring Nebula Graph <<<<")
    cmake_parse_arguments(
        graph
        ""
        "SOURCE_DIR;BUILD_DIR;COMMON_SOURCE_DIR;COMMON_BUILD_DIR"
        ""
        ${ARGN}
    )
    set(graph_source_dir ${graph_SOURCE_DIR})
    set(graph_build_dir ${graph_BUILD_DIR})
    if(NOT EXISTS ${graph_build_dir})
        file(MAKE_DIRECTORY ${graph_build_dir})
    endif()

    if (NEBULA_BUILD_VERSION)
        message(STATUS "NEBULA_BUILD_VERSION: " ${NEBULA_BUILD_VERSION})
        set(graph_build_version -DNEBULA_BUILD_VERSION=${NEBULA_BUILD_VERSION})
    endif()

    execute_process(
        COMMAND
            ${CMAKE_COMMAND}
                -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                -DNEBULA_THIRDPARTY_ROOT=${NEBULA_THIRDPARTY_ROOT}
                -DNEBULA_OTHER_ROOT=${NEBULA_OTHER_ROOT}
                -DENABLE_JEMALLOC=${ENABLE_JEMALLOC}
                -DENABLE_TESTING=OFF
                -DENABLE_CCACHE=${ENABLE_CCACHE}
                -DENABLE_ASAN=${ENABLE_ASAN}
                -DENABLE_UBSAN=${ENABLE_UBSAN}
                -DENABLE_FRAME_POINTER=${ENABLE_FRAME_POINTER}
                -DNEBULA_COMMON_SOURCE_DIR=${graph_COMMON_SOURCE_DIR}
                -DNEBULA_COMMON_BUILD_DIR=${graph_COMMON_BUILD_DIR}
                -DENABLE_PIC=${ENABLE_PIC}
                -DENABLE_COMPRESSED_DEBUG_INFO=${ENABLE_COMPRESSED_DEBUG_INFO}
                -DNEBULA_USE_LINKER=${NEBULA_USE_LINKER}
                -DENABLE_GDB_SCRIPT_SECTION=${ENABLE_GDB_SCRIPT_SECTION}
                ${graph_build_version}
                ${graph_source_dir}
        WORKING_DIRECTORY ${graph_build_dir}
        RESULT_VARIABLE cmake_status
    )

    if(NOT ${cmake_result} EQUAL 0)
        message(FATAL_ERROR "Failed to configure nebula-graph")
    endif()

    add_custom_target(
        graph_project ALL
        COMMAND +${CMAKE_COMMAND} --build ${graph_build_dir}
    )
    add_dependencies(graph_project common_project)
    add_custom_target(
        install-graph
        COMMAND $(MAKE) install
        WORKING_DIRECTORY ${graph_build_dir}
    )
    add_custom_target(
        clean-graph
        COMMAND $(MAKE) clean
        WORKING_DIRECTORY ${graph_build_dir}
    )
    message(">>>> Finished configuring Nebula Graph <<<<")
endmacro()
