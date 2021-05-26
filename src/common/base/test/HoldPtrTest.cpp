/* Copyright (c) 2021 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <memory>

#include <gtest/gtest.h>

#include "common/base/HoldPtr.h"

namespace nebula {

TEST(HoldPtrTest, Basic) {
    {
        auto str = std::make_unique<std::string>("Hello World.");
        HoldPtr<std::string> hp(str.get());

        std::string str1 = *hp;

        hp->size();

        EXPECT_EQ(hp.get(), str.get());

        auto hp1 = hp;

        EXPECT_EQ(hp.get(), hp1.get());

        auto hp2 = std::move(hp1);

        EXPECT_EQ(hp1.get(), nullptr);
        EXPECT_EQ(hp.get(), hp2.get());
    }
    {
        // compile error
        // HoldPtr<std::string&> hp{nullptr};
    }
}

}   // namespace nebula
