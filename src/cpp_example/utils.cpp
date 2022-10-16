#include "utils.hpp"
#include <iostream>

#include <mapbox/geojson_impl.hpp>
#include <mapbox/geojson_value_impl.hpp>
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

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

std::string dumps(const RapidjsonValue &json, bool indent)
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

} // namespace utils
} // namespace cubao
