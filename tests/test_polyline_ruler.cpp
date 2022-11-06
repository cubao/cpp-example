#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "cpp_example/polyline_ruler.hpp"

TEST_CASE("polyline ruler")
{
    auto ruler = mapbox::cheap_ruler::CheapRuler(
        45.0, mapbox::cheap_ruler::CheapRuler::Unit::Meters);
    dbg(ruler.k().norm());
}
