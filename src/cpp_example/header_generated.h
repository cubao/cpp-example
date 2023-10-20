#pragma once
#include "flatbuffers/flatbuffers.h"

namespace FlatGeobuf {

struct Column;
struct ColumnBuilder;

struct Crs;
struct CrsBuilder;

struct Header;
struct HeaderBuilder;

enum class GeometryType : uint8_t {
  Unknown = 0,
  Point = 1,
  LineString = 2,
  Polygon = 3,
  MultiPoint = 4,
  MultiLineString = 5,
  MultiPolygon = 6,
  GeometryCollection = 7,
  CircularString = 8,
  CompoundCurve = 9,
  CurvePolygon = 10,
  MultiCurve = 11,
  MultiSurface = 12,
  Curve = 13,
  Surface = 14,
  PolyhedralSurface = 15,
  TIN = 16,
  Triangle = 17,
  MIN = Unknown,
  MAX = Triangle
};

inline const GeometryType (&EnumValuesGeometryType())[18] {
  static const GeometryType values[] = {
    GeometryType::Unknown,
    GeometryType::Point,
    GeometryType::LineString,
    GeometryType::Polygon,
    GeometryType::MultiPoint,
    GeometryType::MultiLineString,
    GeometryType::MultiPolygon,
    GeometryType::GeometryCollection,
    GeometryType::CircularString,
    GeometryType::CompoundCurve,
    GeometryType::CurvePolygon,
    GeometryType::MultiCurve,
    GeometryType::MultiSurface,
    GeometryType::Curve,
    GeometryType::Surface,
    GeometryType::PolyhedralSurface,
    GeometryType::TIN,
    GeometryType::Triangle
  };
  return values;
}

inline const char * const *EnumNamesGeometryType() {
  static const char * const names[19] = {
    "Unknown",
    "Point",
    "LineString",
    "Polygon",
    "MultiPoint",
    "MultiLineString",
    "MultiPolygon",
    "GeometryCollection",
    "CircularString",
    "CompoundCurve",
    "CurvePolygon",
    "MultiCurve",
    "MultiSurface",
    "Curve",
    "Surface",
    "PolyhedralSurface",
    "TIN",
    "Triangle",
    nullptr
  };
  return names;
}

inline const char *EnumNameGeometryType(GeometryType e) {
  if (::flatbuffers::IsOutRange(e, GeometryType::Unknown, GeometryType::Triangle)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesGeometryType()[index];
}

enum class ColumnType : uint8_t {
  Byte = 0,
  UByte = 1,
  Bool = 2,
  Short = 3,
  UShort = 4,
  Int = 5,
  UInt = 6,
  Long = 7,
  ULong = 8,
  Float = 9,
  Double = 10,
  String = 11,
  Json = 12,
  DateTime = 13,
  Binary = 14,
  MIN = Byte,
  MAX = Binary
};

inline const ColumnType (&EnumValuesColumnType())[15] {
  static const ColumnType values[] = {
    ColumnType::Byte,
    ColumnType::UByte,
    ColumnType::Bool,
    ColumnType::Short,
    ColumnType::UShort,
    ColumnType::Int,
    ColumnType::UInt,
    ColumnType::Long,
    ColumnType::ULong,
    ColumnType::Float,
    ColumnType::Double,
    ColumnType::String,
    ColumnType::Json,
    ColumnType::DateTime,
    ColumnType::Binary
  };
  return values;
}

inline const char * const *EnumNamesColumnType() {
  static const char * const names[16] = {
    "Byte",
    "UByte",
    "Bool",
    "Short",
    "UShort",
    "Int",
    "UInt",
    "Long",
    "ULong",
    "Float",
    "Double",
    "String",
    "Json",
    "DateTime",
    "Binary",
    nullptr
  };
  return names;
}

inline const char *EnumNameColumnType(ColumnType e) {
  if (::flatbuffers::IsOutRange(e, ColumnType::Byte, ColumnType::Binary)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesColumnType()[index];
}

struct Column FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef ColumnBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NAME = 4,
    VT_TYPE = 6,
    VT_TITLE = 8,
    VT_DESCRIPTION = 10,
    VT_WIDTH = 12,
    VT_PRECISION = 14,
    VT_SCALE = 16,
    VT_NULLABLE = 18,
    VT_UNIQUE = 20,
    VT_PRIMARY_KEY = 22,
    VT_METADATA = 24
  };
  const ::flatbuffers::String *name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NAME);
  }
  FlatGeobuf::ColumnType type() const {
    return static_cast<FlatGeobuf::ColumnType>(GetField<uint8_t>(VT_TYPE, 0));
  }
  const ::flatbuffers::String *title() const {
    return GetPointer<const ::flatbuffers::String *>(VT_TITLE);
  }
  const ::flatbuffers::String *description() const {
    return GetPointer<const ::flatbuffers::String *>(VT_DESCRIPTION);
  }
  int32_t width() const {
    return GetField<int32_t>(VT_WIDTH, -1);
  }
  int32_t precision() const {
    return GetField<int32_t>(VT_PRECISION, -1);
  }
  int32_t scale() const {
    return GetField<int32_t>(VT_SCALE, -1);
  }
  bool nullable() const {
    return GetField<uint8_t>(VT_NULLABLE, 1) != 0;
  }
  bool unique() const {
    return GetField<uint8_t>(VT_UNIQUE, 0) != 0;
  }
  bool primary_key() const {
    return GetField<uint8_t>(VT_PRIMARY_KEY, 0) != 0;
  }
  const ::flatbuffers::String *metadata() const {
    return GetPointer<const ::flatbuffers::String *>(VT_METADATA);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffsetRequired(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           VerifyField<uint8_t>(verifier, VT_TYPE, 1) &&
           VerifyOffset(verifier, VT_TITLE) &&
           verifier.VerifyString(title()) &&
           VerifyOffset(verifier, VT_DESCRIPTION) &&
           verifier.VerifyString(description()) &&
           VerifyField<int32_t>(verifier, VT_WIDTH, 4) &&
           VerifyField<int32_t>(verifier, VT_PRECISION, 4) &&
           VerifyField<int32_t>(verifier, VT_SCALE, 4) &&
           VerifyField<uint8_t>(verifier, VT_NULLABLE, 1) &&
           VerifyField<uint8_t>(verifier, VT_UNIQUE, 1) &&
           VerifyField<uint8_t>(verifier, VT_PRIMARY_KEY, 1) &&
           VerifyOffset(verifier, VT_METADATA) &&
           verifier.VerifyString(metadata()) &&
           verifier.EndTable();
  }
};

