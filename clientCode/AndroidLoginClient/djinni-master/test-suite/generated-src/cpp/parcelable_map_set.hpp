// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from parcelable.djinni

#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace testsuite {

struct ParcelableMapSet final {
    std::unordered_map<std::string, std::unordered_set<std::string>> map_set;

    ParcelableMapSet(std::unordered_map<std::string, std::unordered_set<std::string>> map_set_)
    : map_set(std::move(map_set_))
    {}
};

}  // namespace testsuite
