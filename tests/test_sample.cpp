#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "mbgl/style_expression/value.hpp"

TEST_CASE("dummy") { CHECK(dbg(true)); }

TEST_CASE("style_expression")
{
    //
    CHECK(dbg(true));
}
