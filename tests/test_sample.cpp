#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "cpp_example/packedrtree.h"
#include "cpp_example/naive_svg.hpp"
using namespace FlatGeobuf;
using namespace cubao;

TEST_CASE("dummy")
{
    auto nodes = std::vector<NodeItem>{{0, 0, 1, 1}, {2, 2, 3, 3}};
    auto extent = calcExtent(nodes);
    CHECK(nodes[0].intersects({0, 0, 1, 1}));
    CHECK(nodes[1].intersects({2, 2, 3, 3}));

    int index = 0;
    for (auto &node : nodes) {
        node.offset += index;
        ++index;
    }
    hilbertSort(nodes);

    auto rtree = PackedRTree(nodes, extent);
    auto hits = rtree.search(0, 0, 1, 1);
    for (auto &h : hits) {
        dbg(h.index);
        dbg(h.offset);
    }
}

size_t unix_time()
{
    using namespace std;
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
        .count();
}

TEST_CASE("canvas")
{
    const int num_boxes = 800;
    std::random_device rnd_dev;
    std::default_random_engine engine(rnd_dev());

    int width = 1000, height = 1000;
    int padding = 10;

    std::uniform_int_distribution<int> dist_x(padding, width - padding);
    std::uniform_int_distribution<int> dist_y(padding, width - padding);
    std::uniform_int_distribution<int> dist_w(1, padding);
    std::uniform_int_distribution<int> dist_h(1, padding);

    auto nodes = std::vector<NodeItem>();
    nodes.reserve(num_boxes);
    for (int i = 0; i < num_boxes; ++i) {
        double x = (double)dist_x(engine);
        double y = (double)dist_y(engine);
        double w = (double)dist_w(engine);
        double h = (double)dist_h(engine);
        nodes.push_back({x, y, x + w, y + h, (uint64_t)i});
    }
    auto extent = calcExtent(nodes);
    hilbertSort(nodes, extent);
    auto rtree = PackedRTree(nodes, extent);

    double xmin = (double)dist_x(engine);
    double xmax = (double)dist_x(engine);
    double ymin = (double)dist_y(engine);
    double ymax = (double)dist_y(engine);
    if (xmin > xmax) {
        std::swap(xmin, xmax);
    }
    if (ymin > ymax) {
        std::swap(ymin, ymax);
    }
    xmin -= padding;
    xmax += padding;
    ymin -= padding;
    ymax += padding;

    SVG svg(width, height);
    svg.add_polygon({
                        {xmin, ymin},
                        {xmax, ymin},
                        {xmax, ymax},
                        {xmin, ymax},
                        {xmin, ymin},
                    })
        .stroke(SVG::COLOR(0xff0000))
        .stroke_width(3.0);

    std::set<int64_t> hits;
    for (auto h : rtree.search(xmin, ymin, xmax, ymax)) {
        hits.insert(h.offset);
    }
    dbg(hits.size());
    for (auto &node : nodes) {
        auto &box = svg.add_polygon({
            {node.minX, node.minY},
            {node.maxX, node.minY},
            {node.maxX, node.maxY},
            {node.minX, node.maxY},
            {node.minX, node.minY},
        });
        if (!hits.count(node.offset)) {
            box.stroke(SVG::Color(0x00ff00));
        } else {
            box.stroke(SVG::Color(0xff0000)).fill(SVG::Color(0xff0000).a(0.2));
        }
    }
    svg.dump("packedrtree-" + std::to_string(unix_time()) + ".svg");

    // svg.grid_step(10);
}
