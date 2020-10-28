/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_CASEEXPRESSION_H_
#define COMMON_EXPRESSION_CASEEXPRESSION_H_

#include "common/expression/Expression.h"

namespace nebula {

class CaseList final {
public:
    struct Item {
        Item(Expression* wh, Expression* th) : when(wh), then(th) {}
        std::unique_ptr<Expression> when;
        std::unique_ptr<Expression> then;
    };
    CaseList() = default;

    explicit CaseList(size_t sz) {
        items_.reserve(sz);
    }

    void add(Expression* when, Expression* then) {
        items_.emplace_back(when, then);
    }

    auto items() && {
        return std::move(items_);
    }

private:
    std::vector<Item> items_;
};

class CaseExpression final : public Expression {
public:
    explicit CaseExpression(Kind kind) : Expression(kind) {}

    explicit CaseExpression(Kind kind, CaseList* cases) : Expression(kind) {
        cases_ = std::move(*cases).items();
        delete cases;
    }

    bool operator==(const Expression& rhs) const override;

    const Value& eval(ExpressionContext& ctx) override;

    void setCondition(Expression* cond) {
        condition_.reset(cond);
    }

    void setDefault(Expression* defaultResult) {
        default_.reset(defaultResult);
    }

    void setCases(std::vector<CaseList::Item> items) {
        cases_ = std::move(items);
    }

    void setWhen(size_t index, Expression* when) {
        DCHECK_LT(index, cases_.size());
        cases_[index].when.reset(when);
    }

    void setThen(size_t index, Expression* then) {
        DCHECK_LT(index, cases_.size());
        cases_[index].then.reset(then);
    }

    bool hasCondition() const {
        return condition_ != nullptr;
    }

    bool hasDefault() const {
        return default_ != nullptr;
    }

    size_t numCases() const {
        return cases_.size();
    }

    Expression* condition() const {
        return condition_.get();
    }

    Expression* defaultResult() const {
        return default_.get();
    }

    const std::vector<CaseList::Item>& cases() const {
        return cases_;
    }

    std::string toString() const override;

    void accept(ExprVisitor* visitor) override;

    std::unique_ptr<Expression> clone() const override {
        auto caseList = new CaseList(cases_.size());
        for (const auto& whenThen : cases_) {
            caseList->add(whenThen.when->clone().release(), whenThen.then->clone().release());
        }
        auto expr = std::make_unique<CaseExpression>(kind_, caseList);
        if (hasCondition()) {
            expr->setCondition(condition_->clone().release());
        }
        if (hasDefault()) {
            expr->setDefault(default_->clone().release());
        }
        return expr;
    }

private:
    void writeTo(Encoder& encoder) const override;

    void resetFrom(Decoder& decoder) override;

    std::unique_ptr<Expression> condition_{nullptr};
    std::unique_ptr<Expression> default_{nullptr};
    std::vector<CaseList::Item> cases_;
    Value result_;
};

}   // namespace nebula
#endif   // COMMON_EXPRESSION_CASEEXPRESSION_H_
