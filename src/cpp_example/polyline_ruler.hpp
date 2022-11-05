#pragma once

// // https://github.com/microsoft/vscode-cpptools/issues/9692
#if __INTELLISENSE__
#undef __ARM_NEON
#undef __ARM_NEON__
#endif

#include <Eigen/Core>
#include "tl/optional.hpp"

#include "cheap_ruler.hpp"
#include "crs_transform.hpp"

namespace cubao
{
using RowVectors = Eigen::Matrix<double, Eigen::Dynamic, 3, Eigen::RowMajor>;
using RowVectorsNx3 = RowVectors;
using RowVectorsNx2 = Eigen::Matrix<double, Eigen::Dynamic, 2, Eigen::RowMajor>;

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
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    PolylineRuler(const Eigen::Ref<const RowVectors> &polyline,
                  bool is_wgs84 = false)
        : polyline_(polyline),                        //
          N_(polyline.rows()),                        //
          is_wgs84_(is_wgs84),                        //
          k_(is_wgs84 ? CheapRuler::k(polyline(0, 1)) //
                      : Eigen::Vector3d::Ones())
    {
    }

  private:
    const RowVectors polyline_;
    const int N_;
    const bool is_wgs84_;
    const Eigen::Vector3d k_;
    // cache
    mutable tl::optional<Eigen::VectorXd> ranges_;
    mutable tl::optional<RowVectors> dirs_;

  public:
    const RowVectors &polyline() const { return polyline_; }
    int N() const { return N_; }
    bool is_wgs84() const { return is_wgs84_; }
    const Eigen::VectorXd &ranges() const { return *ranges_; }
    const RowVectors &dirs() const { return *dirs_; }

    // almost identical APIs to CheapRuler
    static double squareDistance(const Eigen::Vector3d &a,
                                 const Eigen::Vector3d &b,
                                 bool is_wgs84 = false)
    {
        if (is_wgs84) {
            return lla2enu(a.transpose(), b).row(0).squaredNorm();
        }
        return (a - b).squaredNorm();
    }
    static double distance(const Eigen::Vector3d &a, const Eigen::Vector3d &b,
                           bool is_wgs84 = false)
    {
        return std::sqrt(squareDistance(a, b, is_wgs84));
    }

    static double lineDistance(const Eigen::Ref<const RowVectors> &line,
                               bool is_wgs84 = false)
    {
        if (is_wgs84) {
            return lineDistance(lla2enu(line), !is_wgs84);
        }
        int N = line.rows();
        if (N < 2) {
            return 0.0;
        }
        return (line.bottomRows(N - 1) - line.topRows(N - 1))
            .rowwise()
            .norm()
            .sum();
    }
    double lineDistance() const { return ranges()[N_]; }

    static tl::optional<Eigen::Vector3d>
    along(const Eigen::Ref<const RowVectors> &line, double dist,
          bool is_wgs84 = false)
    {
        int N = line.rows();
        if (!N) {
            return {};
        }
        if (dist <= 0.) {
            return line.row(0);
        }
        if (is_wgs84) {
            auto ret = along(lla2enu(line), dist, !is_wgs84);
            if (ret) {
                ret = enu2lla(ret->transpose(), line.row(0)).row(0);
            }
            return ret;
        }

        double sum = 0.;
        for (int i = 0; i < N - 1; ++i) {
            double d = (line.row(i) - line.row(i + 1)).norm();
            sum += d;
            if (sum > dist) {
                return interpolate(line.row(i), line.row(i + 1),
                                   (dist - (sum - d)) / d);
            }
        }
        return line.row(N - 1);
    }
    tl::optional<Eigen::Vector3d> along(double dist) const
    {
        return along(polyline_, dist, is_wgs84_);
    }

    static double pointToSegmentDistance(const Eigen::Vector3d &p,
                                         const Eigen::Vector3d &a,
                                         const Eigen::Vector3d &b,
                                         bool is_wgs84 = false)
    {

        if (is_wgs84) {
            RowVectors llas(3, 3);
            llas.row(0) = p;
            llas.row(1) = a;
            llas.row(2) = b;
            auto enus = lla2enu(llas);
            return pointToSegmentDistance(enus.row(0), //
                                          enus.row(1), //
                                          enus.row(2), //
                                          !is_wgs84);
        }
        return LineSegment(a, b).distance(p);
    }

