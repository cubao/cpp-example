#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <iostream>
#include <cmath>

using namespace rapidjson;
using namespace std;

template <typename WriterType> struct CoordsHandler
{
    WriterType &writer;
    CoordsHandler(WriterType &writer) : writer(writer) {}

    bool Null()
    {
        writer.Null();
        return true;
    }
    bool Bool(bool b)
    {
        writer.Bool(b);
        return true;
    }
    bool Int(int i)
    {
        writer.Int(i);
        return true;
    }
    bool Uint(unsigned u)
    {
        writer.Uint(u);
        return true;
    }
    bool Int64(int64_t i)
    {
        writer.Int64(i);
        return true;
    }
    bool Uint64(uint64_t u)
    {
        writer.Uint64(u);
        return true;
    }
    bool String(const char *str, SizeType length, bool copy)
    {
        writer.String(str, length, copy);
        return true;
    }
    bool StartObject()
    {
        writer.StartObject();
        return true;
    }
    bool Key(const char *str, SizeType length, bool copy)
    {
        writer.Key(str, length, copy);
        return true;
    }
    bool EndObject(SizeType memberCount)
    {
        writer.EndObject(memberCount);
        return true;
    }

    bool StartArray()
    {
        writer.StartArray();
        return true;
    }
    bool EndArray(SizeType elementCount)
    {
        writer.EndArray(elementCount);
        return true;
    }
};

bool transform_json(const std::string &input_path,
                    const std::string &output_path, bool indent = true)
{
    using namespace rapidjson;

    FILE *ifp = fopen(input_path.c_str(), "rb");
    if (!ifp) {
        fprintf(stderr, "failed to open %s for reading", input_path.c_str());
        return {};
    }
    FILE *ofp = fopen(output_path.c_str(), "wb");
    if (!ofp) {
        fprintf(stderr, "failed to open %s for writing", output_path.c_str());
        fclose(ifp);
        return false;
    }

    Reader reader;
    char readBuffer[65536];
    FileReadStream is(ifp, readBuffer, sizeof(readBuffer));

    char writeBuffer[65536];
    FileWriteStream os(ofp, writeBuffer, sizeof(writeBuffer));
    if (indent) {
        PrettyWriter<FileWriteStream> writer(os);
        CoordsHandler<PrettyWriter<FileWriteStream>> handler(writer);
        if (reader.Parse(is, handler)) {
            fclose(ifp);
            fclose(ofp);
            return true;
        }
    } else {
        Writer<FileWriteStream> writer(os);
        CoordsHandler<Writer<FileWriteStream>> handler(writer);
        if (reader.Parse(is, handler)) {
            fclose(ifp);
            fclose(ofp);
            return true;
        }
    }
    fclose(ifp);
    fclose(ofp);

    fprintf(stderr, "\nError(%u): %s\n", (unsigned)reader.GetErrorOffset(),
            GetParseError_En(reader.GetParseErrorCode()));
    return false;
}

TEST_CASE("dummy") { CHECK(dbg(true)); }
