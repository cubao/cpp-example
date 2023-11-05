#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "mbgl/style_expression/enum.hpp"
#include "mbgl/style_expression/expression.hpp"
#include "mbgl/style_expression/feature.hpp"
#include "mbgl/style_expression/optional.hpp"
#include "mbgl/style_expression/parsing_context.hpp"
#include "mbgl/style_expression/type.hpp"
#include "mbgl/style_expression/value.hpp"
#include "mbgl/style_expression/variant.hpp"

TEST_CASE("dummy") { CHECK(dbg(true)); }

TEST_CASE("style_expression")
{
    //
    CHECK(dbg(true));
}
