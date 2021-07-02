# Copyright (c) 2018 vesoft inc. All rights reserved.
#
# This source code is licensed under Apache 2.0 License,
# attached with Common Clause Condition 1.0, found in the LICENSES directory.
#
# CMakeLists.txt file contains a set of directives and instructions describing
# the project's source files and targets (executable, library, or both).
#
# The building can be controlled by defining the following variables on the
# <cmake> command line.
#
#   CMAKE_C_COMPILER                -- Specify the compiler for C language
#   CMAKE_CXX_COMPILER              -- Specify the compiler for C++ language
#
#   NEBULA_THIRDPARTY_ROOT          -- Specify the root directory for third-party
#   NEBULA_OTHER_ROOT               -- Specify the root directory for user building
#                                   -- Split with ":", exp: DIR:DIR
#   NEBULA_CLANG_USE_GCC_TOOLCHAIN  -- Specify the root of GCC's toolchain, used by Clang
#   NEBULA_USE_LINKER               -- Set linker to use, options are: bfd, gold
#
#   ENABLE_JEMALLOC                 -- Link jemalloc into all executables
#   ENABLE_TESTING                  -- Build unit test
#   ENABLE_PACK_ONE                 -- Package to one or multi packages
#
# CMake version check
cmake_minimum_required(VERSION 3.9.0)

# Set the project name
project("Nebula Common" C CXX)
set(PACKAGE_NAME "nebula-common")
add_definitions(-DNEBULA_HOME=${CMAKE_SOURCE_DIR})

# To include customized CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/nebula")

set(nebula_common_source_dir ${CMAKE_SOURCE_DIR})

include(PlatformCheck)
include(NebulaCMakeMacros)
include(GeneralCMakeOptions)
include(ABIConfig)
include(GeneralCMakeConfig)
include(GeneralCompilerConfig)
include(LinkerConfig)
include(CcacheConfig)
include(ThirdPartyConfig)
include(SanitizerConfig)
include(GitHooksConfig)
include(GitInfoConfig)
include(NebulaCustomTargets)
include(ClangTidy)

if (NEBULA_BUILD_VERSION)
    add_definitions(-DNEBULA_BUILD_VERSION=${NEBULA_BUILD_VERSION})
endif()

# Remove the target exporting file
file(REMOVE ${CMAKE_BINARY_DIR}/${PACKAGE_NAME}-config.cmake)

# For simplicity, we make all ordinary libraries depend on the build-time generated files
macro(nebula_add_library name type)
    add_library(${name} ${type} ${ARGN})
    export(
        TARGETS ${name}
        NAMESPACE "common_"
        APPEND
        FILE ${CMAKE_BINARY_DIR}/${PACKAGE_NAME}-config.cmake
    )
    add_dependencies(
        ${name}
        common_thrift_generator
        graph_thrift_generator
        storage_thrift_generator
        meta_thrift_generator
        raftex_thrift_generator
    )
endmacro()

add_subdirectory(src/common)
