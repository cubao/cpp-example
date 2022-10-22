#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "cpp_example/cheap_ruler.hpp"

TEST_CASE("cheap ruler")
{
    auto ruler = cubao::cheap_ruler::CheapRuler(
        45.0, cubao::cheap_ruler::CheapRuler::Unit::Meters);
    dbg(ruler.k().norm());
}
