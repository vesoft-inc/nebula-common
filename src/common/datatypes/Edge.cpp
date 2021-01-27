/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <folly/String.h>
#include <folly/hash/Hash.h>

#include <common/datatypes/Edge.h>

namespace nebula {

std::string Edge::toString() const {
    std::stringstream os;
    os << "(" << src << ")"
       << "-"
       << "[" << name << "(" << type << ")]"
       << "->"
       << "(" << dst << ")"
       << "@" << ranking;
    if (!props.empty()) {
        std::vector<std::string> value(props.size());
        std::transform(
            props.begin(), props.end(), value.begin(), [](const auto& iter) -> std::string {
                std::stringstream out;
                out << iter.first << ":" << iter.second;
                return out.str();
            });
        os << " " << folly::join(",", value);
    }
    return os.str();
}

std::string Edge::edgeKey() const {
    const auto& srcVid = type > 0 ? src.toString() : dst.toString();
    const auto& dstVid = type > 0 ? dst.toString() : src.toString();
    auto edgeKey = folly::stringPrintf("%s%ld%s%ld%d%ld",
                                       srcVid.c_str(),
                                       srcVid.size(),
                                       dstVid.c_str(),
                                       dstVid.size(),
                                       type < 0 ? -type : type,
                                       ranking);
    return edgeKey;
}

}   // namespace nebula

namespace std {

// Inject a customized hash function
std::size_t hash<nebula::Edge>::operator()(const nebula::Edge& h) const noexcept {
    size_t hv = folly::hash::fnv64(h.src.toString());
    hv = folly::hash::fnv64(h.dst.toString(), hv);
    hv = folly::hash::fnv64_buf(reinterpret_cast<const void*>(&h.type), sizeof(h.type), hv);
    return folly::hash::fnv64_buf(reinterpret_cast<const void*>(&h.ranking), sizeof(h.ranking), hv);
}

}   // namespace std
