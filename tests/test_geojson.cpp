#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "cpp_example/utils.hpp"

TEST_CASE("json loads dumps")
{
    auto json_text = R"({
        "string": "string",
        "number": 42,
        "object": {"4": 2,}, // trailing comma, and comment
        "array": [4, 2],
    })";
    auto json = cubao::utils::loads(json_text);
    dbg(cubao::utils::dumps(json));
    dbg(cubao::utils::dumps(json, true));
    CHECK_EQ(
        cubao::utils::dumps(json),
        R"({"string":"string","number":42,"object":{"4":2},"array":[4,2]})");
}

void dbg_feature(const mapbox::geojson::feature &feature)
{
    dbg(cubao::utils::dumps(
        cubao::utils::loads(mapbox::geojson::stringify(feature)), true));
}

TEST_CASE("load_geojson")
{
    auto feature = mapbox::geojson::feature{mapbox::geojson::point(1, 2, 3),
                                            mapbox::feature::property_map{
                                                {"type", "landmark"},
                                                {"id", 123},
                                            }};
    dbg_feature(feature);

    {
        auto feature = mapbox::geojson::feature{mapbox::geojson::point(1, 2, 3),
                                                mapbox::feature::property_map{
                                                    {"type", "landmark"},
                                                    {"id", 123},
                                                },
                                                3.14};
        dbg_feature(feature);
    }
    {
        auto feature = mapbox::geojson::feature{mapbox::geojson::point(1, 2, 3),
                                                mapbox::feature::property_map{
                                                    {"type", "landmark"},
                                                    {"id", 123},
                                                },
                                                3.14,
                                                mapbox::feature::property_map{
                                                    {"extra", "key"},
                                                    {"can be", "added"},
                                                }};
        dbg_feature(feature);
        feature.geometry.custom_properties = {{"geom_key", "geom_value"}};
        dbg_feature(feature);
    }
    {
        auto feature = mapbox::geojson::feature{mapbox::geojson::point(1, 2, 3),
                                                mapbox::feature::property_map{},
                                                mapbox::feature::property_map{
                                                    {"extra", "key"},
                                                    {"can be", "added"},
                                                }};
        dbg_feature(feature);
    }
}
