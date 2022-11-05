#pragma once

// // https://github.com/microsoft/vscode-cpptools/issues/9692
#if __INTELLISENSE__
#undef __ARM_NEON
#undef __ARM_NEON__
#endif

#include <Eigen/Core>
#include "tl/optional.hpp"

#include "cheap_ruler.hpp"

namespace cubao
{
using RowVectors = Eigen::Matrix<double, Eigen::Dynamic, 3, Eigen::RowMajor>;
using RowVectorsNx3 = RowVectors;
using RowVectorsNx2 = Eigen::Matrix<double, Eigen::Dynamic, 2, Eigen::RowMajor>;

using CheapRuler = mapbox::cheap_ruler::CheapRuler;
inline RowVectors lla2enu_cheap(const Eigen::Ref<const RowVectors> &llas,
                                tl::optional<Eigen::Vector3d> lla = {})
{
    if (!lla) {
        lla = llas.row(0);
    }
    auto k = CheapRuler::k((*lla)[1], CheapRuler::Unit::Meters);
    RowVectors enus = llas;
    for (int i = 0; i < 3; ++i) {
        enus.col(i).array() -= (*lla)[i];
        enus.col(i).array() *= k[i];
    }
    return enus;
}
inline RowVectors enu2lla_cheap(const Eigen::Ref<const RowVectors> &enus,
                                const Eigen::Vector3d &lla)
{
    auto k = CheapRuler::k(lla[1], CheapRuler::Unit::Meters);
    RowVectors llas = enus;
    for (int i = 0; i < 3; ++i) {
        llas.col(i).array() /= k[i];
        llas.col(i).array() += lla[i];
    }
    return llas;
}

// https://github.com/anvaka/isect/blob/80832e75bf8f197845e52ea52c6ca72935abb24a/build/isect.js#L869
// https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect/1968345#1968345
// https://coliru.stacked-crooked.com/a/624e6e0eabc8a103
// returns [[x, y], t, s]
// Note that: won't handle seg1 == seg2
inline tl::optional<std::tuple<Eigen::Vector2d, double, double>>
intersect_segments(const Eigen::Vector2d &a1, const Eigen::Vector2d &a2,
                   const Eigen::Vector2d &b1, const Eigen::Vector2d &b2)
{
    double p0_x = a1[0], p0_y = a1[1];
    double p2_x = b1[0], p2_y = b1[1];
    double s1_x = a2[0] - a1[0];
    double s1_y = a2[1] - a1[1];
    double s2_x = b2[0] - b1[0];
    double s2_y = b2[1] - b1[1];
    double div = s1_x * s2_y - s2_x * s1_y;
    if (div == 0.0) {
        return {};
    }
    double inv = 1.0 / div;
    double s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) * inv;
    if (s < 0.0 || s > 1.0) {
        return {};
    }
    double t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) * inv;
    if (t < 0.0 || t > 1.0) {
        return {};
    }
    return std::make_tuple(
        Eigen::Vector2d(p0_x + (t * s1_x), p0_y + (t * s1_y)), t, s);
}

inline tl::optional<std::tuple<Eigen::Vector3d, double, double>>
intersect_segments(const Eigen::Vector3d &a1, const Eigen::Vector3d &a2,
                   const Eigen::Vector3d &b1, const Eigen::Vector3d &b2)
{
    Eigen::Vector2d A1 = a1.head(2);
    Eigen::Vector2d A2 = a2.head(2);
    Eigen::Vector2d B1 = b1.head(2);
    Eigen::Vector2d B2 = b2.head(2);
    auto ret = intersect_segments(A1, A2, B1, B2);
    if (!ret) {
        return {};
    }
    double t = std::get<1>(*ret);
    double s = std::get<2>(*ret);
    double ha = a1[2] * (1.0 - t) + a2[2] * t;
    double hb = b1[2] * (1.0 - s) + b2[2] * s;
    const Eigen::Vector2d &p = std::get<0>(*ret);
    return std::make_tuple(Eigen::Vector3d(p[0], p[1], (ha + hb) / 2.0), t, s);
}

