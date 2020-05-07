/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef EXPRESSION_SYMBOLPROPERTYEXPRESSION_H_
#define EXPRESSION_SYMBOLPROPERTYEXPRESSION_H_

#include "expression/Expression.h"

namespace nebula {

constexpr char const kInputRef[]  = "$-";
constexpr char const kVarRef[]    = "$";
constexpr char const kSrcRef[]    = "$^";
constexpr char const kDstRef[]    = "$$";

// Base abstract expression of getting properties.
// An expresion of getting props is consisted with 3 parts:
// 1. reference, e.g. $-, $, $^, $$
// 2. symbol, a symbol name, e.g. tag_name, edge_name, variable_name,
// 3. property, property name.
class SymbolPropertyExpression: public Expression {
public:
    SymbolPropertyExpression(Type type = Type::EXP_ALIAS_PROPERTY,
                             std::string* ref = nullptr,
                             std::string* sym = nullptr,
                             std::string* prop = nullptr)
        : Expression(type) {
        ref_.reset(ref);
        sym_.reset(sym);
        prop_.reset(prop);
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

protected:
    std::unique_ptr<std::string>    ref_;
    std::unique_ptr<std::string>    sym_;
    std::unique_ptr<std::string>    prop_;
};

// $-.any_prop_name
class InputPropertyExpression final : public SymbolPropertyExpression {
public:
    explicit InputPropertyExpression(std::string* prop)
        : SymbolPropertyExpression(Type::EXP_INPUT_PROPERTY,
                                   new std::string(kInputRef),
                                   new std::string(""),
                                   prop) {}

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
};

// $VarName.any_prop_name
class VariablePropertyExpression final : public SymbolPropertyExpression {
public:
    VariablePropertyExpression(std::string* var,
                               std::string* prop)
        : SymbolPropertyExpression(Type::EXP_VAR_PROPERTY,
                                   new std::string(kVarRef),
                                   var,
                                   prop) {}

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
};

// $^.TagName.any_prop_name
class SourcePropertyExpression final : public SymbolPropertyExpression {
public:
    SourcePropertyExpression(std::string* tag,
                             std::string* prop)
        : SymbolPropertyExpression(Type::EXP_SRC_PROPERTY,
                                   new std::string(kSrcRef),
                                   tag,
                                   prop) {}

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
};

// $$.TagName.any_prop_name
class DestPropertyExpression final : public SymbolPropertyExpression {
public:
    DestPropertyExpression(std::string* tag,
                           std::string* prop)
        : SymbolPropertyExpression(Type::EXP_DST_PROPERTY,
                                   new std::string(kDstRef),
                                   tag,
                                   prop) {}

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
};

// EdgeName._src
class EdgeSrcIdExpression final : public SymbolPropertyExpression {
public:
    explicit EdgeSrcIdExpression(std::string* edge)
        : SymbolPropertyExpression(Type::EXP_EDGE_SRC,
                                   new std::string(""),
                                   edge,
                                   new std::string(_SRC)) {}

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
};

// EdgeName._type
class EdgeTypeExpression final : public SymbolPropertyExpression {
public:
    explicit EdgeTypeExpression(std::string* edge)
        : SymbolPropertyExpression(Type::EXP_EDGE_TYPE,
                                   new std::string(""),
                                   edge,
                                   new std::string(_TYPE)) {}

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
};

// EdgeName._rank
class EdgeRankExpression final : public SymbolPropertyExpression {
public:
    explicit EdgeRankExpression(std::string* edge)
        : SymbolPropertyExpression(Type::EXP_EDGE_RANK,
                                   new std::string(""),
                                   edge,
                                   new std::string(_RANK)) {}

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
};

// EdgeName._dst
class EdgeDstIdExpression final : public SymbolPropertyExpression {
public:
    explicit EdgeDstIdExpression(std::string* edge)
        : SymbolPropertyExpression(Type::EXP_EDGE_DST,
                                   new std::string(""),
                                   edge,
                                  new std::string(_DST)) {}

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
};
}  // namespace nebula
#endif
