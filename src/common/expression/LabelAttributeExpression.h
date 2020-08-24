/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_LABELATTRIBUTEEXPRESSION_H_
#define COMMON_EXPRESSION_LABELATTRIBUTEEXPRESSION_H_

#include "common/expression/BinaryExpression.h"
#include "common/expression/LabelExpression.h"

namespace nebula {

// label.label
class LabelAttributeExpression final : public BinaryExpression {
public:
    explicit LabelAttributeExpression(Expression *lhs = nullptr,
                                      Expression *rhs = nullptr)
        : BinaryExpression(Kind::kLabelAttribute, lhs, rhs) {}

    const Value& eval(ExpressionContext&) override {
        LOG(FATAL) << "LabelAttributeExpression has to be rewritten";
    }

    const std::string *leftLabel() const {
        return static_cast<const LabelExpression*>(left())->name();
    }

    const std::string *rightLabel() const {
        return static_cast<const LabelExpression*>(right())->name();
    }

    std::string toString() const override;
};

}   // namespace nebula

#endif  // COMMON_EXPRESSION_LABELATTRIBUTEEXPRESSION_H_
