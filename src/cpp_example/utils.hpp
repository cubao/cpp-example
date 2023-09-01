#pragma once

#include <mapbox/geojson.hpp>
#include <mapbox/geojson/rapidjson.hpp>
#include <algorithm>
#include "geometry.hpp"
#include "naive_svg.hpp"

namespace cubao
{
// https://github.com/mapbox/geojson-cpp/blob/master/include/mapbox/geojson/rapidjson.hpp
// Use the CrtAllocator, because the MemoryPoolAllocator is broken on ARM
// https://github.com/miloyip/rapidjson/issues/200, 301, 388
using RapidjsonValue = mapbox::geojson::rapidjson_value;
using RapidjsonAllocator = mapbox::geojson::rapidjson_allocator;
using RapidjsonDocument = mapbox::geojson::rapidjson_document;
constexpr const auto RJFLAGS =
    rapidjson::kParseDefaultFlags | rapidjson::kParseCommentsFlag |
    rapidjson::kParseFullPrecisionFlag | rapidjson::kParseNanAndInfFlag |
    rapidjson::kParseTrailingCommasFlag;

namespace utils
{
void hello();

// https://github.com/cubao/geobuf-cpp/blob/dev/src/geobuf/geobuf.hpp
RapidjsonValue loads(const std::string &json_string, bool raise_error = false);
std::string dumps(const RapidjsonValue &json, bool indent = false,
                  bool write_nan_inf = false);
RapidjsonValue load_json(const std::string &path);
bool dump_json(const std::string &path, const RapidjsonValue &json, //
               bool indent = false, bool sort_keys = false);

inline void sort_keys_inplace(RapidjsonValue &json)
{
    if (json.IsArray()) {
        for (auto &e : json.GetArray()) {
            sort_keys_inplace(e);
        }
    } else if (json.IsObject()) {
        auto obj = json.GetObject();
        // https://rapidjson.docsforge.com/master/sortkeys.cpp/
        std::sort(obj.MemberBegin(), obj.MemberEnd(), [](auto &lhs, auto &rhs) {
            return strcmp(lhs.name.GetString(), rhs.name.GetString()) < 0;
        });
        for (auto &kv : obj) {
            sort_keys_inplace(kv.value);
        }
    }
}

inline RapidjsonValue sort_keys(const RapidjsonValue &json)
{
    RapidjsonAllocator allocator;
    RapidjsonValue copy;
    copy.CopyFrom(json, allocator);
    sort_keys_inplace(copy);
    return copy;
}

} // namespace utils
} // namespace cubao
