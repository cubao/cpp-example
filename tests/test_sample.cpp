#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "cpp_example/packedrtree.h"

TEST_CASE("dummy") {
    using namespace FlatGeobuf;
    auto nodes = std::vector<NodeItem>{
        {0, 0, 1, 1},
        {2, 2, 3, 3}
    };
    auto extent = calcExtent(nodes);
    CHECK(nodes[0].intersects({0, 0, 1, 1}));
    CHECK(nodes[1].intersects({2, 2, 3, 3}));

    int index = 0;
    for (auto &node: nodes) {
        node.offset += index;
        ++index;
    }
    hilbertSort(nodes);

    auto rtree = PackedRTree(nodes, extent);
    auto hits = rtree.search(0, 0, 1, 1);
    for (auto &h: hits) {
        dbg(h.index);
        dbg(h.offset);
    }
}
