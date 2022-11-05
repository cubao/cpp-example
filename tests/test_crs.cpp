// https://github.com/planet36/ecef-geodetic/blob/main/ecef-to-geodetic-funcs.hpp#L3649-L3735

#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "cpp_example/crs_transform.hpp"

TEST_CASE("CRS")
{
    using namespace cubao;
    RowVectors llas(5, 3);
    llas << 120.1, 35.2, 2.1, //
        120.2, 35.3, 2.3,     //
        120.3, 35.4, 2.5,     //
        120.4, 35.5, 2.7,     //
        120.5, 35.6, 2.9      //
        ;
    dbg("llas");
    std::cout << llas << std::endl;

    dbg("ecefs");
    auto ecefs = lla2ecef(llas);
    std::cout << ecefs << std::endl;

    dbg("llas (back)");
    auto llas2 = ecef2lla(ecefs);
    std::cout << llas2 << std::endl;

    dbg((llas - llas2).cwiseAbs().mean());
    CHECK(dbg((llas - llas2).cwiseAbs().maxCoeff()) < 1e-7);

    dbg("ecefs (back)");
    auto ecefs2 = lla2ecef(llas2);
    std::cout << ecefs2 << std::endl;
    dbg((ecefs - ecefs2).cwiseAbs().mean());
    CHECK(dbg((ecefs - ecefs2).cwiseAbs().maxCoeff()) < 1e-6);
}
