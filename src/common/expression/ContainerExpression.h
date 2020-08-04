/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_CONTAINEREXPRESSION_H_
#define COMMON_EXPRESSION_CONTAINEREXPRESSION_H_

#include "common/expression/Expression.h"

namespace nebula {

class ExpressionList final {
public:
    ExpressionList() = default;

    ExpressionList& add(Expression *expr) {
        list_.emplace_back(expr);
        return *this;
    }

    auto get() && {
        return std::move(list_);
    }

private:
    std::vector<std::unique_ptr<Expression>>    list_;
};


class MapItemList final {
public:
    MapItemList() = default;

    MapItemList& add(std::string *key, Expression *value) {
        list_.emplace_back(key, value);
        return *this;
    }

    auto get() && {
        return std::move(list_);
    }

private:
    using Pair = std::pair<std::unique_ptr<std::string>, std::unique_ptr<Expression>>;
    std::vector<Pair>                           list_;
};


class ListExpression final : public Expression {
public:
    ListExpression() : Expression(Kind::kList) {
    }

    explicit ListExpression(ExpressionList *list) : Expression(Kind::kList) {
        list_ = std::move(*list).get();
        delete list;
    }

    const Value& eval(ExpressionContext &ctx) override;

    size_t size() const {
        return list_.size();
    }

    bool operator==(const Expression &rhs) const override;

    std::string toString() const override;

private:
    void writeTo(Encoder &encoder) const override;

    void resetFrom(Decoder &decoder) override;

private:
    std::vector<std::unique_ptr<Expression>>    list_;
    Value                                       result_;
};


class SetExpression final : public Expression {
public:
    SetExpression() : Expression(Kind::kSet) {
    }

    explicit SetExpression(ExpressionList *list) : Expression(Kind::kSet) {
        list_ = std::move(*list).get();
        delete list;
    }

    const Value& eval(ExpressionContext &ctx) override;

    size_t size() const {
        return list_.size();
    }

    bool operator==(const Expression &rhs) const override;

    std::string toString() const override;

private:
    void writeTo(Encoder &encoder) const override;

    void resetFrom(Decoder &decoder) override;

private:
    std::vector<std::unique_ptr<Expression>>    list_;
    Value                                       result_;
};


class MapExpression final : public Expression {
public:
    MapExpression() : Expression(Kind::kMap) {
    }

    explicit MapExpression(MapItemList *list) : Expression(Kind::kMap) {
        list_ = std::move(*list).get();
        delete list;
    }

    const Value& eval(ExpressionContext &ctx) override;

    size_t size() const {
        return list_.size();
    }

    bool operator==(const Expression &rhs) const override;

    std::string toString() const override;

private:
    void writeTo(Encoder &encoder) const override;

    void resetFrom(Decoder &decoder) override;

private:
    using Pair = std::pair<std::unique_ptr<std::string>, std::unique_ptr<Expression>>;
    std::vector<Pair>                       list_;
    Value                                   result_;
};

}   // namespace nebula

#endif  // COMMON_EXPRESSION_CONTAINEREXPRESSION_H_
