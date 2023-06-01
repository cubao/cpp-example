#include "utils.hpp"
#include <iostream>

#include <mapbox/geojson_impl.hpp>
#include <mapbox/geojson_value_impl.hpp>
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include "rapidjson/error/en.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"

namespace cubao
{
namespace utils
{
void hello() { std::cout << "hello world" << std::endl; }

RapidjsonValue loads(const std::string &json, bool raise_error)
{
    RapidjsonDocument d;
    rapidjson::StringStream ss(json.c_str());
    d.ParseStream<RJFLAGS>(ss);
    if (d.HasParseError()) {
        if (raise_error) {
            throw std::invalid_argument(
                "invalid json, offset: " + std::to_string(d.GetErrorOffset()) +
                ", error: " + GetParseError_En(d.GetParseError()));
        } else {
            return RapidjsonValue{};
        }
    }
    return RapidjsonValue{std::move(d.Move())};
}

std::string dumps(const RapidjsonValue &json, bool indent, bool write_nan_inf)
{
    rapidjson::StringBuffer buffer;
    if (write_nan_inf) {
        if (indent) {
            rapidjson::PrettyWriter<
                rapidjson::StringBuffer,
                rapidjson::UTF8<>,
                rapidjson::UTF8<>,
                rapidjson::CrtAllocator,
                rapidjson::WriteFlag::kWriteNanAndInfFlag
            > writer(buffer);
            json.Accept(writer);
        } else {
            rapidjson::Writer<
                rapidjson::StringBuffer,
                rapidjson::UTF8<>,
                rapidjson::UTF8<>,
                rapidjson::CrtAllocator,
                rapidjson::WriteFlag::kWriteNanAndInfFlag
            > writer(buffer);
            json.Accept(writer);
        }
    } else {
        if (indent) {
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
            json.Accept(writer);
        } else {
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            json.Accept(writer);
        }
    }
    return buffer.GetString();
}

static RapidjsonValue load_json(FILE *fp)
{
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    RapidjsonDocument d;
    d.ParseStream<RJFLAGS>(is);
    fclose(fp);

    // https://github.com/Tencent/rapidjson/issues/380
    return RapidjsonValue{std::move(d.Move())};
}
RapidjsonValue load_json(const std::string &path)
{
    FILE *fp = fopen(path.c_str(), "rb");
    if (!fp) {
        return {};
    }
    return load_json(fp);
}

bool dump_json(FILE *fp, const RapidjsonValue &json, bool indent,
               bool _sort_keys)
{
    if (_sort_keys) {
        auto sorted = sort_keys(json);
        return dump_json(fp, sorted, indent, false);
    }
    using namespace rapidjson;
    char writeBuffer[65536];
    FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
    if (indent) {
        PrettyWriter<FileWriteStream> writer(os);
        json.Accept(writer);
    } else {
        Writer<FileWriteStream> writer(os);
        json.Accept(writer);
    }
    fclose(fp);
    return true;
}

bool dump_json(const std::string &path, const RapidjsonValue &json, bool indent,
               bool sort_keys)
{
    FILE *fp = fopen(path.c_str(), "wb");
    if (!fp) {
        return false;
    }
    return dump_json(fp, json, indent, sort_keys);
}

} // namespace utils
} // namespace cubao
