// https://github.com/planet36/ecef-geodetic/blob/main/ecef-to-geodetic-funcs.hpp#L3649-L3735

#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "cpp_example/cheap_ruler.hpp"

// https://github.com/planet36/ecef-geodetic/blob/main/olson_1996/olson_1996.c
// https://github.com/planet36/ecef-geodetic/blob/main/ecef-to-geodetic-funcs.hpp#L3649-L3735
void ecef_to_geodetic(const double x, const double y, const double z,
                      double &lat_rad, double &lon_rad, double &ht)
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

TEST_CASE("CRS") { dbg(1); }
