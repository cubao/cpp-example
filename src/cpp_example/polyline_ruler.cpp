#include "polyline_ruler.hpp"
#include "cheap_ruler.hpp"

namespace cubao
{
double PolylineRuler::squareDistance(const Eigen::Vector3d &a,
                                     const Eigen::Vector3d &b, bool is_wgs84)
{
    if (is_wgs84) {
        return lla2enu(a.transpose(), b).row(0).squaredNorm();
    }
    return (a - b).squaredNorm();
}

double PolylineRuler::lineDistance(const Eigen::Ref<const RowVectors> &line,
                                   bool is_wgs84)
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
double PolylineRuler::lineDistance() const { return lineDistance_; }

tl::optional<Eigen::Vector3d>
PolylineRuler::along(const Eigen::Ref<const RowVectors> &line, double dist,
                     bool is_wgs84)
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

tl::optional<Eigen::Vector3d> PolylineRuler::along(double dist) const
{
    return along(polyline_, dist, is_wgs84_);
}

double PolylineRuler::pointToSegmentDistance(const Eigen::Vector3d &p,
                                             const Eigen::Vector3d &a,
                                             const Eigen::Vector3d &b,
                                             bool is_wgs84)
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

std::tuple<Eigen::Vector3d, int, double>
PolylineRuler::pointOnLine(const Eigen::Ref<const RowVectors> &line,
                           const Eigen::Vector3d &p, bool is_wgs84)
{
    int N = line.rows();
    if (!N) {
        return std::make_tuple(Eigen::Vector3d(0, 0, 0), 0., 0.);
    }
    if (is_wgs84) {
        Eigen::Vector3d anchor = line.row(0);
        auto ret =
            pointOnLine(lla2enu(line, anchor),
                        lla2enu(p.transpose(), anchor).row(0), !is_wgs84);
        std::get<0>(ret) = enu2lla(std::get<0>(ret).transpose(), anchor).row(0);
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
PolylineRuler::pointOnLine(const Eigen::Vector3d &p) const
{
    return pointOnLine(polyline_, p, is_wgs84_);
}

RowVectors PolylineRuler::lineSlice(const Eigen::Vector3d &start,
                                    const Eigen::Vector3d &stop,
                                    const Eigen::Ref<const RowVectors> &line,
                                    bool is_wgs84)
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
RowVectors PolylineRuler::lineSlice(const Eigen::Vector3d &start,
                                    const Eigen::Vector3d &stop) const
{
    return lineSlice(start, stop, polyline_, is_wgs84_);
}

RowVectors
PolylineRuler::lineSliceAlong(double start, double stop,
                              const Eigen::Ref<const RowVectors> &line,
                              bool is_wgs84)
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
RowVectors PolylineRuler::lineSliceAlong(double start, double stop) const
{
    return lineSliceAlong(start, stop, polyline_, is_wgs84_);
}

Eigen::Vector3d PolylineRuler::interpolate(const Eigen::Vector3d &a,
                                           const Eigen::Vector3d &b, double t,
                                           bool is_wgs84)
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
} // namespace cubao
