#pragma once

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 23 &&
                  FLATBUFFERS_VERSION_MINOR == 3 &&
                  FLATBUFFERS_VERSION_REVISION == 3,
              "Non-compatible flatbuffers version included");

#include "header_generated.h"

namespace FlatGeobuf
{

struct Geometry;
struct GeometryBuilder;

struct Feature;
struct FeatureBuilder;

struct Geometry FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table
{
    typedef GeometryBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE
    {
        VT_ENDS = 4,
        VT_XY = 6,
        VT_Z = 8,
        VT_M = 10,
        VT_T = 12,
        VT_TM = 14,
        VT_TYPE = 16,
        VT_PARTS = 18
    };
    const ::flatbuffers::Vector<uint32_t> *ends() const
    {
        return GetPointer<const ::flatbuffers::Vector<uint32_t> *>(VT_ENDS);
    }
    const ::flatbuffers::Vector<double> *xy() const
    {
        return GetPointer<const ::flatbuffers::Vector<double> *>(VT_XY);
    }
    const ::flatbuffers::Vector<double> *z() const
    {
        return GetPointer<const ::flatbuffers::Vector<double> *>(VT_Z);
    }
    const ::flatbuffers::Vector<double> *m() const
    {
        return GetPointer<const ::flatbuffers::Vector<double> *>(VT_M);
    }
    const ::flatbuffers::Vector<double> *t() const
    {
        return GetPointer<const ::flatbuffers::Vector<double> *>(VT_T);
    }
    const ::flatbuffers::Vector<uint64_t> *tm() const
    {
        return GetPointer<const ::flatbuffers::Vector<uint64_t> *>(VT_TM);
    }
    FlatGeobuf::GeometryType type() const
    {
        return static_cast<FlatGeobuf::GeometryType>(
            GetField<uint8_t>(VT_TYPE, 0));
    }
    const ::flatbuffers::Vector<::flatbuffers::Offset<FlatGeobuf::Geometry>> *
    parts() const
    {
        return GetPointer<const ::flatbuffers::Vector<
            ::flatbuffers::Offset<FlatGeobuf::Geometry>> *>(VT_PARTS);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) && VerifyOffset(verifier, VT_ENDS) &&
               verifier.VerifyVector(ends()) && VerifyOffset(verifier, VT_XY) &&
               verifier.VerifyVector(xy()) && VerifyOffset(verifier, VT_Z) &&
               verifier.VerifyVector(z()) && VerifyOffset(verifier, VT_M) &&
               verifier.VerifyVector(m()) && VerifyOffset(verifier, VT_T) &&
               verifier.VerifyVector(t()) && VerifyOffset(verifier, VT_TM) &&
               verifier.VerifyVector(tm()) &&
               VerifyField<uint8_t>(verifier, VT_TYPE, 1) &&
               VerifyOffset(verifier, VT_PARTS) &&
               verifier.VerifyVector(parts()) &&
               verifier.VerifyVectorOfTables(parts()) && verifier.EndTable();
    }
};

