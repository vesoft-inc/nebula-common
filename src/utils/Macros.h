/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef UTILS_MACROS_H_
#define UTILS_MACROS_H_

#if (__cplusplus == 201703L)

#include <folly/CppAttributes.h>

#define MAYBE_UNUSED FOLLY_MAYBE_UNUSED

#else

#define MAYBE_UNUSED

#endif

#endif   // UTILS_MACROS_H_
