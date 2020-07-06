/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_EXPRESSION_H_
#define COMMON_EXPRESSION_EXPRESSION_H_

#include <folly/functional/ApplyTuple.h>
#include <gtest/gtest_prod.h>

#include "common/base/Base.h"
#include "common/datatypes/Value.h"
#include "common/context/ExpressionContext.h"

namespace nebula {

class Expression {
public:
    FRIEND_TEST(ExpressionTest, CheckComponent);

    enum class Kind : uint8_t {
        kConstant,

        kAdd,
        kMinus,
        kMultiply,
        kDivision,
        kMod,

        kUnaryPlus,
        kUnaryNegate,
        kUnaryNot,
        kUnaryIncr,
        kUnaryDecr,

        kRelEQ,
        kRelNE,
        kRelLT,
        kRelLE,
        kRelGT,
        kRelGE,
        kRelIn,

        kLogicalAnd,
        kLogicalOr,
        kLogicalXor,

        kTypeCasting,

        kFunctionCall,

        kSymProperty,
        kEdgeProperty,
        kInputProperty,
        kVarProperty,
        kDstProperty,
        kSrcProperty,
        kEdgeSrc,
        kEdgeType,
        kEdgeRank,
        kEdgeDst,

        kUUID,

        kVar,
        kVersionedVar,
    };

    explicit Expression(Kind kind) : kind_(kind) {}

    virtual ~Expression() = default;

    Kind kind() const {
        return kind_;
    }

    static Value eval(Expression* expr, ExpressionContext& ctx) {
        return expr->eval(ctx);
    }

    virtual const Value& eval(ExpressionContext& ctx) = 0;

    virtual bool operator==(const Expression& rhs) const = 0;
    virtual bool operator!=(const Expression& rhs) const {
        return !operator==(rhs);
    }

    virtual std::string toString() const = 0;

    static std::string encode(const Expression& exp);

    std::string encode() const;

    static std::unique_ptr<Expression> decode(folly::StringPiece encoded);

    // return true continue, false return now
    using Visitor = std::function<bool(const Expression*)>;

    // preorder traversal in fact
    // return true continue, false return now
    virtual bool traversal(Visitor visitor) const = 0;

private:
    template <typename T, typename = std::enable_if_t<std::is_same<T, Kind>::value>>
    bool isAnyKind(T k) const {
        return kind_ == k;
    }

    template <typename T, typename... Ts, typename = std::enable_if_t<std::is_same<T, Kind>::value>>
    bool isAnyKind(T k, Ts... ts) const {
        return kind_ == k || isAnyKind(ts...);
    }

    // null for not found
    template <typename... Ts,
              typename = std::enable_if_t<std::is_same<Kind, std::common_type_t<Ts...>>::value>>
    const Expression* findAnyKind(Ts... ts) const {
        const Expression *found = nullptr;
        traversal([pack = std::make_tuple(ts...), &found](const Expression *expr) {
            auto bind = [expr](Ts... ts_) {
                return expr->isAnyKind(ts_...);
            };
            if (folly::apply(bind, pack)) {
                found = expr;
                return false;  // Already find so return now
            }
            return true;  // Not find so continue traversal
        });
        return found;
    }

    // Find all expression fit any kind
    // Empty for not found any one
    template <typename... Ts,
              typename = std::enable_if_t<std::is_same<Kind, std::common_type_t<Ts...>>::value>>
    std::vector<const Expression*> findAnyKindInAll(Ts... ts) const {
        std::vector<const Expression*> exprs;
        traversal([pack = std::make_tuple(ts...), &exprs](const Expression *expr) {
            auto bind = [expr](Ts... ts_) {
                return expr->isAnyKind(ts_...);
            };
            if (folly::apply(bind, pack)) {
                exprs.emplace_back(expr);
            }
            return true;  // Not return always to traversal entire expression tree
        });
        return exprs;
    }

    template <typename... Ts,
              typename = std::enable_if_t<std::is_same<Kind, std::common_type_t<Ts...>>::value>>
    bool hasAnyKind(Ts... ts) const {
        return findAnyKind(ts...) != nullptr;
    }

public:
    // Require data from input/variable
    bool hasInput() const {
        return hasAnyKind(Kind::kInputProperty,
                          Kind::kVarProperty,
                          Kind::kVar,
                          Kind::kVersionedVar);
    }

    // require data from graph storage
    const Expression* findStorage() const {
        return findAnyKind(Kind::kSymProperty,
                           Kind::kEdgeProperty,
                           Kind::kDstProperty,
                           Kind::kSrcProperty,
                           Kind::kEdgeSrc,
                           Kind::kEdgeType,
                           Kind::kEdgeRank,
                           Kind::kEdgeDst);
    }

    const std::vector<const Expression*> findAllStorage() const {
        return findAnyKindInAll(Kind::kSymProperty,
                                Kind::kEdgeProperty,
                                Kind::kDstProperty,
                                Kind::kSrcProperty,
                                Kind::kEdgeSrc,
                                Kind::kEdgeType,
                                Kind::kEdgeRank,
                                Kind::kEdgeDst);
    }

    bool hasStorage() const {
        return findStorage() != nullptr;
    }

    bool isStorage() const {
        return hasAnyKind(Kind::kSymProperty,
                          Kind::kEdgeProperty,
                          Kind::kDstProperty,
                          Kind::kSrcProperty,
                          Kind::kEdgeSrc,
                          Kind::kEdgeType,
                          Kind::kEdgeRank,
                          Kind::kEdgeDst);
    }

    bool isConstExpr() const {
        return !(hasInput() || hasStorage());
    }

protected:
    class Encoder final {
    public:
        explicit Encoder(size_t bufSizeHint = 2048);
        std::string moveStr();

        Encoder& operator<<(Kind kind) noexcept;
        Encoder& operator<<(const std::string* str) noexcept;
        Encoder& operator<<(const Value& val) noexcept;
        Encoder& operator<<(size_t size) noexcept;
        Encoder& operator<<(Value::Type vType) noexcept;
        Encoder& operator<<(const Expression& exp) noexcept;

    private:
        std::string buf_;
    };

    class Decoder final {
    public:
        explicit Decoder(folly::StringPiece encoded);

        bool finished() const;

        Kind readKind() noexcept;
        std::unique_ptr<std::string> readStr() noexcept;
        Value readValue() noexcept;
        size_t readSize() noexcept;
        Value::Type readValueType() noexcept;
        std::unique_ptr<Expression> readExpression() noexcept;

        // Convert the unprocessed part into the hex string
        std::string getHexStr() const;

    private:
        folly::StringPiece encoded_;
        const char* ptr_;
    };

protected:
    static std::unique_ptr<Expression> decode(Decoder& decoder);

    // Serialize the content of the expression to the given encoder
    virtual void writeTo(Encoder& encoder) const = 0;

    // Reset the content of the expression from the given decoder
    virtual void resetFrom(Decoder& decoder) = 0;

    Kind kind_;
};

std::ostream& operator<<(std::ostream& os, Expression::Kind kind);

}   // namespace nebula
#endif   // COMMON_EXPRESSION_EXPRESSION_H_