    static std::tuple<Eigen::Vector3d, int, double>
    pointOnLine(const Eigen::Ref<const RowVectors> &line,
                const Eigen::Vector3d &p, bool is_wgs84 = false)
    {
        int N = line.rows();
        if (!N) {
            return std::make_tuple(p, -1, 0.);
        }
        if (is_wgs84) {
            Eigen::Vector3d anchor = line.row(0);
            auto ret =
                pointOnLine(lla2enu(line, anchor),
                            lla2enu(p.transpose(), anchor).row(0), !is_wgs84);
            std::get<0>(ret) =
                enu2lla(std::get<0>(ret).transpose(), anchor).row(0);
            return ret;
        }
        double minDist = std::numeric_limits<double>::infinity();
        Eigen::Vector3d minP(0.0, 0.0, 0.0);
        double minI = 0., minT = 0.;
        for (int i = 0; i < N - 1; ++i) {
            double t = 0.;
            Eigen::Vector3d ab = line.row(i + 1) - line.row(i);
            Eigen::Vector3d pp = line.row(i);
            if (ab[0] != 0. || ab[1] != 0. || ab[2] != 0.) {
                Eigen::Vector3d ap = p - line.row(i).transpose();
                t = ab.dot(ap) / ab.squaredNorm();
                // t' = unit(ab).dot(ap) = t / |ab|
                // t' > |ap|        -> pp   = line.row(i+1)
                // t' > 0.0         -> pp  += t' * unit(ab)
                //                     pp  += t  * ab
                if (t > 1.0) {
                    pp = line.row(i + 1);
                } else if (t > 0) {
                    pp += t * ab;
                }
            }
            double sqDist = (pp - p).squaredNorm();
            if (sqDist < minDist) {
                minDist = sqDist;
                minP = pp;
                minI = i;
                minT = t;
            }
        }
        return std::make_tuple(minP, minI, std::fmax(0., std::fmin(1., minT)));
    }
    std::tuple<Eigen::Vector3d, int, double>
    pointOnLine(const Eigen::Vector3d &p) const
    {
        return pointOnLine(polyline_, p, is_wgs84_);
    }

    static RowVectors lineSlice(const Eigen::Vector3d &start,
                                const Eigen::Vector3d &stop,
                                const Eigen::Ref<const RowVectors> &line,
                                bool is_wgs84 = false)
    {

        return RowVectors(0, 3);
        /*
        if (is_wgs84) {
            Eigen::Vector3d anchor = line.row(0);
            RowVectors start_stop(2, 3);
            start_stop.row(0) = start;
            start_stop.row(1) = stop;
            auto enus = lla2enu(start_stop, anchor);
            return enu2lla(
                lineSlice(enus.row(0), enus.row(1),
                          lla2enu(line, anchor), !is_wgs84),
                anchor);
        }

        auto getPoint = [](auto tuple) { return std::get<0>(tuple); };
        auto getIndex = [](auto tuple) { return std::get<1>(tuple); };
        auto getT = [](auto tuple) { return std::get<2>(tuple); };
        auto p1 = pointOnLine(line, start);
        auto p2 = pointOnLine(line, stop);
        if (getIndex(p1) > getIndex(p2) ||
            (getIndex(p1) == getIndex(p2) && getT(p1) > getT(p2))) {
            std::swap(p1, p2);
        }
        std::vector<Eigen::Vector3d> slice = {getPoint(p1)};
        auto l = getIndex(p1) + 1;
        auto r = getIndex(p2);
        if (line.row(l).transpose() != slice[0] && l <= r) {
            slice.push_back(line.row(l));
        }
        for (int i = l + 1; i <= r; ++i) {
            slice.push_back(line.row(i));
        }
        if (line.row(r).transpose() != getPoint(p2)) {
            slice.push_back(getPoint(p2));
        }
        return as_row_vectors(&slice[0][0], slice.size());
        */
    }
    RowVectors lineSlice(const Eigen::Vector3d &start,
                         const Eigen::Vector3d &stop) const
    {
        return lineSlice(start, stop, polyline_, is_wgs84_);
    }

    static RowVectors lineSliceAlong(double start, double stop,
                                     const Eigen::Ref<const RowVectors> &line,
                                     bool is_wgs84 = false)
    {

        return RowVectors(0, 3);
        /*
        const int N = line.rows();
        if (N < 2) {
            return RowVectors(0, 3);
        }
        if (is_wgs84) {
            return enu2lla(
                lineSliceAlong(start, stop, lla2enu(line), !is_wgs84),
                line.row(0));
        }
        double sum = 0.;
        std::vector<Eigen::Vector3d> slice;
        for (int i = 1; i < N; ++i) {
            Eigen::Vector3d p0 = line.row(i - 1);
            Eigen::Vector3d p1 = line.row(i);
            double d = (p0 - p1).norm();
            sum += d;
            if (sum > start && slice.size() == 0) {
                if (start < 0) {
                    slice.push_back(p0);
                } else {
                    slice.push_back(
                        interpolate(p0, p1, (start - (sum - d)) / d, is_wgs84));
                }
            }
            if (sum >= stop) {
                slice.push_back(
                    interpolate(p0, p1, (stop - (sum - d)) / d, is_wgs84));
                break;
            }
            if (sum > start) {
                slice.push_back(p1);
            }
        }
        return as_row_vectors(&slice[0][0], slice.size());
        */
    }
    RowVectors lineSliceAlong(double start, double stop) const
    {
        return lineSliceAlong(start, stop, polyline_, is_wgs84_);
    }

    static Eigen::Vector3d interpolate(const Eigen::Vector3d &a,
                                       const Eigen::Vector3d &b, double t,
                                       bool is_wgs84 = false)
    {
        if (is_wgs84) {
            RowVectors llas(2, 3);
            llas.row(0) = a;
            llas.row(1) = b;
            auto enus = lla2enu(llas);
            return enu2lla(interpolate(enus.row(0), enus.row(1), t, !is_wgs84)
                               .transpose(),
                           llas.row(0))
                .row(0);
        }
        return a + (b - a) * t;
    }
};

} // namespace cubao
