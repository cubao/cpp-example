#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "cpp_example/polyline_ruler.hpp"

#define CHECK_VEC3(v1, v2) CHECK((Eigen::Vector3d)v1 == (Eigen::Vector3d)v2)
#define CHECK_VEC3_NEAR(v1, v2, eps)                                           \
    CHECK(((Eigen::Vector3d)v1 - (Eigen::Vector3d)v2).norm() < eps)

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
        CHECK((std::get<1>(scaneline) - scaneline.first).norm() == 10.0);
        CHECK_VEC3(Eigen::Vector3d(-5, -5, 0), scaneline.first);
        CHECK_VEC3(Eigen::Vector3d(-5, 5, 0), scaneline.second);
    }
    {
        auto scaneline = dbg(ruler.scanline(5));
        CHECK((scaneline.second - scaneline.first).norm() == 10.0);
        CHECK_VEC3(Eigen::Vector3d(5, -5, 0), scaneline.first);
        CHECK_VEC3(Eigen::Vector3d(5, 5, 0), scaneline.second);
    }
    {
        auto scaneline = dbg(ruler.scanline(10 - 1e-3));
        CHECK((scaneline.second - scaneline.first).norm() == 10.0);
        CHECK_VEC3(Eigen::Vector3d(10 - 1e-3, -5, 0), scaneline.first);
        CHECK_VEC3(Eigen::Vector3d(10 - 1e-3, 5, 0), scaneline.second);
    }
    {
        auto scaneline = dbg(ruler.scanline(10 + 1e-3));
        CHECK((scaneline.second - scaneline.first).norm() == 10.0);
        CHECK_VEC3_NEAR(Eigen::Vector3d(15, 1e-3, 0), scaneline.first, 1e-3);
        CHECK_VEC3_NEAR(Eigen::Vector3d(5, 1e-3, 0), scaneline.second, 1e-3);
    }
    {
        double radius = 10.0 * std::sqrt(2.0);
        auto scaneline = dbg(ruler.scanline(10, -radius, radius));
        CHECK(dbg(std::fabs((scaneline.second - scaneline.first).norm() -
                            2 * radius)) < 1e-3);
        CHECK_VEC3_NEAR(Eigen::Vector3d(20, -10, 0), scaneline.first, 1e-3);
        CHECK_VEC3_NEAR(Eigen::Vector3d(0, 10, 0), scaneline.second, 1e-3);
    }
}

TEST_CASE("polyline ruler, plane-xy duplicates")
{
    auto ruler = cubao::PolylineRuler({
        {0, 0, 0},
        {10, 0, 0},
        {10, 0, 10},
        {10, 10, 10},
        {20, 10, 10},
    });
    auto ranges = ruler.ranges();
    dbg(ranges);
    CHECK((ranges.array() -
           (Eigen::VectorXd(5) << 0, 10, 20, 30, 40).finished().array())
              .cwiseAbs2()
              .sum() < 1e-3);

    auto dirs = ruler.dirs();
    std::cout << dirs << std::endl;
    std::cout << dirs.rowwise().norm() << std::endl;
    CHECK(dirs.rows() == 4);
    CHECK_VEC3(Eigen::Vector3d(1, 0, 0), dirs.row(0));
    CHECK_VEC3_NEAR(
        Eigen::Vector3d(0, std::sqrt(2.0) / 2.0, std::sqrt(2.0) / 2.0),
        dirs.row(1), 1e-3);
    CHECK_VEC3(Eigen::Vector3d(0, 1, 0), dirs.row(2));
    CHECK_VEC3(Eigen::Vector3d(1, 0, 0), dirs.row(3));

    CHECK_VEC3(dbg(ruler.extended_along(-5.0)), Eigen::Vector3d(-5, 0, 0));
    CHECK_VEC3(dbg(ruler.extended_along(5.0)), Eigen::Vector3d(5, 0, 0));
    CHECK_VEC3(dbg(ruler.extended_along(10.0)), Eigen::Vector3d(10, 0, 0));
    CHECK_VEC3(dbg(ruler.extended_along(15.0)), Eigen::Vector3d(10, 0, 5));
    CHECK_VEC3(dbg(ruler.extended_along(20.0)), Eigen::Vector3d(10, 0, 10));
    CHECK_VEC3(dbg(ruler.extended_along(25.0)), Eigen::Vector3d(10, 5, 10));
    CHECK_VEC3(dbg(ruler.extended_along(30.0)), Eigen::Vector3d(10, 10, 10));
    CHECK_VEC3(dbg(ruler.extended_along(35.0)), Eigen::Vector3d(15, 10, 10));
    CHECK_VEC3(dbg(ruler.extended_along(40.0)), Eigen::Vector3d(20, 10, 10));
    CHECK_VEC3(dbg(ruler.extended_along(45.0)), Eigen::Vector3d(25, 10, 10));
}

