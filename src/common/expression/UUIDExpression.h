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
    explicit UUIDExpression(std::string* field) : Expression(Kind::kUUID) {
        field_.reset(field);
    }

    void setExpCtxt(ExpressionContext* ctxt) override {
        expCtxt_ = ctxt;
    }

    Value eval() const override;

    std::string encode() const override {
        // TODO
        return "";
    }

    std::string decode() const override {
        // TODO
        return "";
    }

    std::string toString() const override {
        // TODO
        return "";
    }

private:
    std::unique_ptr<std::string> field_;
};
}   // namespace nebula
#endif
