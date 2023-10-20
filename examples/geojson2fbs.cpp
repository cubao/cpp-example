#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include "flatbuffers/flatbuffers.h"
#include "header_generated.h"
#include "geojson.h"

using namespace mapbox::geojson;
using namespace flatbuffers;
using namespace FlatGeobuf;

int main(int argc, char **argv)
{
    //   auto geojson = parse(fixtureString).get<feature_collection>();
    feature_collection geojson;
    std::vector<uint8_t> flatgeobuf;
    bool createIndex = true;
    serialize(flatgeobuf, geojson, createIndex);
    std::cout.write((char *)&flatgeobuf[0], flatgeobuf.size());
    return 0;
}