// https://github.com/cubao/pybind11-rdp/blob/master/src/main.cpp
struct LineSegment
{
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    const Eigen::Vector3d A, B, AB;
    const double len2, inv_len2;
    LineSegment(const Eigen::Vector3d &a, const Eigen::Vector3d &b)
        : A(a), B(b), AB(b - a), //
          len2((b - a).squaredNorm()), inv_len2(1.0 / len2)
    {
    }
    double distance2(const Eigen::Vector3d &P) const
    {
        double dot = (P - A).dot(AB);
        if (dot <= 0) {
            return (P - A).squaredNorm();
        } else if (dot >= len2) {
            return (P - B).squaredNorm();
        }
        // P' = A + dot/length * normed(AB)
        //    = A + dot * AB / (length^2)
        return (A + (dot * inv_len2 * AB) - P).squaredNorm();
    }
    double distance(const Eigen::Vector3d &P) const
    {
        return std::sqrt(distance2(P));
    }

    tl::optional<std::tuple<Eigen::Vector3d, double, double>>
    intersects(const LineSegment &other)
    {
        return intersect_segments(A, B, other.A, other.B);
    }
};

struct PolylineRuler
{
  private:
    // almost identical APIs to CheapRuler
    const RowVectors polyline_;
    const bool is_wgs84_;
    const double lineDistance_;

  public:
    PolylineRuler(const Eigen::Ref<const RowVectors> &polyline,
                  bool is_wgs84 = false)
        : polyline_(polyline), //
          is_wgs84_(is_wgs84), //
          lineDistance_(lineDistance(polyline, is_wgs84))
    {
    }
    const RowVectors &polyline() const { return polyline_; }
    bool is_wgs84() const { return is_wgs84_; }

    static double squareDistance(const Eigen::Vector3d &a,
                                 const Eigen::Vector3d &b,
                                 bool is_wgs84 = false);
    static double distance(const Eigen::Vector3d &a, const Eigen::Vector3d &b,
                           bool is_wgs84 = false)
    {
        return std::sqrt(squareDistance(a, b, is_wgs84));
    }

    static double lineDistance(const Eigen::Ref<const RowVectors> &line,
                               bool is_wgs84 = false);
    double lineDistance() const;

    static tl::optional<Eigen::Vector3d>
    along(const Eigen::Ref<const RowVectors> &line, double dist,
          bool is_wgs84 = false);
    tl::optional<Eigen::Vector3d> along(double dist) const;

    static double pointToSegmentDistance(const Eigen::Vector3d &p,
                                         const Eigen::Vector3d &a,
                                         const Eigen::Vector3d &b,
                                         bool is_wgs84 = false);

    static std::tuple<Eigen::Vector3d, int, double>
    pointOnLine(const Eigen::Ref<const RowVectors> &line,
                const Eigen::Vector3d &p, bool is_wgs84 = false);
    std::tuple<Eigen::Vector3d, int, double>
    pointOnLine(const Eigen::Vector3d &p) const;

    static RowVectors lineSlice(const Eigen::Vector3d &start,
                                const Eigen::Vector3d &stop,
                                const Eigen::Ref<const RowVectors> &line,
                                bool is_wgs84 = false);
    RowVectors lineSlice(const Eigen::Vector3d &start,
                         const Eigen::Vector3d &stop) const;

    static RowVectors lineSliceAlong(double start, double stop,
                                     const Eigen::Ref<const RowVectors> &line,
                                     bool is_wgs84 = false);
    RowVectors lineSliceAlong(double start, double stop) const;

    static Eigen::Vector3d interpolate(const Eigen::Vector3d &a,
                                       const Eigen::Vector3d &b, double t,
                                       bool is_wgs84 = false);
};

} // namespace cubao
