/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_EXPRESSION_H_
#define COMMON_EXPRESSION_EXPRESSION_H_

#include <folly/functional/ApplyTuple.h>

#include "common/base/Base.h"
#include "common/datatypes/Value.h"
#include "common/context/ExpressionContext.h"

namespace nebula {

class Expression {
public:
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

    template <typename T, typename = std::enable_if_t<std::is_same<T, Kind>::value>>
    bool isAnyKind(T k) const {
        return kind_ == k;
    }

    template <typename T, typename... Ts, typename = std::enable_if_t<std::is_same<T, Kind>::value>>
    bool isAnyKind(T k, Ts... ts) const {
        return kind_ == k || isAnyKind(ts...);
    }

    template <typename... Ts,
              typename = std::enable_if_t<std::is_same<Kind, std::common_type_t<Ts...>>::value>>
    bool hasAnyKind(Ts... ts) const {
        bool has = false;
        traversal([pack = std::make_tuple(ts...), &has](const Expression *expr) {
            auto bind = [expr](Ts... ts) {
                return expr->isAnyKind(ts...);
            };
            if (folly::apply(bind, pack)) {
                has = true;
                return false;  // Already find so return now
            }
            return true;  // Not find so continue traversal
        });
        return has;
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
