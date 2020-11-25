/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

class DataTest : public ::testing::Test {
protected:
    template <typename T>
    static ::testing::AssertionResult verifyVectorWithoutOrder(const std::vector<T> &l,
                                                               const std::vector<T> &r) {
        std::vector<T> lCopy = l;
        std::vector<T> rCopy = r;
        std::sort(lCopy.begin(), lCopy.end());
        std::sort(rCopy.begin(), rCopy.end());
        if (lCopy == rCopy) {
            return ::testing::AssertionSuccess();
        }
        return ::testing::AssertionFailure();
    }
};