struct GeometryBuilder
{
    typedef Geometry Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_ends(::flatbuffers::Offset<::flatbuffers::Vector<uint32_t>> ends)
    {
        fbb_.AddOffset(Geometry::VT_ENDS, ends);
    }
    void add_xy(::flatbuffers::Offset<::flatbuffers::Vector<double>> xy)
    {
        fbb_.AddOffset(Geometry::VT_XY, xy);
    }
    void add_z(::flatbuffers::Offset<::flatbuffers::Vector<double>> z)
    {
        fbb_.AddOffset(Geometry::VT_Z, z);
    }
    void add_m(::flatbuffers::Offset<::flatbuffers::Vector<double>> m)
    {
        fbb_.AddOffset(Geometry::VT_M, m);
    }
    void add_t(::flatbuffers::Offset<::flatbuffers::Vector<double>> t)
    {
        fbb_.AddOffset(Geometry::VT_T, t);
    }
    void add_tm(::flatbuffers::Offset<::flatbuffers::Vector<uint64_t>> tm)
    {
        fbb_.AddOffset(Geometry::VT_TM, tm);
    }
    void add_type(FlatGeobuf::GeometryType type)
    {
        fbb_.AddElement<uint8_t>(Geometry::VT_TYPE, static_cast<uint8_t>(type),
                                 0);
    }
    void add_parts(
        ::flatbuffers::Offset<
            ::flatbuffers::Vector<::flatbuffers::Offset<FlatGeobuf::Geometry>>>
            parts)
    {
        fbb_.AddOffset(Geometry::VT_PARTS, parts);
    }
    explicit GeometryBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<Geometry> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<Geometry>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<Geometry> CreateGeometry(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::Vector<uint32_t>> ends = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<double>> xy = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<double>> z = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<double>> m = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<double>> t = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<uint64_t>> tm = 0,
    FlatGeobuf::GeometryType type = FlatGeobuf::GeometryType::Unknown,
    ::flatbuffers::Offset<
        ::flatbuffers::Vector<::flatbuffers::Offset<FlatGeobuf::Geometry>>>
        parts = 0)
{
    GeometryBuilder builder_(_fbb);
    builder_.add_parts(parts);
    builder_.add_tm(tm);
    builder_.add_t(t);
    builder_.add_m(m);
    builder_.add_z(z);
    builder_.add_xy(xy);
    builder_.add_ends(ends);
    builder_.add_type(type);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<Geometry> CreateGeometryDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<uint32_t> *ends = nullptr,
    const std::vector<double> *xy = nullptr,
    const std::vector<double> *z = nullptr,
    const std::vector<double> *m = nullptr,
    const std::vector<double> *t = nullptr,
    const std::vector<uint64_t> *tm = nullptr,
    FlatGeobuf::GeometryType type = FlatGeobuf::GeometryType::Unknown,
    const std::vector<::flatbuffers::Offset<FlatGeobuf::Geometry>> *parts =
        nullptr)
{
    auto ends__ = ends ? _fbb.CreateVector<uint32_t>(*ends) : 0;
    auto xy__ = xy ? _fbb.CreateVector<double>(*xy) : 0;
    auto z__ = z ? _fbb.CreateVector<double>(*z) : 0;
    auto m__ = m ? _fbb.CreateVector<double>(*m) : 0;
    auto t__ = t ? _fbb.CreateVector<double>(*t) : 0;
    auto tm__ = tm ? _fbb.CreateVector<uint64_t>(*tm) : 0;
    auto parts__ =
        parts ? _fbb.CreateVector<::flatbuffers::Offset<FlatGeobuf::Geometry>>(
                    *parts)
              : 0;
    return FlatGeobuf::CreateGeometry(_fbb, ends__, xy__, z__, m__, t__, tm__,
                                      type, parts__);
}

struct Feature FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table
{
    typedef FeatureBuilder Builder;
    enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE
    {
        VT_GEOMETRY = 4,
        VT_PROPERTIES = 6,
        VT_COLUMNS = 8
    };
    const FlatGeobuf::Geometry *geometry() const
    {
        return GetPointer<const FlatGeobuf::Geometry *>(VT_GEOMETRY);
    }
    const ::flatbuffers::Vector<uint8_t> *properties() const
    {
        return GetPointer<const ::flatbuffers::Vector<uint8_t> *>(
            VT_PROPERTIES);
    }
    const ::flatbuffers::Vector<::flatbuffers::Offset<FlatGeobuf::Column>> *
    columns() const
    {
        return GetPointer<const ::flatbuffers::Vector<
            ::flatbuffers::Offset<FlatGeobuf::Column>> *>(VT_COLUMNS);
    }
    bool Verify(::flatbuffers::Verifier &verifier) const
    {
        return VerifyTableStart(verifier) &&
               VerifyOffset(verifier, VT_GEOMETRY) &&
               verifier.VerifyTable(geometry()) &&
               VerifyOffset(verifier, VT_PROPERTIES) &&
               verifier.VerifyVector(properties()) &&
               VerifyOffset(verifier, VT_COLUMNS) &&
               verifier.VerifyVector(columns()) &&
               verifier.VerifyVectorOfTables(columns()) && verifier.EndTable();
    }
};

struct FeatureBuilder
{
    typedef Feature Table;
    ::flatbuffers::FlatBufferBuilder &fbb_;
    ::flatbuffers::uoffset_t start_;
    void add_geometry(::flatbuffers::Offset<FlatGeobuf::Geometry> geometry)
    {
        fbb_.AddOffset(Feature::VT_GEOMETRY, geometry);
    }
    void add_properties(
        ::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> properties)
    {
        fbb_.AddOffset(Feature::VT_PROPERTIES, properties);
    }
    void add_columns(
        ::flatbuffers::Offset<
            ::flatbuffers::Vector<::flatbuffers::Offset<FlatGeobuf::Column>>>
            columns)
    {
        fbb_.AddOffset(Feature::VT_COLUMNS, columns);
    }
    explicit FeatureBuilder(::flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb)
    {
        start_ = fbb_.StartTable();
    }
    ::flatbuffers::Offset<Feature> Finish()
    {
        const auto end = fbb_.EndTable(start_);
        auto o = ::flatbuffers::Offset<Feature>(end);
        return o;
    }
};

inline ::flatbuffers::Offset<Feature> CreateFeature(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<FlatGeobuf::Geometry> geometry = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> properties = 0,
    ::flatbuffers::Offset<
        ::flatbuffers::Vector<::flatbuffers::Offset<FlatGeobuf::Column>>>
        columns = 0)
{
    FeatureBuilder builder_(_fbb);
    builder_.add_columns(columns);
    builder_.add_properties(properties);
    builder_.add_geometry(geometry);
    return builder_.Finish();
}

inline ::flatbuffers::Offset<Feature> CreateFeatureDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<FlatGeobuf::Geometry> geometry = 0,
    const std::vector<uint8_t> *properties = nullptr,
    const std::vector<::flatbuffers::Offset<FlatGeobuf::Column>> *columns =
        nullptr)
{
    auto properties__ =
        properties ? _fbb.CreateVector<uint8_t>(*properties) : 0;
    auto columns__ =
        columns ? _fbb.CreateVector<::flatbuffers::Offset<FlatGeobuf::Column>>(
                      *columns)
                : 0;
    return FlatGeobuf::CreateFeature(_fbb, geometry, properties__, columns__);
}

inline const FlatGeobuf::Feature *GetFeature(const void *buf)
{
    return ::flatbuffers::GetRoot<FlatGeobuf::Feature>(buf);
}

inline const FlatGeobuf::Feature *GetSizePrefixedFeature(const void *buf)
{
    return ::flatbuffers::GetSizePrefixedRoot<FlatGeobuf::Feature>(buf);
}

inline bool VerifyFeatureBuffer(::flatbuffers::Verifier &verifier)
{
    return verifier.VerifyBuffer<FlatGeobuf::Feature>(nullptr);
}

inline bool VerifySizePrefixedFeatureBuffer(::flatbuffers::Verifier &verifier)
{
    return verifier.VerifySizePrefixedBuffer<FlatGeobuf::Feature>(nullptr);
}

inline void FinishFeatureBuffer(::flatbuffers::FlatBufferBuilder &fbb,
                                ::flatbuffers::Offset<FlatGeobuf::Feature> root)
{
    fbb.Finish(root);
}

inline void
FinishSizePrefixedFeatureBuffer(::flatbuffers::FlatBufferBuilder &fbb,
                                ::flatbuffers::Offset<FlatGeobuf::Feature> root)
{
    fbb.FinishSizePrefixed(root);
}

} // namespace FlatGeobuf
