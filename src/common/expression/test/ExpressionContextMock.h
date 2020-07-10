/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/datatypes/Value.h"
#include "common/datatypes/List.h"
#include "common/context/ExpressionContext.h"

namespace nebula {

class ExpressionContextMock final : public ExpressionContext {
public:
    const Value& getVar(const std::string& var) const override {
        auto found = vals_.find(var);
        if (found == vals_.end()) {
            return Value::kNullValue;
        } else {
            if (var == "versioned_var") {
                return found->second.getList().values.back();
            }
            return found->second;
        }
    }

    const Value& getVersionedVar(const std::string& var,
                                 int64_t version) const override {
        auto found = vals_.find(var);
        if (found == vals_.end()) {
            return Value::kNullValue;
        } else {
            if (found->second.type() != Value::Type::LIST) {
                return Value::kNullValue;
            }
            auto size = found->second.getList().size();
            if (size <= static_cast<size_t>(std::abs(version))) {
                return Value::kNullValue;
            }
            if (version <= 0) {
                return found->second.getList()[std::abs(version)];
            } else {
                return found->second.getList()[size - version];
            }
        }
    }

    const Value& getVarProp(const std::string& var,
                            const std::string& prop) const override {
        UNUSED(var);
        auto found = vals_.find(prop);
        if (found == vals_.end()) {
            return Value::kNullValue;
        } else {
            return found->second;
        }
    }

    const Value& getEdgeProp(const std::string& edgeType,
                             const std::string& prop) const override {
        UNUSED(edgeType);
        auto found = vals_.find(prop);
        if (found == vals_.end()) {
            return Value::kNullValue;
        } else {
            return found->second;
        }
    }

    const Value& getTagProp(const std::string& tag,
                            const std::string& prop) const override {
        UNUSED(tag);
        auto found = vals_.find(prop);
        if (found == vals_.end()) {
            return Value::kNullValue;
        } else {
            return found->second;
        }
    }

    const Value& getSrcProp(const std::string& tag,
                            const std::string& prop) const override {
        UNUSED(tag);
        auto found = vals_.find(prop);
        if (found == vals_.end()) {
            return Value::kNullValue;
        } else {
            return found->second;
        }
    }

    const Value& getDstProp(const std::string& tag,
                            const std::string& prop) const override {
        UNUSED(tag);
        auto found = vals_.find(prop);
        if (found == vals_.end()) {
            return Value::kNullValue;
        } else {
            return found->second;
        }
    }

    const Value& getInputProp(const std::string& prop) const override {
        auto found = vals_.find(prop);
        if (found == vals_.end()) {
            return Value::kNullValue;
        } else {
            return found->second;
        }
    }

    void setVar(const std::string& var, Value val) override {
        UNUSED(var);
        UNUSED(val);
    }

private:
    static std::unordered_map<std::string, Value>      vals_;
};
}  // namespace nebula
