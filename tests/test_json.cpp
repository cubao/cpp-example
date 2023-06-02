#define DBG_MACRO_NO_WARNING
#include <dbg.h>
#include <limits>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "cpp_example/utils.hpp"
#include "cpp_example/version.h"

TEST_CASE("load_dump_json")
{
    auto j = cubao::utils::load_json(DATA_DIR "/schema");
    {
        std::string path = PROJECT_BINARY_DIR "/schema_origin.json";
        dbg(path);
        CHECK_EQ(true, cubao::utils::dump_json(path, j, true));
    }
    {
        std::string path = PROJECT_BINARY_DIR "/schema_sorted.json";
        dbg(path);
        CHECK_EQ(true, cubao::utils::dump_json(path, j, true, true));
    }
}

TEST_CASE("dump nan")
{
    using namespace cubao;
    double d = 3.14;
    CHECK_EQ(dbg(utils::dumps(RapidjsonValue(d))), "3.14");

    d = std::numeric_limits<double>::quiet_NaN();
    CHECK_EQ(dbg(utils::dumps(RapidjsonValue(d), true, true)), "NaN");

    d = std::numeric_limits<double>::infinity();
    CHECK_EQ(dbg(utils::dumps(RapidjsonValue(d), true, true)), "Infinity");
    d *= -1;
    CHECK_EQ(dbg(utils::dumps(RapidjsonValue(d), true, true)), "-Infinity");
}
