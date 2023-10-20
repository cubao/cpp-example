#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include "rapidjson/error/en.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include "flatbuffers/flatbuffers.h"
#include "header_generated.h"
#include "geojson.h"

using namespace mapbox::geojson;
using namespace flatbuffers;
using namespace FlatGeobuf;

using RapidjsonValue = mapbox::geojson::rapidjson_value;
using RapidjsonAllocator = mapbox::geojson::rapidjson_allocator;
using RapidjsonDocument = mapbox::geojson::rapidjson_document;

constexpr const auto RJFLAGS = rapidjson::kParseDefaultFlags |      //
                               rapidjson::kParseCommentsFlag |      //
                               rapidjson::kParseFullPrecisionFlag | //
                               rapidjson::kParseTrailingCommasFlag;

inline RapidjsonValue load_json(FILE *fp)
{
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    RapidjsonDocument d;
    d.ParseStream<RJFLAGS>(is);
    fclose(fp);

    // https://github.com/Tencent/rapidjson/issues/380
    return RapidjsonValue{std::move(d.Move())};
}
inline RapidjsonValue load_json(const std::string &path)
{
    FILE *fp = fopen(path.c_str(), "rb");
    if (!fp) {
        return {};
    }
    return load_json(fp);
}

inline std::string dumps(const RapidjsonValue &json, bool indent = false)
{
    rapidjson::StringBuffer buffer;
    if (indent) {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        json.Accept(writer);
    } else {
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        json.Accept(writer);
    }
    return buffer.GetString();
}

int main(int argc, char **argv)
{
    FILE *fp = fopen(argv[2], "wb");
    if (!fp) {
        return -1;
    }
    std::cout << "reading " << argv[1] << std::endl;
    auto json = load_json(argv[1]);
    // std::cout << dumps(json) << std::endl;
    auto geojson = mapbox::geojson::convert(json);
    std::vector<uint8_t> flatgeobuf;
    bool createIndex = true;
    serialize(flatgeobuf, geojson.get<mapbox::geojson::feature_collection>(),
              createIndex);
    std::cout << "writing " << argv[2] << std::endl;
    size_t nbytes = fwrite((char *)&flatgeobuf[0], 1, flatgeobuf.size(), fp);
    return flatgeobuf.size() == nbytes ? 0 : -1;
}
