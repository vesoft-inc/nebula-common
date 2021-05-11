/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_DATATYPES_VERTEX_H_
#define COMMON_DATATYPES_VERTEX_H_

#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>

#include "common/thrift/ThriftTypes.h"
#include "common/datatypes/Value.h"

namespace nebula {

struct Tag {
    std::string name;
    std::unordered_map<std::string, Value> props;

    Tag() = default;
    Tag(Tag&& tag) noexcept
        : name(std::move(tag.name))
        , props(std::move(tag.props)) {}
    Tag(const Tag& tag)
        : name(tag.name)
        , props(tag.props) {}
    Tag(std::string tagName, std::unordered_map<std::string, Value> tagProps)
        : name(std::move(tagName))
        , props(std::move(tagProps)) {}

    void clear() {
        name.clear();
        props.clear();
    }

    void __clear() {
        clear();
    }

    std::string toString() const;

    Tag& operator=(Tag&& rhs) noexcept {
        if (&rhs != this) {
            name = std::move(rhs.name);
            props = std::move(rhs.props);
        }
        return *this;
    }

    Tag& operator=(const Tag& rhs) {
        if (&rhs != this) {
            name = rhs.name;
            props = rhs.props;
        }
        return *this;
    }

    bool operator==(const Tag& rhs) const {
        return name == rhs.name && props == rhs.props;
    }
};


struct Vertex {
    Value vid;
    std::vector<Tag> tags;

    Vertex() = default;
    Vertex(const Vertex& v) : vid(v.vid), tags(v.tags) {}
    Vertex(Vertex&& v) noexcept
        : vid(std::move(v.vid))
        , tags(std::move(v.tags)) {}
    Vertex(Value id, std::vector<Tag> t)
        : vid(std::move(id))
        , tags(std::move(t)) {}

    void clear() {
        vid.clear();
        tags.clear();
    }

    void __clear() {
        clear();
    }

    std::vector<std::string> tagNames() const {
        std::vector<std::string> t;
        t.reserve(tags.size());
        for (const auto &tag : tags) {
            t.emplace_back(tag.name);
        }
        return t;
    }

    bool hasTag(const std::string &tagName) const {
        const auto find = std::find_if(tags.begin(), tags.end(), [&tagName](const auto &tag) {
            return tag.name == tagName;
        });
        return find != tags.end();
    }

    const std::unordered_map<std::string, Value>& properties(const std::string &tagName) const {
        static const std::unordered_map<std::string, Value> kEmpty;
        const auto find = std::find_if(tags.begin(), tags.end(), [&tagName](const auto &tag) {
            return tag.name == tagName;
        });
        if (find == tags.end()) {
            return kEmpty;
        }
        return find->props;
    }

    std::vector<std::string> keys(const std::string &tagName) const {
        std::vector<std::string> propsName;
        const auto find = std::find_if(tags.begin(), tags.end(), [&tagName](const auto &tag) {
            return tag.name == tagName;
        });
        if (find != tags.end()) {
            propsName.reserve(find->props.size());
            for (const auto &prop : find->props) {
                propsName.emplace_back(prop.first);
            }
        }
        return propsName;
    }

    std::vector<Value> values(const std::string &tagName) const {
        std::vector<Value> values;
        const auto find = std::find_if(tags.begin(), tags.end(), [&tagName](const auto &tag) {
            return tag.name == tagName;
        });
        if (find != tags.end()) {
            values.reserve(find->props.size());
            for (const auto &prop : find->props) {
                values.emplace_back(prop.second);
            }
        }
        return values;
    }

    std::string toString() const;

    Vertex& operator=(Vertex&& rhs) noexcept;

    Vertex& operator=(const Vertex& rhs);

    bool operator==(const Vertex& rhs) const {
        return vid == rhs.vid && tags == rhs.tags;
    }

    bool operator<(const Vertex& rhs) const;

    bool contains(const Value &key) const;
};


inline void swap(Vertex& a, Vertex& b) {
    auto temp = std::move(a);
    a = std::move(b);
    b = std::move(temp);
}

inline std::ostream &operator<<(std::ostream& os, const Vertex& v) {
    return os << v.toString();
}

}  // namespace nebula


namespace std {

// Inject a customized hash function
template<>
struct hash<nebula::Tag> {
    std::size_t operator()(const nebula::Tag& h) const noexcept;
};


template<>
struct hash<nebula::Vertex> {
    std::size_t operator()(const nebula::Vertex& h) const noexcept;
};

}  // namespace std
#endif  // COMMON_DATATYPES_VERTEX_H_
