#include "cpp_example/naive_svg.hpp"
#include "cpp_example/encodeURIComponent.hpp"

#include <chrono>
#include <iostream>
#include <sstream>

#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using namespace std;
using namespace std::chrono;
using namespace cubao;

size_t unix_time()
{
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
        .count();
}

TEST_CASE("dummy")
{
    SVG svg(500, 400);
    svg.add_circle({100, 100}, 20)
        .stroke(SVG::COLOR::BLACK)
        .fill(SVG::COLOR::GREEN);
    svg.add_text({180, 160}, "some text", 32).stroke(SVG::COLOR::RED);
    svg.add_polyline({
        {110, 2},
        {310, 222},
        {400, 300},
    });
    svg.add_polygon({{61, 82}, {112, 30}, {131, 142}})
        .stroke(SVG::COLOR::RED)
        .stroke_width(0.5)
        .fill(SVG::Color(255, 255, 0, 0.5));
    svg.grid_step(10.0);

    stringstream ss;
    ss << svg << endl;
    cout << svg << endl;

    size_t epoch = unix_time();
    string path = to_string(epoch) + ".svg";
    svg.save(path);
    cout << "wrote to '" << path << "'" << endl;
}