TEST_CASE("polyline ruler, xyz duplicates")
{
    auto ruler = cubao::PolylineRuler({
        {0, 0, 0},
        {10, 0, 0},
        {10, 0, 0},
        {10, 10, 0},
        {20, 10, 0},
    });
    auto ranges = ruler.ranges();
    dbg(ranges);
    CHECK((ranges.array() -
           (Eigen::VectorXd(5) << 0, 10, 10, 20, 30).finished().array())
              .cwiseAbs2()
              .sum() < 1e-3);

    auto dirs = ruler.dirs();
    std::cout << dirs << std::endl;
    CHECK(dirs.rows() == 4);
    CHECK_VEC3(Eigen::Vector3d(1, 0, 0), dirs.row(0));
    CHECK_VEC3(Eigen::Vector3d(0, 1, 0), dirs.row(1));
    CHECK_VEC3(Eigen::Vector3d(0, 1, 0), dirs.row(2));
    CHECK_VEC3(Eigen::Vector3d(1, 0, 0), dirs.row(3));
}

TEST_CASE("polyline ruler, local frame")
{
    auto ruler = cubao::PolylineRuler({
        {0, 0, 0},
        {10, 0, 0},
        {10, 10, 0},
    });
    dbg(ruler.N());
    dbg("0.0");
    std::cout << ruler.local_frame(0.0) << std::endl;
    dbg("5.0");
    std::cout << ruler.local_frame(5.0) << std::endl;
    dbg("10.0");
    std::cout << ruler.local_frame(10.0) << std::endl;
    dbg("15.0");
    std::cout << ruler.local_frame(15.0) << std::endl;

    dbg("10.0 no-smooth");
    std::cout << ruler.local_frame(10.0, false) << std::endl;
    CHECK((ruler.local_frame(10.0, false) - ruler.local_frame(10.0 - 1e-9))
              .cwiseAbs2()
              .sum() < 1e-3);
}

TEST_CASE("polyline ruler, local frame")
{
    auto ruler = cubao::PolylineRuler(
        {
            {120, 45, 0},
            {121, 45, 0},
        },
        true);
    dbg(ruler.ranges());

    dbg("ECEF pose");
    auto T_ecef_local = ruler.local_frame(10.0);
    CHECK(!T_ecef_local.topLeftCorner(3, 3).isIdentity(1e-3));
    std::cout << T_ecef_local << std::endl;

    dbg("ENU pose");
    Eigen::Matrix4d T_enu_local =
        cubao::T_ecef_enu(ruler.polyline().row(0)).inverse() * T_ecef_local;
    std::cout << T_enu_local << std::endl;
    CHECK(T_enu_local.topLeftCorner(3, 3).isIdentity(1e-3));
    CHECK(
        (T_enu_local.topRightCorner(3, 1) - Eigen::Vector3d(10, 0, 0)).norm() <
        1e-3);
}
