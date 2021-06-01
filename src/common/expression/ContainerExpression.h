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
    static ExpressionList* make(ObjectPool *pool = nullptr, size_t sz = 0) {
        DCHECK(!!pool);
        return pool->add(new ExpressionList(sz));
    }

    ExpressionList() = default;
    explicit ExpressionList(size_t sz) {
        items_.reserve(sz);
    }

    ExpressionList& add(Expression *expr) {
        items_.emplace_back(expr);
        return *this;
    }

    auto get() {
        return items_;
    }

private:
    std::vector<Expression*>    items_;
};


class MapItemList final {
public:
    static MapItemList* make(ObjectPool *pool = nullptr, size_t sz = 0) {
        DCHECK(!!pool);
        return pool->add(new MapItemList(sz));
    }

    MapItemList() = default;
    explicit MapItemList(size_t sz) {
        items_.reserve(sz);
    }

    MapItemList &add(const std::string &key, Expression *value) {
        items_.emplace_back(key, value);
        return *this;
    }

    auto get() {
        return items_;
    }

private:
    using Pair = std::pair<std::string, Expression*>;
    std::vector<Pair>                         items_;
};


class ListExpression final : public Expression {
public:
    static ListExpression *make(ObjectPool *pool = nullptr, ExpressionList *items = nullptr) {
        DCHECK(!!pool);
        return items == nullptr ? pool->add(new ListExpression(pool))
                                : pool->add(new ListExpression(pool, items));
    }

    explicit ListExpression(ObjectPool *pool) : Expression(pool, Kind::kList) {}

    explicit ListExpression(ObjectPool *pool, ExpressionList *items)
        : Expression(pool, Kind::kList) {
        items_ = items->get();
    }

    const Value& eval(ExpressionContext &ctx) override;

    const std::vector<Expression*> &items() const {
        return items_;
    }

    void setItem(size_t index, Expression *item) {
        DCHECK_LT(index, items_.size());
        items_[index] = item;
    }

    std::vector<Expression*> get() {
        return items_;
    }

    void setItems(std::vector<Expression*> items) {
        items_ = items;
    }

    size_t size() const {
        return items_.size();
    }

    bool operator==(const Expression &rhs) const override;

    std::string toString() const override;

    void accept(ExprVisitor *visitor) override;

    Expression* clone() const override {
        auto items = new ExpressionList(items_.size());
        for (auto &item : items_) {
            items->add(item->clone());
        }
        return ListExpression::make(pool_, items);
    }

private:
    void writeTo(Encoder &encoder) const override;

    void resetFrom(Decoder &decoder) override;

private:
    std::vector<Expression*>    items_;
    Value                       result_;
};


class SetExpression final : public Expression {
public:
    static SetExpression *make(ObjectPool *pool = nullptr, ExpressionList *items = nullptr) {
        DCHECK(!!pool);
        return items == nullptr ? pool->add(new SetExpression(pool))
                                : pool->add(new SetExpression(pool, items));
    }

    explicit SetExpression(ObjectPool *pool = nullptr) : Expression(pool, Kind::kSet) {}

    explicit SetExpression(ObjectPool *pool, ExpressionList *items)
        : Expression(pool, Kind::kSet) {
        items_ = items->get();
    }

    const Value& eval(ExpressionContext &ctx) override;

    const std::vector<Expression*> &items() const {
        return items_;
    }

    void setItem(size_t index, Expression* item) {
        DCHECK_LT(index, items_.size());
        items_[index] = item;
    }

    std::vector<Expression*> get() {
        return items_;
    }

    void setItems(std::vector<Expression*> items) {
        items_ = items;
    }

    size_t size() const {
        return items_.size();
    }

    bool operator==(const Expression &rhs) const override;

    std::string toString() const override;

    void accept(ExprVisitor* visitor) override;

    Expression* clone() const override {
        auto items = new ExpressionList(items_.size());
        for (auto &item : items_) {
            items->add(item->clone());
        }
        return SetExpression::make(pool_, items);
    }

private:
    void writeTo(Encoder &encoder) const override;

    void resetFrom(Decoder &decoder) override;

private:
    std::vector<Expression*>    items_;
    Value                       result_;
};

class MapExpression final : public Expression {
public:
    static MapExpression *make(ObjectPool *pool = nullptr, MapItemList *items = nullptr) {
        DCHECK(!!pool);
        return items == nullptr ? pool->add(new MapExpression(pool))
                                : pool->add(new MapExpression(pool, items));
    }

    using Item = std::pair<std::string, Expression *>;

    explicit MapExpression(ObjectPool *pool) : Expression(pool, Kind::kMap) {}

    explicit MapExpression(ObjectPool *pool, MapItemList *items)
        : Expression(pool, Kind::kMap) {
        items_ = items->get();
    }

    const Value& eval(ExpressionContext &ctx) override;

    const std::vector<Item> &items() const {
        return items_;
    }

    void setItems(std::vector<Item> items) {
        items_ = items;
    }

    void setItem(size_t index, Item item) {
        DCHECK_LT(index, items_.size());
        items_[index] = item;
    }

    std::vector<Item> get() {
        return items_;
    }

    size_t size() const {
        return items_.size();
    }

    bool operator==(const Expression &rhs) const override;

    std::string toString() const override;

    void accept(ExprVisitor* visitor) override;

    Expression* clone() const override {
        auto items = new MapItemList(items_.size());
        for (auto &item : items_) {
            items->add(item.first, item.second->clone());
        }
        return MapExpression::make(pool_, items);
    }

private:
    void writeTo(Encoder &encoder) const override;

    void resetFrom(Decoder &decoder) override;

private:
    std::vector<Item>                       items_;
    Value                                   result_;
};

}   // namespace nebula

#endif  // COMMON_EXPRESSION_CONTAINEREXPRESSION_H_
