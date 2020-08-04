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
#   ENABLE_NATIVE                   -- Build native client
#   ENABLE_TESTING                  -- Build unit test
#   ENABLE_PACK_ONE                 -- Package to one or multi packages
#
# CMake version check
cmake_minimum_required(VERSION 3.5.0)

# Set the project name
project("Nebula Common" C CXX)
set(PACKAGE_NAME "nebula-common")
add_definitions(-DNEBULA_HOME=${CMAKE_SOURCE_DIR})

# To include customized CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/nebula")

<<<<<<< HEAD
<<<<<<< HEAD
include(PlatformCheck)
include(GeneralCMakeOptions)
include(GeneralCMakeConfig)
include(GeneralCompilerConfig)
include(LinkerConfig)
include(CcacheConfig)
include(ThirdPartyConfig)
include(SanitizerConfig)
include(GitHooksConfig)
include(GitInfoConfig)
include(NebulaCMakeMacros)
include(NebulaCustomTargets)
=======
if(${CMAKE_SYSTEM_NAME} MATCHES "(Darwin|FreeBSD|Windows)")
    MESSAGE(FATAL_ERROR "Only Linux is supported")
endif ()
>>>>>>> update

# Remove the target exporting file
file(REMOVE ${CMAKE_BINARY_DIR}/${PACKAGE_NAME}-config.cmake)

# For simplicity, we make all ordinary libraries depend on the compile-time generated files,
# including the precompiled header, a.k.a Base.h.gch, and thrift headers.
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
        common_thrift_headers
        graph_thrift_headers
        storage_thrift_headers
        meta_thrift_headers
        raftex_thrift_headers
    )
endmacro()

add_subdirectory(src/common)
=======
# To include customized FindXXX.cmake modules
set(CMAKE_MODULE_PATH "${NEBULA_HOME}/cmake" ${CMAKE_MODULE_PATH})

include(GetGitInfo)
include(BuildOption)

find_package(Bzip2 REQUIRED)
find_package(DoubleConversion REQUIRED)
find_package(Fbthrift REQUIRED)
find_package(Folly REQUIRED)
find_package(Gflags REQUIRED)
find_package(Glog REQUIRED)
find_package(Googletest REQUIRED)
if(ENABLE_JEMALLOC)
    find_package(Jemalloc REQUIRED)
endif()
find_package(Libevent REQUIRED)
find_package(Mstch REQUIRED)
find_package(Proxygen REQUIRED)
find_package(Snappy REQUIRED)
find_package(Wangle REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Zstd REQUIRED)
find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Krb5 REQUIRED gssapi)
find_package(GPERF 2.8 REQUIRED)
find_package(Libunwind REQUIRED)
find_package(BISON 3.0.5 REQUIRED)
include(MakeBisonRelocatable)
find_package(FLEX REQUIRED)
find_package(LibLZMA MODULE)

include(BuildMacro)

add_subdirectory(src/common)

include(CleanBuild)
>>>>>>> make CMakeLists modular
