#define DBG_MACRO_NO_WARNING
#include <dbg.h>

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
