/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "context/QueryContext.h"

namespace nebula {

void QueryContext::setValue(const std::string& name, Value&& val) {
    auto& hist = valueMap_[name];
    hist.emplace_front(std::move(val));
}


void QueryContext::deleteValue(const std::string& name) {
    valueMap_.erase(name);
}


size_t QueryContext::numValueVersions(const std::string& name) const {
    auto it = valueMap_.find(name);
    CHECK(it != valueMap_.end());
    return it->second.size();
}


// Only keep the last several versoins of the Value
void QueryContext::truncValueHist(const std::string& name, size_t numVersionsToKeep) {
    auto it = valueMap_.find(name);
    CHECK(it != valueMap_.end());
    if (it->second.size() <= numVersionsToKeep) {
        return;
    }
    // Truncate the tail, only keep the first N values
    it->second.resize(numVersionsToKeep);
}


// Get the latest version of the value
const Value& QueryContext::getValue(const std::string& name) const {
    auto it = valueMap_.find(name);
    CHECK(it != valueMap_.end());
    return it->second.front();
}


const std::list<Value>& QueryContext::getValueHist(const std::string& name) const {
    auto it = valueMap_.find(name);
    CHECK(it != valueMap_.end());
    return it->second;
}

}  // namespace nebula