struct ColumnBuilder {
  typedef Column Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_name(::flatbuffers::Offset<::flatbuffers::String> name) {
    fbb_.AddOffset(Column::VT_NAME, name);
  }
  void add_type(FlatGeobuf::ColumnType type) {
    fbb_.AddElement<uint8_t>(Column::VT_TYPE, static_cast<uint8_t>(type), 0);
  }
  void add_title(::flatbuffers::Offset<::flatbuffers::String> title) {
    fbb_.AddOffset(Column::VT_TITLE, title);
  }
  void add_description(::flatbuffers::Offset<::flatbuffers::String> description) {
    fbb_.AddOffset(Column::VT_DESCRIPTION, description);
  }
  void add_width(int32_t width) {
    fbb_.AddElement<int32_t>(Column::VT_WIDTH, width, -1);
  }
  void add_precision(int32_t precision) {
    fbb_.AddElement<int32_t>(Column::VT_PRECISION, precision, -1);
  }
  void add_scale(int32_t scale) {
    fbb_.AddElement<int32_t>(Column::VT_SCALE, scale, -1);
  }
  void add_nullable(bool nullable) {
    fbb_.AddElement<uint8_t>(Column::VT_NULLABLE, static_cast<uint8_t>(nullable), 1);
  }
  void add_unique(bool unique) {
    fbb_.AddElement<uint8_t>(Column::VT_UNIQUE, static_cast<uint8_t>(unique), 0);
  }
  void add_primary_key(bool primary_key) {
    fbb_.AddElement<uint8_t>(Column::VT_PRIMARY_KEY, static_cast<uint8_t>(primary_key), 0);
  }
  void add_metadata(::flatbuffers::Offset<::flatbuffers::String> metadata) {
    fbb_.AddOffset(Column::VT_METADATA, metadata);
  }
  explicit ColumnBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Column> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Column>(end);
    fbb_.Required(o, Column::VT_NAME);
    return o;
  }
};

