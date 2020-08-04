# Copyright (c) 2020 vesoft inc. All rights reserved.
#
# This source code is licensed under Apache 2.0 License,
# attached with Common Clause Condition 1.0, found in the LICENSES directory.
#

include(LinkerConfig)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -no-pie")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

set(CMAKE_SKIP_RPATH TRUE)

option(ENABLE_JEMALLOC "Whether to link jemalloc to all executables" ON)
option(ENABLE_NATIVE "Whether to build native client" OFF)
option(ENABLE_CCACHE "Whether to use ccache to speed up compiling" ON)
option(ENABLE_ASAN "Whether to turn AddressSanitizer ON or OFF" OFF)
option(ENABLE_TESTING "Whether to turn unit test ON or OFF" ON)
option(ENABLE_COVERAGE "Whether to turn unit test coverage ON or OFF" OFF)
option(ENABLE_UBSAN "Whether to turn Undefined Behavior Sanitizer ON or OFF" OFF)
option(ENABLE_FUZZ_TEST "Whether to turn Fuzz Test ON or OFF" OFF)
option(ENABLE_WERROR "Whether to error on warnings" ON)
option(ENABLE_STRICT_ALIASING "Whether to turn strict-aliasing ON or OFF" OFF)
option(ENABLE_TSAN "Whether to turn Thread Sanitizer ON or OFF" OFF)
option(ENABLE_STATIC_ASAN "Whether directs the GCC driver to link libasan statically" OFF)
option(ENABLE_STATIC_UBSAN "Whether directs the GCC driver to link libubsan statically" OFF)
option(ENABLE_PACK_ONE "Whether to package into one" ON)
option(ENABLE_PCH "Whether to enable use of precompiled header" OFF)
option(ENABLE_FRAME_POINTER "Whether to build with -fno-omit-frame-pointer" ON)
option(NEED_COMMON "Whether to need common" OFF)

message(STATUS "ENABLE_ASAN: ${ENABLE_ASAN}")
message(STATUS "ENABLE_TESTING: ${ENABLE_TESTING}")
message(STATUS "ENABLE_UBSAN: ${ENABLE_UBSAN}")
message(STATUS "ENABLE_FUZZ_TEST: ${ENABLE_FUZZ_TEST}")
message(STATUS "ENABLE_TSAN: ${ENABLE_TSAN}")
message(STATUS "ENABLE_STATIC_ASAN : ${ENABLE_STATIC_ASAN}")
message(STATUS "ENABLE_STATIC_UBSAN : ${ENABLE_STATIC_UBSAN}")
message(STATUS "ENABLE_WERROR: ${ENABLE_WERROR}")
message(STATUS "ENABLE_STRICT_ALIASING: ${ENABLE_STRICT_ALIASING}")
message(STATUS "ENABLE_PCH: ${ENABLE_PCH}")
message(STATUS "ENABLE_FRAME_POINTER: ${ENABLE_FRAME_POINTER}")
message(STATUS "NEED_COMMON: ${NEED_COMMON}")

if (ENABLE_NATIVE)
    message(STATUS "ENABLE_NATIVE is ${ENABLE_NATIVE}")
    add_compile_options(-fPIC)
endif()

if (ENABLE_TESTING)
    enable_testing()
endif()

if (!CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "No C++ compiler found")
endif()

set(CMAKE_CXX_STANDARD 14)  # specify the C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# coverage
if (ENABLE_TESTING AND ENABLE_COVERAGE)
    add_compile_options(--coverage)
    add_compile_options(-g)
    add_compile_options(-O0)

    set(COVERAGES --coverage)
endif()

# To detect if ccache is available
find_program(ccache_program_found "ccache")
if (ENABLE_CCACHE AND ccache_program_found)
    message(STATUS "ENABLE_CCACHE: ON")
    if (NOT $ENV{CCACHE_DIR} STREQUAL "")
        message(STATUS "CCACHE_DIR: $ENV{CCACHE_DIR}")
    else()
        message(STATUS "CCACHE_DIR: $ENV{HOME}/.ccache")
    endif()
    set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")
elseif (ENABLE_CCACHE)
    message(STATUS "CCACHE: enabled but not found")
    set(CMAKE_CXX_COMPILER_LAUNCHER)
else ()
    message(STATUS "CCACHE: OFF")
    set(CMAKE_CXX_COMPILER_LAUNCHER)
endif()

# Possible values are Debug, Release, RelWithDebInfo, MinSizeRel
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif(NOT CMAKE_BUILD_TYPE)

if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message(STATUS "Set macro _FORTIFY_SOURCE=2")
    add_definitions(-D_FORTIFY_SOURCE=2)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--compress-debug-sections=zlib")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-fno-limit-debug-info)
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (NOT ${NEBULA_USE_LINKER} STREQUAL "gold")
        # gold linker is buggy for `--gc-sections', disabled for now
        add_compile_options(-ffunction-sections)
        add_compile_options(-fdata-sections)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
    endif()
endif()

