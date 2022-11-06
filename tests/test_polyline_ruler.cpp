#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "cpp_example/polyline_ruler.hpp"

#define CHECK_VEC3(v1, v2) CHECK((Eigen::Vector3d)v1 == (Eigen::Vector3d)v2)
#define CHECK_VEC3_NEAR(v1, v2, eps) CHECK((v1 - v2).norm() < eps)

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

TEST_CASE("polyline ruler, scanline")
{
    auto ruler = cubao::PolylineRuler({
        {0, 0, 0},
        {10, 0, 0},
        {10, 10, 0},
        {20, 10, 0},
    });
    {
        auto scaneline = dbg(ruler.scanline(-5));
        CHECK((std::get<1>(scaneline) - std::get<0>(scaneline)).norm() == 10.0);
        CHECK_VEC3(Eigen::Vector3d(-5, -5, 0), std::get<0>(scaneline));
        CHECK_VEC3(Eigen::Vector3d(-5, 5, 0), std::get<1>(scaneline));
    }
    {
        auto scaneline = dbg(ruler.scanline(5));
        CHECK((std::get<1>(scaneline) - std::get<0>(scaneline)).norm() == 10.0);
        CHECK_VEC3(Eigen::Vector3d(5, -5, 0), std::get<0>(scaneline));
        CHECK_VEC3(Eigen::Vector3d(5, 5, 0), std::get<1>(scaneline));
    }
    {
        auto scaneline = dbg(ruler.scanline(10 - 1e-3));
        CHECK((std::get<1>(scaneline) - std::get<0>(scaneline)).norm() == 10.0);
        CHECK_VEC3(Eigen::Vector3d(10 - 1e-3, -5, 0), std::get<0>(scaneline));
        CHECK_VEC3(Eigen::Vector3d(10 - 1e-3, 5, 0), std::get<1>(scaneline));
    }
    {
        auto scaneline = dbg(ruler.scanline(10 + 1e-3));
        CHECK((std::get<1>(scaneline) - std::get<0>(scaneline)).norm() == 10.0);
        CHECK_VEC3_NEAR(Eigen::Vector3d(15, 1e-3, 0), std::get<0>(scaneline),
                        1e-3);
        CHECK_VEC3_NEAR(Eigen::Vector3d(5, 1e-3, 0), std::get<1>(scaneline),
                        1e-3);
    }
}
