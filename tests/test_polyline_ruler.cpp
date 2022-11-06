#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "cpp_example/polyline_ruler.hpp"

#define CHECK_VEC3(v1, v2) CHECK((Eigen::Vector3d)v1 == (Eigen::Vector3d)v2)
// void check_equal(const Eigen::Vector3d &v1, const Eigen::Vector3d &v2) {
//    CHECK(v1 == v2);
// }

TEST_CASE("polyline ruler, basic")
{
    auto ruler = cubao::PolylineRuler({
        {0, 0, 0},
        {5, 0, 0},
        {10, 0, 0},
    });
    CHECK(10.0 == dbg(ruler.length()));

    // ranges
    auto ranges = dbg(ruler.ranges());
    CHECK(ranges.size() == 3);
    CHECK(ranges[0] == 0.0);
    CHECK(ranges[1] == 5.0);
    CHECK(ranges[2] == 10.0);

    // dirs
    auto &dirs = ruler.dirs();
    // dbg(dirs);
    CHECK(dirs.rows() == 2);
    CHECK_VEC3(dirs.row(0), Eigen::Vector3d(1, 0, 0));
    CHECK_VEC3(dirs.row(1), Eigen::Vector3d(1, 0, 0));

    // dir
    CHECK_VEC3(Eigen::Vector3d::UnitX(), dbg(ruler.dir(-1)));
    CHECK_VEC3(Eigen::Vector3d::UnitX(), dbg(ruler.dir(0)));
    CHECK_VEC3(Eigen::Vector3d::UnitX(), dbg(ruler.dir(5)));
    CHECK_VEC3(Eigen::Vector3d::UnitX(), dbg(ruler.dir(15)));
}

TEST_CASE("polyline ruler, dir")
{
    auto ruler = cubao::PolylineRuler({
        {0, 0, 0},
        {10, 0, 0},
        {10, 10, 0},
    });
    std::cout << ruler.polyline() << std::endl;
    CHECK(20.0 == dbg(ruler.length()));

    auto ranges = dbg(ruler.ranges());
    CHECK(ranges.size() == 3);
    CHECK(ranges[0] == 0.0);
    CHECK(ranges[1] == 10.0);
    CHECK(ranges[2] == 20.0);

    // dirs
    auto &dirs = ruler.dirs();
    std::cout << dirs << std::endl;
    CHECK(dirs.rows() == 2);
    CHECK_VEC3(dirs.row(0), Eigen::Vector3d(1, 0, 0));
    CHECK_VEC3(dirs.row(1), Eigen::Vector3d(0, 1, 0));

    // dir
    CHECK_VEC3(Eigen::Vector3d::UnitX(), dbg(ruler.dir(10.0 - 1e-3)));
    CHECK_VEC3(Eigen::Vector3d::UnitX(), dbg(ruler.dir(10.0, false)));
    CHECK_VEC3(Eigen::Vector3d::UnitY(), dbg(ruler.dir(10.0 + 1e-3)));
    Eigen::Vector3d dir = dbg(ruler.dir(10.0));
    CHECK(dir[0] == dir[1]);
}
