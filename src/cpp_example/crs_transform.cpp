#include "crs_transform.hpp"

namespace cubao
{
// lla: lon, lat, alt
RowVectors ecef2lla(const Eigen::Ref<const RowVectors> &ecefs)
{
    const int N = ecefs.rows();
    RowVectors ret = ecefs;
    for (int i = 0; i < N; ++i) {
        internal::ecef_to_geodetic(ret(i, 0), ret(i, 1), ret(i, 2), ret(i, 0),
                                   ret(i, 1), ret(i, 2));
    }
    ret.col(0) *= 180.0 / M_PI;
    ret.col(1) *= 180.0 / M_PI;
    return ret;
}

RowVectors lla2ecef(const Eigen::Ref<const RowVectors> &llas)
{
    const int N = llas.rows();
    RowVectors ret = llas;
    ret.col(0) *= M_PI / 180.0;
    ret.col(1) *= M_PI / 180.0;
    for (int i = 0; i < N; ++i) {
        internal::geodetic_to_ecef(ret(i, 0), ret(i, 1), ret(i, 2), ret(i, 0),
                                   ret(i, 1), ret(i, 2));
    }
    return ret;
}
} // namespace cubao
