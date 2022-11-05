#pragma once

// // https://github.com/microsoft/vscode-cpptools/issues/9692
#if __INTELLISENSE__
#undef __ARM_NEON
#undef __ARM_NEON__
#endif

#include <Eigen/Core>

namespace cubao
{
namespace internal
{
// https://github.com/planet36/ecef-geodetic/blob/main/ecef-to-geodetic-funcs.hpp#L3649-L3735
inline void ecef_to_geodetic(const double x, const double y, const double z,
                             double &lon_rad, double &lat_rad, double &ht)
{
    const auto w2 = x * x + y * y;
    const auto w = std::sqrt(w2);
    const auto z2 = z * z;
    lon_rad = std::atan2(y, x);

    constexpr auto a = 6378137.0;
    constexpr auto e2 = 6.6943799901377997e-3;
    constexpr auto a1 = a * e2;
    constexpr auto a2 = a1 * a1;
    constexpr auto a3 = a1 * e2 / 2;
    constexpr auto a4 = 2.5 * a2;
    constexpr auto a5 = a1 + a3;

    const auto r2 = w2 + z2;
    const auto r = std::sqrt(r2);

    const auto s2 = z2 / r2;
    const auto c2 = w2 / r2;
    auto u = a2 / r;
    auto v = a3 - a4 / r;

    double s;
    double c;
    double ss;

    // cos(45 deg)^2 == 0.5
    if (c2 > 0.5) // Equatorial
    {
        s = (z / r) * (1 + c2 * (a1 + u + s2 * v) / r);
        lat_rad = std::asin(s);
        ss = s * s;
        c = std::sqrt(1 - ss);
    } else // Polar
    {
        c = (w / r) * (1 - s2 * (a5 - u - c2 * v) / r);
        lat_rad = std::acos(c);
        ss = 1 - c * c;
        s = std::sqrt(ss);

        if (z < 0) {
            lat_rad = -lat_rad;
            s = -s;
        }
    }

    const auto d2 = 1 - e2 * ss;
    const auto Rn = a / std::sqrt(d2);
    const auto Rm = (1 - e2) * Rn / d2;
    const auto rf = (1 - e2) * Rn;
    u = w - Rn * c;
    v = z - rf * s;
    const auto f = c * u + s * v;
    const auto m = c * v - s * u;
    const auto p = m / (Rm + f);

    lat_rad += p;

    ht = f + m * p / 2;
}

inline void geodetic_to_ecef(const double lon_rad, const double lat_rad,
                             const double ht, double &x, double &y, double &z)
{
    constexpr double a = 6378137.0;
    constexpr double b = 6356752.314245;
    constexpr double E = (a * a - b * b) / (a * a);
    double coslat = std::cos(lat_rad);
    double sinlat = std::sin(lat_rad);
    double coslong = std::cos(lon_rad);
    double sinlong = std::sin(lon_rad);
    double N = a / (std::sqrt(1 - E * sinlat * sinlat));
    double NH = N + ht;
    x = NH * coslat * coslong;
    y = NH * coslat * sinlong;
    z = (b * b * N / (a * a) + ht) * sinlat;
}
} // namespace internal

// note that:
//  lla: lon, lat, alt, in degrees

inline double to_degrees(double r) { return 180.0 / M_PI * r; }
inline double to_radians(double d) { return M_PI / 180.0 * d; }

inline Eigen::Vector3d ecef2lla(double x, double y, double z)
{
    double lon_rad, lat_rad, ht;
    internal::ecef_to_geodetic(x, y, z, lon_rad, lat_rad, ht);
    return {to_degrees(lon_rad), to_degrees(lat_rad), ht};
}
inline Eigen::Vector3d lla2ecef(double lon, double lat, double alt)
{
    Eigen::Vector3d xyz;
    internal::geodetic_to_ecef(to_radians(lon), to_radians(lat), alt, xyz[0],
                               xyz[1], xyz[2]);
    return xyz;
}

using RowVectors = Eigen::Matrix<double, Eigen::Dynamic, 3, Eigen::RowMajor>;
RowVectors ecef2lla(const Eigen::Ref<const RowVectors> &ecefs);
RowVectors lla2ecef(const Eigen::Ref<const RowVectors> &llas);
} // namespace cubao
