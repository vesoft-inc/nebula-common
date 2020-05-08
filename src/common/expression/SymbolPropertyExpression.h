/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_SYMBOLPROPERTYEXPRESSION_H_
#define COMMON_EXPRESSION_SYMBOLPROPERTYEXPRESSION_H_

#include "expression/Expression.h"

namespace nebula {

constexpr char const kInputRef[] = "$-";
constexpr char const kVarRef[] = "$";
constexpr char const kSrcRef[] = "$^";
constexpr char const kDstRef[] = "$$";

// Base abstract expression of getting properties.
// An expresion of getting props is consisted with 3 parts:
// 1. reference, e.g. $-, $, $^, $$
// 2. symbol, a symbol name, e.g. tag_name, edge_name, variable_name,
// 3. property, property name.
//
// The SymbolPropertyExpression will only be used in parser to indicate
// the form of symbol.prop, it will be transform to a proper expression
// in a parse rule.
class SymbolPropertyExpression: public Expression {
public:
    SymbolPropertyExpression(Kind kind = Kind::kAliasProperty,
                             std::string* ref = nullptr,
                             std::string* sym = nullptr,
                             std::string* prop = nullptr)
        : Expression(type) {
        ref_.reset(ref);
        sym_.reset(sym);
        prop_.reset(prop);
    }

    void setExpCtxt(ExpressionContext* ctxt) override {
        expCtxt_ = ctxt;
    }

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

protected:
    const Value& eval() override {
        LOG(FATAL) << "Not supported for calling SymbolPropertyExpression::eval().";
    }

protected:
    std::unique_ptr<std::string>    ref_;
    std::unique_ptr<std::string>    sym_;
    std::unique_ptr<std::string>    prop_;
};

// edge_name.any_prop_name
class EdgePropertyExpression final : public SymbolPropertyExpression {
public:
    EdgePropertyExpression(std::string* edge, std::string* prop)
        : SymbolPropertyExpression(Type::EXP_EDGE_PROPERTY,
                                   new std::string(""),
                                   edge,
                                   prop) {}

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

protected:
    const Value& eval() override;
};

// $-.any_prop_name
class InputPropertyExpression final : public SymbolPropertyExpression {
public:
    explicit InputPropertyExpression(std::string* prop)
        : SymbolPropertyExpression(Kind::kInputProperty,
                                   new std::string(kInputRef),
                                   new std::string(""),
                                   prop) {}

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

protected:
    const Value& eval() override;
};

// $VarName.any_prop_name
class VariablePropertyExpression final : public SymbolPropertyExpression {
public:
    VariablePropertyExpression(std::string* var,
                               std::string* prop)
        : SymbolPropertyExpression(Kind::kVarProperty,
                                   new std::string(kVarRef),
                                   var,
                                   prop) {}

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

protected:
    const Value& eval() override;
};

// $^.TagName.any_prop_name
class SourcePropertyExpression final : public SymbolPropertyExpression {
public:
    SourcePropertyExpression(std::string* tag,
                             std::string* prop)
        : SymbolPropertyExpression(Kind::kSrcProperty,
                                   new std::string(kSrcRef),
                                   tag,
                                   prop) {}

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

protected:
    const Value& eval() override;
};

// $$.TagName.any_prop_name
class DestPropertyExpression final : public SymbolPropertyExpression {
public:
    DestPropertyExpression(std::string* tag,
                           std::string* prop)
        : SymbolPropertyExpression(Kind::kDstProperty,
                                   new std::string(kDstRef),
                                   tag,
                                   prop) {}

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

protected:
    const Value& eval() override;
};

// EdgeName._src
class EdgeSrcIdExpression final : public SymbolPropertyExpression {
public:
    explicit EdgeSrcIdExpression(std::string* edge)
        : SymbolPropertyExpression(Kind::kEdgeSrc,
                                   new std::string(""),
                                   edge,
                                   new std::string(_SRC)) {}

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

protected:
    const Value& eval() override;
};

// EdgeName._type
class EdgeTypeExpression final : public SymbolPropertyExpression {
public:
    explicit EdgeTypeExpression(std::string* edge)
        : SymbolPropertyExpression(Kind::kEdgeType,
                                   new std::string(""),
                                   edge,
                                   new std::string(_TYPE)) {}

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

protected:
    const Value& eval() override;
};

// EdgeName._rank
class EdgeRankExpression final : public SymbolPropertyExpression {
public:
    explicit EdgeRankExpression(std::string* edge)
        : SymbolPropertyExpression(Kind::kEdgeRank,
                                   new std::string(""),
                                   edge,
                                   new std::string(_RANK)) {}

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

protected:
    const Value& eval() override;
};

// EdgeName._dst
class EdgeDstIdExpression final : public SymbolPropertyExpression {
public:
    explicit EdgeDstIdExpression(std::string* edge)
        : SymbolPropertyExpression(Kind::kEdgeDst,
                                   new std::string(""),
                                   edge,
                                   new std::string(_DST)) {}

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

protected:
    const Value& eval() override;
};
}   // namespace nebula
#endif
