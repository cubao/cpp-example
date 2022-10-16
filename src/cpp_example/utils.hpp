#pragma once

#include <mapbox/geojson.hpp>
#include <mapbox/geojson/rapidjson.hpp>

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

RapidjsonValue loads(const std::string &json_string, bool raise_error = false);
std::string dumps(const RapidjsonValue &json, bool indent = false);
} // namespace utils
} // namespace cubao
