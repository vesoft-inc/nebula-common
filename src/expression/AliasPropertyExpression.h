/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef EXPRESSION_ALIASPROPERTYEXPRESSION_H_
#define EXPRESSION_ALIASPROPERTYEXPRESSION_H_

#include "expression/Expression.h"

namespace nebula {

constexpr char INPUT_REF[]  = "$-";
constexpr char VAR_REF[]    = "$";
constexpr char SRC_REF[]    = "$^";
constexpr char DST_REF[]    = "$$";

// Alias.any_prop_name, i.e. EdgeName.any_prop_name
class AliasPropertyExpression: public Expression {
public:
    AliasPropertyExpression(Type type = Type::EXP_ALIAS_PROPERTY,
                            std::unique_ptr<std::string> ref = nullptr,
                            std::unique_ptr<std::string> alias = nullptr,
                            std::unique_ptr<std::string> prop = nullptr)
        : Expression(type),
          ref_(std::move(ref)),
          alias_(std::move(alias)),
          prop_(std::move(prop)) {}

    Value eval() const override;

    std::string encode() const override {
        // TODO
        return "";
    }

protected:
    std::unique_ptr<std::string>    ref_;
    std::unique_ptr<std::string>    alias_;
    std::unique_ptr<std::string>    prop_;
};

// $-.any_prop_name
class InputPropertyExpression final : public AliasPropertyExpression {
public:
    explicit InputPropertyExpression(std::unique_ptr<std::string> prop)
        : AliasPropertyExpression(Type::EXP_INPUT_PROPERTY,
                                  std::make_unique<std::string>(INPUT_REF),
                                  std::make_unique<std::string>(""),
                                  std::move(prop)) {}

    Value eval() const override;

    std::string encode() const override {
        // TODO
        return "";
    }
};

// $VarName.any_prop_name
class VariablePropertyExpression final : public AliasPropertyExpression {
public:
    VariablePropertyExpression(std::unique_ptr<std::string> var,
                               std::unique_ptr<std::string> prop)
        : AliasPropertyExpression(Type::EXP_VAR_PROPERTY,
                                  std::make_unique<std::string>(VAR_REF),
                                  std::move(var),
                                  std::move(prop)) {}

    Value eval() const override;

    std::string encode() const override {
        // TODO
        return "";
    }
};

// $^.TagName.any_prop_name
class SourcePropertyExpression final : public AliasPropertyExpression {
public:
    SourcePropertyExpression(std::unique_ptr<std::string> tag,
                             std::unique_ptr<std::string> prop)
        : AliasPropertyExpression(Type::EXP_SRC_PROPERTY,
                                  std::make_unique<std::string>(SRC_REF),
                                  std::move(tag),
                                  std::move(prop)) {}

    Value eval() const override;

    std::string encode() const override {
        // TODO
        return "";
    }
};

// $$.TagName.any_prop_name
class DestPropertyExpression final : public AliasPropertyExpression {
public:
    DestPropertyExpression(std::unique_ptr<std::string> tag,
                           std::unique_ptr<std::string> prop)
        : AliasPropertyExpression(Type::EXP_DST_PROPERTY,
                                  std::make_unique<std::string>(DST_REF),
                                  std::move(tag),
                                  std::move(prop)) {}

    Value eval() const override;

    std::string encode() const override {
        // TODO
        return "";
    }
};

// Alias._src, i.e. EdgeName._src
class EdgeSrcIdExpression final : public AliasPropertyExpression {
public:
    explicit EdgeSrcIdExpression(std::unique_ptr<std::string> alias)
        : AliasPropertyExpression(Type::EXP_EDGE_SRC,
                                  std::make_unique<std::string>(""),
                                  std::move(alias),
                                  std::make_unique<std::string>(_SRC)) {}

    Value eval() const override;

    std::string encode() const override {
        // TODO
        return "";
    }
};

// Alias._type, i.e. EdgeName._type
class EdgeTypeExpression final : public AliasPropertyExpression {
public:
    explicit EdgeTypeExpression(std::unique_ptr<std::string> alias)
        : AliasPropertyExpression(Type::EXP_EDGE_TYPE,
                                  std::make_unique<std::string>(""),
                                  std::move(alias),
                                  std::make_unique<std::string>(_TYPE)) {}

    Value eval() const override;

    std::string encode() const override {
        // TODO
        return "";
    }
};

// Alias._rank, i.e. EdgeName._rank
class EdgeRankExpression final : public AliasPropertyExpression {
public:
    explicit EdgeRankExpression(std::unique_ptr<std::string> alias)
        : AliasPropertyExpression(Type::EXP_EDGE_RANK,
                                  std::make_unique<std::string>(""),
                                  std::move(alias),
                                  std::make_unique<std::string>(_RANK)) {}

    Value eval() const override;

    std::string encode() const override {
        // TODO
        return "";
    }
};

// Alias._dst, i.e. EdgeName._dst
class EdgeDstIdExpression final : public AliasPropertyExpression {
public:
    explicit EdgeDstIdExpression(std::unique_ptr<std::string> alias)
        : AliasPropertyExpression(Type::EXP_EDGE_DST,
                                  std::make_unique<std::string>(""),
                                  std::move(alias),
                                  std::make_unique<std::string>(_DST)) {}

    Value eval() const override;

    std::string encode() const override {
        // TODO
        return "";
    }
};
}  // namespace nebula
#endif
