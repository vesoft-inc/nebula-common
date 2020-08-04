# Copyright (c) 2020 vesoft inc. All rights reserved.
#
# This source code is licensed under Apache 2.0 License,
# attached with Common Clause Condition 1.0, found in the LICENSES directory.
#

if(${PACKAGE_NAME} STREQUAL "nebula-common")
    set(NEBULA_CLEAN_ALL_DEPS clean-interface clean-pch clean-config clean-bin clean-lib)
else()
    set(NEBULA_CLEAN_ALL_DEPS clean-modules clean-bin clean-lib)
endif()

add_custom_target(
    clean-config
    COMMAND "rm" "-fr" "nebula-common-config.cmake"
)

add_custom_target(
    clean-bin
    COMMAND "rm" "-fr" "bin"
)

add_custom_target(
    clean-lib
    COMMAND "rm" "-fr" "lib"
)

add_custom_target(
    clean-modules
    COMMAND "rm" "-fr" "modules/*"
)

add_custom_target(
    clean-all
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    COMMAND "find" "." "-name" "Testing" "|" "xargs" "rm" "-fr"
    DEPENDS ${NEBULA_CLEAN_ALL_DEPS}
)

add_custom_target(
    distclean
    COMMAND "find" "." "-name" "CMakeFiles" "|" "xargs" "rm" "-fr"
    COMMAND "find" "." "-name" "CMakeCache.txt" "|" "xargs" "rm" "-f"
    COMMAND "find" "." "-name" "cmake_install.cmake" "|" "xargs" "rm" "-f"
    COMMAND "find" "." "-name" "CTestTestfile.cmake" "|" "xargs" "rm" "-f"
    COMMAND "find" "." "-name" "CPackConfig.cmake" "|" "xargs" "rm" "-f"
    COMMAND "find" "." "-name" "CPackSourceConfig.cmake" "|" "xargs" "rm" "-f"
    COMMAND "find" "." "-name" "Makefile" "|" "xargs" "rm" "-f"
    DEPENDS clean-all
)