inline ::flatbuffers::Offset<Column> CreateColumn(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::String> name = 0,
    FlatGeobuf::ColumnType type = FlatGeobuf::ColumnType::Byte,
    ::flatbuffers::Offset<::flatbuffers::String> title = 0,
    ::flatbuffers::Offset<::flatbuffers::String> description = 0,
    int32_t width = -1,
    int32_t precision = -1,
    int32_t scale = -1,
    bool nullable = true,
    bool unique = false,
    bool primary_key = false,
    ::flatbuffers::Offset<::flatbuffers::String> metadata = 0) {
  ColumnBuilder builder_(_fbb);
  builder_.add_metadata(metadata);
  builder_.add_scale(scale);
  builder_.add_precision(precision);
  builder_.add_width(width);
  builder_.add_description(description);
  builder_.add_title(title);
  builder_.add_name(name);
  builder_.add_primary_key(primary_key);
  builder_.add_unique(unique);
  builder_.add_nullable(nullable);
  builder_.add_type(type);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<Column> CreateColumnDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr,
    FlatGeobuf::ColumnType type = FlatGeobuf::ColumnType::Byte,
    const char *title = nullptr,
    const char *description = nullptr,
    int32_t width = -1,
    int32_t precision = -1,
    int32_t scale = -1,
    bool nullable = true,
    bool unique = false,
    bool primary_key = false,
    const char *metadata = nullptr) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  auto title__ = title ? _fbb.CreateString(title) : 0;
  auto description__ = description ? _fbb.CreateString(description) : 0;
  auto metadata__ = metadata ? _fbb.CreateString(metadata) : 0;
  return FlatGeobuf::CreateColumn(
      _fbb,
      name__,
      type,
      title__,
      description__,
      width,
      precision,
      scale,
      nullable,
      unique,
      primary_key,
      metadata__);
}

struct Crs FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef CrsBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ORG = 4,
    VT_CODE = 6,
    VT_NAME = 8,
    VT_DESCRIPTION = 10,
    VT_WKT = 12,
    VT_CODE_STRING = 14
  };
  const ::flatbuffers::String *org() const {
    return GetPointer<const ::flatbuffers::String *>(VT_ORG);
  }
  int32_t code() const {
    return GetField<int32_t>(VT_CODE, 0);
  }
  const ::flatbuffers::String *name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NAME);
  }
  const ::flatbuffers::String *description() const {
    return GetPointer<const ::flatbuffers::String *>(VT_DESCRIPTION);
  }
  const ::flatbuffers::String *wkt() const {
    return GetPointer<const ::flatbuffers::String *>(VT_WKT);
  }
  const ::flatbuffers::String *code_string() const {
    return GetPointer<const ::flatbuffers::String *>(VT_CODE_STRING);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_ORG) &&
           verifier.VerifyString(org()) &&
           VerifyField<int32_t>(verifier, VT_CODE, 4) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           VerifyOffset(verifier, VT_DESCRIPTION) &&
           verifier.VerifyString(description()) &&
           VerifyOffset(verifier, VT_WKT) &&
           verifier.VerifyString(wkt()) &&
           VerifyOffset(verifier, VT_CODE_STRING) &&
           verifier.VerifyString(code_string()) &&
           verifier.EndTable();
  }
};

