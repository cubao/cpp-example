#ifndef CUBAO_COMMON_HPP
#define CUBAO_COMMON_HPP

#include <Eigen/Core>
#include <mapbox/geojson.hpp>
#include <mapbox/geojson/rapidjson.hpp>
#include <nlohmann/json.hpp>

// should sync
// - https://github.com/cubao/cpp-example/blob/main/src/cpp_example/common.hpp
// - https://github.com/cubao/headers/tree/main/include/cubao/common.hpp

namespace cubao
{
using json = nlohmann::json;
using VariantValue = mapbox::feature::value;
using PropertyMap = mapbox::feature::property_map;

// https://github.com/mapbox/geojson-cpp/blob/master/include/mapbox/geojson/rapidjson.hpp
// Use the CrtAllocator, because the MemoryPoolAllocator is broken on ARM
// https://github.com/miloyip/rapidjson/issues/200, 301, 388
using RapidjsonValue = mapbox::geojson::rapidjson_value;
using RapidjsonAllocator = mapbox::geojson::rapidjson_allocator;
using RapidjsonDocument = mapbox::geojson::rapidjson_document;
using RowVectors = Eigen::Matrix<double, Eigen::Dynamic, 3, Eigen::RowMajor>;
} // namespace cubao

#endif
