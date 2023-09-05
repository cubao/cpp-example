#include "matplotlibcpp.h"

#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <vector>

namespace plt = matplotlibcpp;

TEST_CASE("minimal")
{
    std::vector<double> y = {1, 3, 2, 4};
    plt::plot(y);
    plt::save("minimal.pdf");
}

TEST_CASE("standard")
{
    int n = 5000; // 5000 data points
    std::vector<double> x(n), y(n), z(n), w(n, 2);
    for (int i = 0; i < n; ++i) {
        x.at(i) = i * i;
        y.at(i) = sin(2 * M_PI * i / 360.0);
        z.at(i) = log(i);
    }

    plt::figure(); // declare a new figure (optional if only one is used)

    plt::plot(x, y);                        // automatic coloring: tab:blue
    plt::plot(x, w, "r--");                 // red dashed line
    plt::plot(x, z, {{"label", "log(x)"}}); // legend label "log(x)"

    plt::xlim(0, 1000 * 1000);    // x-axis interval: [0, 1e6]
    plt::title("Standard usage"); // set a title
    plt::legend();                // enable the legend

    plt::save("standard.pdf"); // save the figure
}