struct CrsBuilder {
  typedef Crs Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_org(::flatbuffers::Offset<::flatbuffers::String> org) {
    fbb_.AddOffset(Crs::VT_ORG, org);
  }
  void add_code(int32_t code) {
    fbb_.AddElement<int32_t>(Crs::VT_CODE, code, 0);
  }
  void add_name(::flatbuffers::Offset<::flatbuffers::String> name) {
    fbb_.AddOffset(Crs::VT_NAME, name);
  }
  void add_description(::flatbuffers::Offset<::flatbuffers::String> description) {
    fbb_.AddOffset(Crs::VT_DESCRIPTION, description);
  }
  void add_wkt(::flatbuffers::Offset<::flatbuffers::String> wkt) {
    fbb_.AddOffset(Crs::VT_WKT, wkt);
  }
  void add_code_string(::flatbuffers::Offset<::flatbuffers::String> code_string) {
    fbb_.AddOffset(Crs::VT_CODE_STRING, code_string);
  }
  explicit CrsBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Crs> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Crs>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Crs> CreateCrs(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::String> org = 0,
    int32_t code = 0,
    ::flatbuffers::Offset<::flatbuffers::String> name = 0,
    ::flatbuffers::Offset<::flatbuffers::String> description = 0,
    ::flatbuffers::Offset<::flatbuffers::String> wkt = 0,
    ::flatbuffers::Offset<::flatbuffers::String> code_string = 0) {
  CrsBuilder builder_(_fbb);
  builder_.add_code_string(code_string);
  builder_.add_wkt(wkt);
  builder_.add_description(description);
  builder_.add_name(name);
  builder_.add_code(code);
  builder_.add_org(org);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<Crs> CreateCrsDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const char *org = nullptr,
    int32_t code = 0,
    const char *name = nullptr,
    const char *description = nullptr,
    const char *wkt = nullptr,
    const char *code_string = nullptr) {
  auto org__ = org ? _fbb.CreateString(org) : 0;
  auto name__ = name ? _fbb.CreateString(name) : 0;
  auto description__ = description ? _fbb.CreateString(description) : 0;
  auto wkt__ = wkt ? _fbb.CreateString(wkt) : 0;
  auto code_string__ = code_string ? _fbb.CreateString(code_string) : 0;
  return FlatGeobuf::CreateCrs(
      _fbb,
      org__,
      code,
      name__,
      description__,
      wkt__,
      code_string__);
}

struct Header FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef HeaderBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NAME = 4,
    VT_ENVELOPE = 6,
    VT_GEOMETRY_TYPE = 8,
    VT_HAS_Z = 10,
    VT_HAS_M = 12,
    VT_HAS_T = 14,
    VT_HAS_TM = 16,
    VT_COLUMNS = 18,
    VT_FEATURES_COUNT = 20,
    VT_INDEX_NODE_SIZE = 22,
    VT_CRS = 24,
    VT_TITLE = 26,
    VT_DESCRIPTION = 28,
    VT_METADATA = 30
  };
  const ::flatbuffers::String *name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NAME);
  }
  const ::flatbuffers::Vector<double> *envelope() const {
    return GetPointer<const ::flatbuffers::Vector<double> *>(VT_ENVELOPE);
  }
  FlatGeobuf::GeometryType geometry_type() const {
    return static_cast<FlatGeobuf::GeometryType>(GetField<uint8_t>(VT_GEOMETRY_TYPE, 0));
  }
  bool has_z() const {
    return GetField<uint8_t>(VT_HAS_Z, 0) != 0;
  }
  bool has_m() const {
    return GetField<uint8_t>(VT_HAS_M, 0) != 0;
  }
  bool has_t() const {
    return GetField<uint8_t>(VT_HAS_T, 0) != 0;
  }
  bool has_tm() const {
    return GetField<uint8_t>(VT_HAS_TM, 0) != 0;
  }
  const ::flatbuffers::Vector<::flatbuffers::Offset<FlatGeobuf::Column>> *columns() const {
    return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<FlatGeobuf::Column>> *>(VT_COLUMNS);
  }
  uint64_t features_count() const {
    return GetField<uint64_t>(VT_FEATURES_COUNT, 0);
  }
  uint16_t index_node_size() const {
    return GetField<uint16_t>(VT_INDEX_NODE_SIZE, 16);
  }
  const FlatGeobuf::Crs *crs() const {
    return GetPointer<const FlatGeobuf::Crs *>(VT_CRS);
  }
  const ::flatbuffers::String *title() const {
    return GetPointer<const ::flatbuffers::String *>(VT_TITLE);
  }
  const ::flatbuffers::String *description() const {
    return GetPointer<const ::flatbuffers::String *>(VT_DESCRIPTION);
  }
  const ::flatbuffers::String *metadata() const {
    return GetPointer<const ::flatbuffers::String *>(VT_METADATA);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           VerifyOffset(verifier, VT_ENVELOPE) &&
           verifier.VerifyVector(envelope()) &&
           VerifyField<uint8_t>(verifier, VT_GEOMETRY_TYPE, 1) &&
           VerifyField<uint8_t>(verifier, VT_HAS_Z, 1) &&
           VerifyField<uint8_t>(verifier, VT_HAS_M, 1) &&
           VerifyField<uint8_t>(verifier, VT_HAS_T, 1) &&
           VerifyField<uint8_t>(verifier, VT_HAS_TM, 1) &&
           VerifyOffset(verifier, VT_COLUMNS) &&
           verifier.VerifyVector(columns()) &&
           verifier.VerifyVectorOfTables(columns()) &&
           VerifyField<uint64_t>(verifier, VT_FEATURES_COUNT, 8) &&
           VerifyField<uint16_t>(verifier, VT_INDEX_NODE_SIZE, 2) &&
           VerifyOffset(verifier, VT_CRS) &&
           verifier.VerifyTable(crs()) &&
           VerifyOffset(verifier, VT_TITLE) &&
           verifier.VerifyString(title()) &&
           VerifyOffset(verifier, VT_DESCRIPTION) &&
           verifier.VerifyString(description()) &&
           VerifyOffset(verifier, VT_METADATA) &&
           verifier.VerifyString(metadata()) &&
           verifier.EndTable();
  }
};

