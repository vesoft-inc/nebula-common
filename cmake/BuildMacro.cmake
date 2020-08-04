set(COMMON_REPO "nebula-common")
set(NEBULA_REPO "nebula-nebula")
set(STORAGE_REPO "nebula-storage")

macro(nebula_add_executable)
    cmake_parse_arguments(
        nebula_exec                 # prefix
        ""                          # <options>
        "NAME"                      # <one_value_args>
        "SOURCES;OBJECTS;LIBRARIES" # <multi_value_args>
        ${ARGN}
    )
    add_executable(
        ${nebula_exec_NAME}
        ${nebula_exec_SOURCES}
        ${nebula_exec_OBJECTS}
    )
    nebula_link_libraries(
        ${nebula_exec_NAME}
        ${nebula_exec_LIBRARIES}
    )

    if(NOT ${PACKAGE_NAME} STREQUAL ${COMMON_REPO}) 
        add_dependencies(
            ${nebula_exec_NAME}
            common_project
        )
    endif()

    if(${nebula_exec_NAME} MATCHES "_test$")
        set_target_properties(
            ${nebula_exec_NAME}
            PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/test
        )
    elseif(${nebula_exec_NAME} MATCHES "_bm$")
        set_target_properties(
            ${nebula_exec_NAME}
            PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/bench
        )
    endif()
endmacro()

macro(nebula_add_test)
    cmake_parse_arguments(
        nebula_test                 # prefix
        "DISABLED;FUZZER"           # <options>
        "NAME"                      # <one_value_args>
        "SOURCES;OBJECTS;LIBRARIES" # <multi_value_args>
        ${ARGN}
    )

    nebula_add_executable(
        NAME ${nebula_test_NAME}
        SOURCES ${nebula_test_SOURCES}
        OBJECTS ${nebula_test_OBJECTS}
        LIBRARIES ${nebula_test_LIBRARIES}
    )

    if (${nebula_test_FUZZER})
        #Currently only Clang supports fuzz test
        if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
            set_target_properties(${nebula_test_NAME} PROPERTIES COMPILE_FLAGS "-g -fsanitize=fuzzer")
            set_target_properties(${nebula_test_NAME} PROPERTIES LINK_FLAGS "-fsanitize=fuzzer")
        endif()
    elseif (NOT ${nebula_test_DISABLED})
        string(REGEX REPLACE "${CMAKE_SOURCE_DIR}/src/(.*)/test" "\\1" test_group ${CMAKE_CURRENT_SOURCE_DIR})
        add_test(NAME ${nebula_test_NAME} COMMAND ${nebula_test_NAME})
        set_tests_properties(${nebula_test_NAME} PROPERTIES LABELS ${test_group})
        # e.g. cmake -DNEBULA_ASAN_PRELOAD=/path/to/libasan.so
        # or,  cmake -DNEBULA_ASAN_PRELOAD=`/path/to/gcc --print-file-name=libasan.so`
        if (NEBULA_ASAN_PRELOAD)
            set_property(
                TEST ${nebula_test_NAME}
                PROPERTY ENVIRONMENT LD_PRELOAD=${NEBULA_ASAN_PRELOAD}
            )
        endif()
    endif()
endmacro()

# For simplicity, we make all ordinary libraries depend on the compile-time generated files,
# including the precompiled header, a.k.a Base.h.gch, and thrift headers.
macro(nebula_add_library name type)
    add_library(${name} ${type} ${ARGN})
    if (PCHSupport_FOUND)
        add_dependencies(
            ${name}
            base_obj_gch
        )
    endif()

    if(${PACKAGE_NAME} STREQUAL ${COMMON_REPO})
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
    elseif(${PACKAGE_NAME} STREQUAL ${NEBULA_REPO})
        message(STATUS "nebula repo nebula_add_library")
        add_dependencies(
            ${name}
            common_project
            parser_target
        )
    elseif(${PACKAGE_NAME} STREQUAL ${STORAGE_REPO})
        message(STATUS "storage repo nebula_add_library")
        add_dependencies(
            ${name}
            common_project
        )
    endif()
endmacro()

include_directories(AFTER ${NEBULA_HOME}/src)
include_directories(AFTER ${CMAKE_CURRENT_BINARY_DIR}/src)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L ${NEBULA_THIRDPARTY_ROOT}/lib")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L ${NEBULA_THIRDPARTY_ROOT}/lib64")

# All thrift libraries
set(THRIFT_LIBRARIES
    thriftcpp2
    thrift
    thriftprotocol
    async
    protocol
    transport
    concurrency
    security
    thriftfrozen2
    thrift-core
)

set(ROCKSDB_LIBRARIES ${Rocksdb_LIBRARY})

# All compression libraries
set(COMPRESSION_LIBRARIES bz2 snappy zstd z lz4)
if (LIBLZMA_FOUND)
    include_directories(SYSTEM ${LIBLZMA_INCLUDE_DIRS})
    list(APPEND COMPRESSION_LIBRARIES ${LIBLZMA_LIBRARIES})
endif()

if (NOT ENABLE_JEMALLOC OR ENABLE_ASAN OR ENABLE_UBSAN)
    set(JEMALLOC_LIB )
else()
    set(JEMALLOC_LIB jemalloc)
endif()

execute_process(
    COMMAND ldd --version
    COMMAND head -1
    COMMAND cut -d ")" -f 2
    COMMAND cut -d " " -f 2
    OUTPUT_VARIABLE GLIBC_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Glibc version is " ${GLIBC_VERSION})

if (GLIBC_VERSION VERSION_LESS "2.17")
    set(GETTIME_LIB rt)
else()
    set(GETTIME_LIB)
endif()

# A wrapper for target_link_libraries()
macro(nebula_link_libraries target)
    target_link_libraries(
        ${target}
        ${ARGN}
        folly
        glog
        gflags
        boost_context
        boost_system
        event
        double-conversion
        s2
        ${OPENSSL_SSL_LIBRARY}
        ${OPENSSL_CRYPTO_LIBRARY}
        ${KRB5_LIBRARIES}
        ${COMPRESSION_LIBRARIES}
        ${JEMALLOC_LIB}
        ${LIBUNWIND_LIBRARIES}
        keyutils
        resolv
        dl
        ${GETTIME_LIB}
        ${libatomic_link_flags}
        -pthread
        ${COVERAGES}
    )
endmacro(nebula_link_libraries)

function(nebula_add_subdirectory dir_name)
    if ((NOT ENABLE_TESTING) AND (${dir_name} STREQUAL test))
        add_subdirectory(${dir_name} EXCLUDE_FROM_ALL)
        return()
    endif()
    add_subdirectory(${dir_name})
endfunction()

