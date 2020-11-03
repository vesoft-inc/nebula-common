/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/expression/PathBuildExpression.h"

namespace nebula {
const Value& PathBuildExpression::eval(ExpressionContext& ctx) {
    if ((items_.size() & 1) != 1) {
        return Value::kNullValue;
    }
    Path path;
    for (size_t i = 0; i < items_.size(); ++i) {
        auto& val = items_[i]->eval(ctx);
        if ((i & 1) == 1) {
            if (!getEdge(val, path.steps.back())) {
                return Value::kNullBadType;
            }
        } else {
            path.steps.emplace_back();
            if (i == 0 && !getVertex(val, path.src)) {
                return Value::kNullBadType;
            } else if (!getVertex(val, path.steps.back().dst)) {
                return Value::kNullBadType;
            }
        }
    }
    result_ = path;
    return result_;
}

bool PathBuildExpression::getVertex(const Value& value, Vertex& vertex) const {
    if (value.isStr()) {
        vertex = Vertex(value.getStr(), {});
        return true;
    }
    if (value.isVertex()) {
        vertex = Vertex(value.getVertex());
        return true;
    }
    if (value.isInt()) {
        // TODO:
    }
    return false;
}

bool PathBuildExpression::getEdge(const Value& value, Step& step) const {
    if (value.isEdge()) {
        const auto& edge = value.getEdge();
        step.type = edge.type;
        step.name = edge.name;
        step.ranking = edge.ranking;
        step.props = edge.props;
        return true;
    }

    return false;
}

bool PathBuildExpression::operator==(const Expression& rhs) const {
    if (kind() != rhs.kind()) {
        return false;
    }

    auto& pathBuild = static_cast<const PathBuildExpression&>(rhs);
    if (length() != pathBuild.length()) {
        return false;
    }

    for (size_t i = 0; i < size(); ++i) {
        if (*items_[i] != *pathBuild.items_[i]) {
            return false;
        }
    }

    return true;
}

std::string PathBuildExpression::toString() const {
    return "";
}

void PathBuildExpression::accept(ExprVisitor* visitor) {
    UNUSED(visitor);
}

std::unique_ptr<Expression> PathBuildExpression::clone() const {
    return nullptr;
}

void PathBuildExpression::writeTo(Encoder &encoder) const {
    UNUSED(encoder);
}

void PathBuildExpression::resetFrom(Decoder &decoder) {
    UNUSED(decoder);
}
}  // namespace nebula