struct HeaderBuilder {
  typedef Header Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_name(::flatbuffers::Offset<::flatbuffers::String> name) {
    fbb_.AddOffset(Header::VT_NAME, name);
  }
  void add_envelope(::flatbuffers::Offset<::flatbuffers::Vector<double>> envelope) {
    fbb_.AddOffset(Header::VT_ENVELOPE, envelope);
  }
  void add_geometry_type(FlatGeobuf::GeometryType geometry_type) {
    fbb_.AddElement<uint8_t>(Header::VT_GEOMETRY_TYPE, static_cast<uint8_t>(geometry_type), 0);
  }
  void add_has_z(bool has_z) {
    fbb_.AddElement<uint8_t>(Header::VT_HAS_Z, static_cast<uint8_t>(has_z), 0);
  }
  void add_has_m(bool has_m) {
    fbb_.AddElement<uint8_t>(Header::VT_HAS_M, static_cast<uint8_t>(has_m), 0);
  }
  void add_has_t(bool has_t) {
    fbb_.AddElement<uint8_t>(Header::VT_HAS_T, static_cast<uint8_t>(has_t), 0);
  }
  void add_has_tm(bool has_tm) {
    fbb_.AddElement<uint8_t>(Header::VT_HAS_TM, static_cast<uint8_t>(has_tm), 0);
  }
  void add_columns(::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<FlatGeobuf::Column>>> columns) {
    fbb_.AddOffset(Header::VT_COLUMNS, columns);
  }
  void add_features_count(uint64_t features_count) {
    fbb_.AddElement<uint64_t>(Header::VT_FEATURES_COUNT, features_count, 0);
  }
  void add_index_node_size(uint16_t index_node_size) {
    fbb_.AddElement<uint16_t>(Header::VT_INDEX_NODE_SIZE, index_node_size, 16);
  }
  void add_crs(::flatbuffers::Offset<FlatGeobuf::Crs> crs) {
    fbb_.AddOffset(Header::VT_CRS, crs);
  }
  void add_title(::flatbuffers::Offset<::flatbuffers::String> title) {
    fbb_.AddOffset(Header::VT_TITLE, title);
  }
  void add_description(::flatbuffers::Offset<::flatbuffers::String> description) {
    fbb_.AddOffset(Header::VT_DESCRIPTION, description);
  }
  void add_metadata(::flatbuffers::Offset<::flatbuffers::String> metadata) {
    fbb_.AddOffset(Header::VT_METADATA, metadata);
  }
  explicit HeaderBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<Header> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<Header>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<Header> CreateHeader(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::String> name = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<double>> envelope = 0,
    FlatGeobuf::GeometryType geometry_type = FlatGeobuf::GeometryType::Unknown,
    bool has_z = false,
    bool has_m = false,
    bool has_t = false,
    bool has_tm = false,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<FlatGeobuf::Column>>> columns = 0,
    uint64_t features_count = 0,
    uint16_t index_node_size = 16,
    ::flatbuffers::Offset<FlatGeobuf::Crs> crs = 0,
    ::flatbuffers::Offset<::flatbuffers::String> title = 0,
    ::flatbuffers::Offset<::flatbuffers::String> description = 0,
    ::flatbuffers::Offset<::flatbuffers::String> metadata = 0) {
  HeaderBuilder builder_(_fbb);
  builder_.add_features_count(features_count);
  builder_.add_metadata(metadata);
  builder_.add_description(description);
  builder_.add_title(title);
  builder_.add_crs(crs);
  builder_.add_columns(columns);
  builder_.add_envelope(envelope);
  builder_.add_name(name);
  builder_.add_index_node_size(index_node_size);
  builder_.add_has_tm(has_tm);
  builder_.add_has_t(has_t);
  builder_.add_has_m(has_m);
  builder_.add_has_z(has_z);
  builder_.add_geometry_type(geometry_type);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<Header> CreateHeaderDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const char *name = nullptr,
    const std::vector<double> *envelope = nullptr,
    FlatGeobuf::GeometryType geometry_type = FlatGeobuf::GeometryType::Unknown,
    bool has_z = false,
    bool has_m = false,
    bool has_t = false,
    bool has_tm = false,
    const std::vector<::flatbuffers::Offset<FlatGeobuf::Column>> *columns = nullptr,
    uint64_t features_count = 0,
    uint16_t index_node_size = 16,
    ::flatbuffers::Offset<FlatGeobuf::Crs> crs = 0,
    const char *title = nullptr,
    const char *description = nullptr,
    const char *metadata = nullptr) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  auto envelope__ = envelope ? _fbb.CreateVector<double>(*envelope) : 0;
  auto columns__ = columns ? _fbb.CreateVector<::flatbuffers::Offset<FlatGeobuf::Column>>(*columns) : 0;
  auto title__ = title ? _fbb.CreateString(title) : 0;
  auto description__ = description ? _fbb.CreateString(description) : 0;
  auto metadata__ = metadata ? _fbb.CreateString(metadata) : 0;
  return FlatGeobuf::CreateHeader(
      _fbb,
      name__,
      envelope__,
      geometry_type,
      has_z,
      has_m,
      has_t,
      has_tm,
      columns__,
      features_count,
      index_node_size,
      crs,
      title__,
      description__,
      metadata__);
}

inline const FlatGeobuf::Header *GetHeader(const void *buf) {
  return ::flatbuffers::GetRoot<FlatGeobuf::Header>(buf);
}

inline const FlatGeobuf::Header *GetSizePrefixedHeader(const void *buf) {
  return ::flatbuffers::GetSizePrefixedRoot<FlatGeobuf::Header>(buf);
}

inline bool VerifyHeaderBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<FlatGeobuf::Header>(nullptr);
}

inline bool VerifySizePrefixedHeaderBuffer(
    ::flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<FlatGeobuf::Header>(nullptr);
}

inline void FinishHeaderBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<FlatGeobuf::Header> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedHeaderBuffer(
    ::flatbuffers::FlatBufferBuilder &fbb,
    ::flatbuffers::Offset<FlatGeobuf::Header> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace FlatGeobuf