message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE} "
        "(Options are: Debug, Release, RelWithDebInfo, MinSizeRel)")

# By default, all dynamic and static libraries will be placed at ${CMAKE_BINARY_DIR}/lib,
# while all executables at ${CMAKE_BINARY_DIR}/bin.
# But for the sake of cleanliness, all executables ending with `_test' will be placed
# at ${CMAKE_BINARY_DIR}/bin/test, while those ending with `_bm' at ${CMAKE_BINARY_DIR}/bin/bench.
# Please see `nebula_add_executable' for this rule.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_definitions(-DNEBULA_HOME=${NEBULA_HOME})

# Let s2 use glog
add_definitions(-DS2_USE_GLOG)

if (NEBULA_BUILD_VERSION)
    add_definitions(-DNEBULA_BUILD_VERSION=${NEBULA_BUILD_VERSION})
endif()

if (${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/local")
    set(CMAKE_INSTALL_PREFIX "/usr/local/nebula")
endif()
message(STATUS "CMAKE_INSTALL_PREFIX: " ${CMAKE_INSTALL_PREFIX})

# The precedence to decide NEBULA_THIRDPARTY_ROOT is:
#   1. The path defined with CMake argument, i.e -DNEBULA_THIRDPARTY_ROOT=path
#   2. ${CMAKE_BINARY_DIR}/third-party/install, if exists
#   3. The path specified with environment variable NEBULA_THIRDPARTY_ROOT=path
#   4. /opt/vesoft/third-party, if exists
#   5. At last, one copy will be downloaded and installed to ${CMAKE_BINARY_DIR}/third-party/install
if("${NEBULA_THIRDPARTY_ROOT}" STREQUAL "")
    if(EXISTS ${CMAKE_BINARY_DIR}/third-party/install)
        SET(NEBULA_THIRDPARTY_ROOT ${CMAKE_BINARY_DIR}/third-party/install)
    elseif(NOT $ENV{NEBULA_THIRDPARTY_ROOT} STREQUAL "")
        SET(NEBULA_THIRDPARTY_ROOT $ENV{NEBULA_THIRDPARTY_ROOT})
    elseif(EXISTS /opt/vesoft/third-party)
        SET(NEBULA_THIRDPARTY_ROOT "/opt/vesoft/third-party")
    else()
        include(InstallThirdParty)
    endif()
endif()

# third-party
if(NOT ${NEBULA_THIRDPARTY_ROOT} STREQUAL "")
    message(STATUS "Specified NEBULA_THIRDPARTY_ROOT: " ${NEBULA_THIRDPARTY_ROOT})
    list(INSERT CMAKE_INCLUDE_PATH 0 ${NEBULA_THIRDPARTY_ROOT}/include)
    list(INSERT CMAKE_LIBRARY_PATH 0 ${NEBULA_THIRDPARTY_ROOT}/lib)
    list(INSERT CMAKE_LIBRARY_PATH 0 ${NEBULA_THIRDPARTY_ROOT}/lib64)
    list(INSERT CMAKE_PROGRAM_PATH 0 ${NEBULA_THIRDPARTY_ROOT}/bin)
    include_directories(SYSTEM ${NEBULA_THIRDPARTY_ROOT}/include)
    link_directories(
            ${NEBULA_THIRDPARTY_ROOT}/lib
            ${NEBULA_THIRDPARTY_ROOT}/lib64
    )
endif()

if(NOT ${NEBULA_OTHER_ROOT} STREQUAL "")
    string(REPLACE ":" ";" DIR_LIST ${NEBULA_OTHER_ROOT})
    list(LENGTH DIR_LIST len)
    foreach(DIR IN LISTS DIR_LIST )
        list(INSERT CMAKE_INCLUDE_PATH 0 ${DIR}/include)
        list(INSERT CMAKE_LIBRARY_PATH 0 ${DIR}/lib)
        list(INSERT CMAKE_PROGRAM_PATH 0 ${DIR}/bin)
        include_directories(SYSTEM ${DIR}/include)
        link_directories(${DIR}/lib)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L ${DIR}/lib")
    endforeach()
endif()

string(REPLACE ";" ":" INCLUDE_PATH_STR "${CMAKE_INCLUDE_PATH}")
string(REPLACE ";" ":" LIBRARY_PATH_STR "${CMAKE_LIBRARY_PATH}")
string(REPLACE ";" ":" PROGRAM_PATH_STR "${CMAKE_PROGRAM_PATH}")
message(STATUS "CMAKE_INCLUDE_PATH: " ${INCLUDE_PATH_STR})
message(STATUS "CMAKE_LIBRARY_PATH: " ${LIBRARY_PATH_STR})
message(STATUS "CMAKE_PROGRAM_PATH: " ${PROGRAM_PATH_STR})

if(ENABLE_PCH AND NOT ENABLE_ASAN AND NOT ENABLE_NATIVE)
    find_package(PCHSupport)
    add_compile_options(-Winvalid-pch)
else()
    # Remove if existing
    file(REMOVE ${CMAKE_SOURCE_DIR}/src/common/base/Base.h.gch)
endif()

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Wpedantic)
add_compile_options(-Wunused-parameter)
add_compile_options(-Wshadow)
add_compile_options(-Wnon-virtual-dtor)
add_compile_options(-Woverloaded-virtual)
add_compile_options(-Wignored-qualifiers)
if(ENABLE_WERROR)
    add_compile_options(-Werror)
endif()

if(NOT ENABLE_STRICT_ALIASING)
    add_compile_options(-fno-strict-aliasing)
endif()

if(ENABLE_FRAME_POINTER)
    add_compile_options(-fno-omit-frame-pointer)
else()
    add_compile_options(-fomit-frame-pointer)
endif()

include(CheckCXXCompilerFlag)

# TODO(dutor) Move all compiler-config-related operations to a separate file
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # This requries GCC 5.1+
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.1)
        add_compile_options(-Wsuggest-override)
    endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    MESSAGE(STATUS "NEBULA_CLANG_USE_GCC_TOOLCHAIN: ${NEBULA_CLANG_USE_GCC_TOOLCHAIN}")
    # Here we need to specify the path of libstdc++ used by Clang
    if(NOT ${NEBULA_CLANG_USE_GCC_TOOLCHAIN} STREQUAL "")
        execute_process(
                COMMAND ${NEBULA_CLANG_USE_GCC_TOOLCHAIN}/bin/gcc -dumpmachine
                OUTPUT_VARIABLE gcc_target_triplet
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        add_compile_options("--gcc-toolchain=${NEBULA_CLANG_USE_GCC_TOOLCHAIN}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --gcc-toolchain=${NEBULA_CLANG_USE_GCC_TOOLCHAIN}")
        if(NOT "${gcc_target_triplet}" STREQUAL "")
            add_compile_options(--target=${gcc_target_triplet})
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --target=${gcc_target_triplet}")
        endif()
    endif()
    add_compile_options(-Wno-self-assign-overloaded)
    add_compile_options(-Wno-self-move)
    add_compile_options(-Wno-format-pedantic)
    add_compile_options(-Wno-gnu-zero-variadic-macro-arguments)
    set(libatomic_link_flags "-Wl,-Bstatic -latomic -Wl,-Bdynamic")
    set(CMAKE_REQUIRED_FLAGS "${libatomic_link_flags}")
    check_cxx_compiler_flag("${libatomic_link_flags}" has_static_libatomic)
    if(NOT has_static_libatomic)
        set(libatomic_link_flags "-latomic")
    endif()
endif()

if(ENABLE_ASAN OR ENABLE_UBSAN)
    add_definitions(-DBUILT_WITH_SANITIZER)
endif()

if(ENABLE_ASAN)
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
    check_cxx_compiler_flag("-fsanitize=address" ENABLE_ASAN_OK)
    if (NOT ENABLE_ASAN_OK)
        MESSAGE(FATAL_ERROR "The compiler does not support address sanitizer")
    endif()
    if(ENABLE_STATIC_ASAN AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-static-libasan)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libasan")
    endif()
    add_compile_options(-fsanitize=address)
    add_compile_options(-g)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

if(ENABLE_TSAN)
    if (ENABLE_ASAN)
        MESSAGE(FATAL_ERROR "ENABLE_TSAN cannot be combined with ENABLE_ASAN")
    endif()
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=thread")
    check_cxx_compiler_flag("-fsanitize=thread" ENABLE_TSAN_OK)
    if (NOT ENABLE_TSAN_OK)
        MESSAGE(FATAL_ERROR "The compiler does not support thread sanitizer")
    endif()
    set(ENV{TSAN_OPTIONS} "report_atomic_races=0")
    add_compile_options(-fsanitize=thread)
    add_compile_options(-g)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")
endif()

if(ENABLE_UBSAN)
    check_cxx_compiler_flag("-fsanitize=undefined -fno-sanitize=alignment" ENABLE_UBSAN_OK)
    if (NOT ENABLE_UBSAN_OK)
        MESSAGE(FATAL_ERROR "The compiler does not support Undefined Behavior Sanitizer")
    endif()
    if(ENABLE_STATIC_UBSAN AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-static-libubsan)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libubsan")
    endif()
    add_compile_options(-fsanitize=undefined -fno-sanitize=alignment)
    add_compile_options(-fno-sanitize-recover=all)  # Exit on failure
    # TODO(dutor) Remove the following line when RTTI-enabled RocksDB is ready
    add_compile_options(-fno-sanitize=vptr)
    if(NOT ENABLE_ASAN)
        add_compile_options(-g)
    endif()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
endif()

if(ENABLE_FUZZ_TEST)
    check_cxx_compiler_flag("-fsanitize=fuzzer" ENABLE_FUZZ_OK)
    if (NOT ENABLE_FUZZ_OK)
        MESSAGE(FATAL_ERROR "The compiler does not support fuzz testing")
    endif()
endif()

