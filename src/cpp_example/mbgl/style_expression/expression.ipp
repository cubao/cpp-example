#include <mbgl/style/expression/compound_expression.hpp>
#include "../expression.hpp"
#include <utility>

namespace mbgl
{
namespace style
{
namespace expression
{

class GeoJSONFeature : public GeometryTileFeature
{
  public:
    const Feature &feature;
    mutable optional<GeometryCollection> geometry;

    explicit GeoJSONFeature(const Feature &feature_) : feature(feature_) {}
    GeoJSONFeature(const Feature &feature_, const CanonicalTileID &canonical)
        : feature(feature_)
    {
        geometry = convertGeometry(feature.geometry, canonical);
        // https://github.com/mapbox/geojson-vt-cpp/issues/44
        if (getTypeImpl() == FeatureType::Polygon) {
            geometry = fixupPolygons(*geometry);
        }
    }

    FeatureType getType() const override { return getTypeImpl(); }

    const PropertyMap &getProperties() const override
    {
        return feature.properties;
    }
    FeatureIdentifier getID() const override { return feature.id; }
    optional<mbgl::Value> getValue(const std::string &key) const override
    {
        auto it = feature.properties.find(key);
        if (it != feature.properties.end()) {
            return optional<mbgl::Value>(it->second);
        }
        return optional<mbgl::Value>();
    }
    const GeometryCollection &getGeometries() const override
    {
        if (geometry)
            return *geometry;
        geometry = GeometryCollection();
        return *geometry;
    }

  private:
    FeatureType getTypeImpl() const
    {
        return apply_visitor(ToFeatureType(), feature.geometry);
    }
};
} // namespace expression
} // namespace style
} // namespace mbgl
