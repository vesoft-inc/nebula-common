# Copyright (c) 2020 vesoft inc. All rights reserved.
#
# This source code is licensed under Apache 2.0 License,
# attached with Common Clause Condition 1.0, found in the LICENSES directory.
#

find_package(Git)
if (GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    execute_process(
        COMMAND
        ${GIT_EXECUTABLE} rev-parse --short HEAD
        OUTPUT_VARIABLE GIT_INFO_SHA
    )
endif()

if (GIT_INFO_SHA)
    string(REGEX REPLACE "[^0-9a-f]+" "" GIT_INFO_SHA "${GIT_INFO_SHA}")
    add_definitions(-DGIT_INFO_SHA=${GIT_INFO_SHA})
endif()

IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git/")
    # Create the pre-commit hook every time we run cmake
    message(STATUS "Create the pre-commit hook")
    set(PRE_COMMIT_HOOK ${CMAKE_CURRENT_SOURCE_DIR}/.git/hooks/pre-commit)
    execute_process(
        COMMAND
        "rm" "-f" ${PRE_COMMIT_HOOK}
    )
    execute_process(
        COMMAND
        "ln" "-s" ${CMAKE_CURRENT_SOURCE_DIR}/.linters/cpp/hooks/pre-commit.sh ${PRE_COMMIT_HOOK}
        RESULT_VARIABLE retcode
    )
    IF(${retcode} EQUAL 0)
        MESSAGE(STATUS "Creating pre-commit hook done")
    ELSE()
        MESSAGE(FATAL_ERROR "Creating pre-commit hook failed: ${retcode}")
    ENDIF()
ENDIF()
