/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_UUIDEXPRESSION_H_
#define COMMON_EXPRESSION_UUIDEXPRESSION_H_

#include "common/expression/Expression.h"

namespace nebula {

class UUIDExpression final : public Expression {
public:
    explicit UUIDExpression(std::string* field = nullptr)
        : Expression(Kind::kUUID)
        , field_(field) {}

    bool operator==(const Expression& rhs) const override;

    Value eval(const ExpressionContext& ctx) const override;

    size_t encode(std::string& buf) const override;

    void resetFrom(char*& ptr, const char* end) override;

    std::string toString() const override {
        // TODO
        return "";
    }


protected:
    std::unique_ptr<std::string> field_;
};

}   // namespace nebula
#endif  // EXPRESSION_UUIDEXPRESSION_H_
