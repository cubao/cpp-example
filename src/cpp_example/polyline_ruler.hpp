#pragma once

// // https://github.com/microsoft/vscode-cpptools/issues/9692
#if __INTELLISENSE__
#undef __ARM_NEON
#undef __ARM_NEON__
#endif

#include <Eigen/Core>
#include "tl/optional.hpp"

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
};

using RowVectors = Eigen::Matrix<double, Eigen::Dynamic, 3, Eigen::RowMajor>;
using RowVectorsNx3 = RowVectors;
using RowVectorsNx2 = Eigen::Matrix<double, Eigen::Dynamic, 2, Eigen::RowMajor>;

namespace cubao
{
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

namespace utils
{
RowVectors lla2enu_cheap(const Eigen::Ref<const RowVectors> &llas,
                         tl::optional<Eigen::Vector3d> lla = {});
RowVectors enu2lla_cheap(const Eigen::Ref<const RowVectors> &enus,
                         const Eigen::Vector3d &lla);
} // namespace utils
} // namespace cubao
