#ifndef CUBAO_FROM_TO_JSON_HPP
#define CUBAO_FROM_TO_JSON_HPP

// should sync
// -
// https://github.com/cubao/cpp-example/blob/main/src/cpp_example/from_to_json.hpp
// - https://github.com/cubao/headers/tree/main/include/cubao/from_to_json.hpp

#include <Eigen/Core>
#include <mapbox/geojson.hpp>
#include <mapbox/geojson/rapidjson.hpp>
#include <vector>

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

namespace cubao
{
using VariantValue = mapbox::feature::value;
using PropertyMap = mapbox::feature::property_map;
using RapidjsonValue = mapbox::geojson::rapidjson_value;
using RapidjsonAllocator = mapbox::geojson::rapidjson_allocator;
using RapidjsonDocument = mapbox::geojson::rapidjson_document;

constexpr const auto RJFLAGS =
    rapidjson::kParseDefaultFlags | rapidjson::kParseCommentsFlag |
    rapidjson::kParseFullPrecisionFlag | rapidjson::kParseNanAndInfFlag |
    rapidjson::kParseTrailingCommasFlag;

namespace detail
{
template <typename T> struct is_vector_like_impl : std::false_type
{
};
template <typename T, typename A>
struct is_vector_like_impl<std::vector<T, A>> : std::true_type
{
};
template <typename T, typename U = void>
struct is_array_like_impl : std::false_type
{
};
template <typename T, size_t N>
struct is_array_like_impl<std::array<T, N>> : std::true_type
{
};
template <typename T, typename U = void>
struct is_const_char_array_impl : std::false_type
{
};
template <size_t N>
struct is_const_char_array_impl<const char (&)[N]> : std::true_type
{
};
template <typename T, typename U = void>
struct is_pair_like_impl : std::false_type
{
};
template <typename F, typename S>
struct is_pair_like_impl<std::pair<F, S>> : std::true_type
{
};
template <typename T> struct is_tuple_like_impl : std::false_type
{
};
template <typename T, typename... Ts>
struct is_tuple_like_impl<std::tuple<T, Ts...>> : std::true_type
{
};
template <typename T> struct is_set_like_impl : std::false_type
{
};
template <typename K, typename C, typename A>
struct is_set_like_impl<std::set<K, C, A>> : std::true_type
{
};
template <typename K, typename C, typename A>
struct is_set_like_impl<std::unordered_set<K, C, A>> : std::true_type
{
};
template <typename...> struct voider
{
    using type = void;
};
template <typename... T> using void_t = typename voider<T...>::type;
template <typename T, typename U = void>
struct is_map_like_impl : std::false_type
{
};
template <typename T>
struct is_map_like_impl<T,
                        void_t<typename T::key_type, typename T::mapped_type>>
    : std::true_type
{
};

template <typename T> struct is_geometry_type : std::false_type
{
};
#define ENABLE_GEOMETRY_TYPE(GeometryType)                                     \
    template <> struct is_geometry_type<GeometryType> : std::true_type         \
    {                                                                          \
    };
ENABLE_GEOMETRY_TYPE(mapbox::geojson::empty)
ENABLE_GEOMETRY_TYPE(mapbox::geojson::point)
ENABLE_GEOMETRY_TYPE(mapbox::geojson::multi_point)
ENABLE_GEOMETRY_TYPE(mapbox::geojson::line_string)
ENABLE_GEOMETRY_TYPE(mapbox::geojson::linear_ring)
ENABLE_GEOMETRY_TYPE(mapbox::geojson::multi_line_string)
ENABLE_GEOMETRY_TYPE(mapbox::geojson::polygon)
ENABLE_GEOMETRY_TYPE(mapbox::geojson::multi_polygon)
ENABLE_GEOMETRY_TYPE(mapbox::geojson::geometry)
ENABLE_GEOMETRY_TYPE(mapbox::geojson::geometry_collection)
ENABLE_GEOMETRY_TYPE(mapbox::geojson::feature)
ENABLE_GEOMETRY_TYPE(mapbox::geojson::feature_collection)
#undef ENABLE_GEOMETRY_TYPE

// https://codereview.stackexchange.com/a/67394
template <typename Tuple, typename F, std::size_t... Indices>
void tuple_for_each_impl(Tuple &&tuple, F &&f, std::index_sequence<Indices...>)
{
    using swallow = int[];
    (void)swallow{1, (f(std::get<Indices>(std::forward<Tuple>(tuple))), void(),
                      int{})...};
}
} // namespace detail

template <typename T>
struct IS_VECTOR_LIKE : detail::is_vector_like_impl<T>::type
{
};
template <typename T> struct IS_ARRAY_LIKE : detail::is_array_like_impl<T>::type
{
};
template <typename T>
struct IS_CONST_CHAR_ARRAY : detail::is_const_char_array_impl<T>::type
{
};
template <typename T> struct IS_PAIR_LIKE : detail::is_pair_like_impl<T>::type
{
};
template <typename T> struct IS_TUPLE_LIKE : detail::is_tuple_like_impl<T>::type
{
};
template <typename T> struct IS_SET_LIKE : detail::is_set_like_impl<T>::type
{
};
template <typename T> struct IS_MAP_LIKE : detail::is_map_like_impl<T>::type
{
};
template <typename T>
struct IS_GEOMETRY_TYPE : detail::is_geometry_type<T>::type
{
};

template <typename Tuple, typename F> void tuple_for_each(Tuple &&tuple, F &&f)
{
    constexpr std::size_t N =
        std::tuple_size<std::remove_reference_t<Tuple>>::value;
    detail::tuple_for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
                                std::make_index_sequence<N>{});
}
template <typename T> struct HAS_TO_JSON
{
    template <typename U, RapidjsonValue (U::*)(RapidjsonAllocator &) const>
    struct SFINAE
    {
    };
    template <typename U> static char Test(SFINAE<U, &U::to_rapidjson> *);
    template <typename U> static int Test(...);
    static const bool Has = sizeof(Test<T>(0)) == sizeof(char);
};
template <typename T> struct HAS_FROM_JSON
{
    template <typename U, U &(U::*)(const RapidjsonValue &)> struct SFINAE
    {
    };
    template <typename U> static char Test(SFINAE<U, &U::from_rapidjson> *);
    template <typename U> static int Test(...);
    static const bool Has = sizeof(Test<T>(0)) == sizeof(char);
};

////////////////////////////// DUMP & PARSE ///////////////////////////////
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
inline RapidjsonValue loads(const std::string &json, bool raise_error = true)
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

////////////////////////////// TO/FROM STRING DECLARATION /////////////////
// STRING1: const char *, string -> string
template <
    typename T,                                                           //
    std::enable_if_t<std::is_convertible<T, std::string>::value, int> = 0 //
    >
std::string to_string(const T &t, bool indent = false);
// STRING2: T -> json -> string
template <
    typename T,                                                             //
    std::enable_if_t<!std::is_convertible<T, std::string>::value, int> = 0, //
    std::enable_if_t<!std::is_same<T, RapidjsonValue>::value, int> = 0      //
    >
std::string to_string(const T &t, bool indent = false);
// STRING3: json -> stringify
template <
    typename T,                                                             //
    std::enable_if_t<!std::is_convertible<T, std::string>::value, int> = 0, //
    std::enable_if_t<std::is_same<T, RapidjsonValue>::value, int> = 0       //
    >
std::string to_string(const T &json, bool indent = false);
// STRING4: string -> T
template <
    typename T,                                                           //
    std::enable_if_t<std::is_convertible<std::string, T>::value, int> = 0 //
    >
T from_string(const std::string &t, bool raise_error = true);
// STRING5: string -> json -> T
template <
    typename T,                                                             //
    std::enable_if_t<!std::is_convertible<std::string, T>::value, int> = 0, //
    std::enable_if_t<!std::is_same<T, RapidjsonValue>::value, int> = 0      //
    >
T from_string(const std::string &t, bool raise_error = true);
// STRING6: string -> json
template <
    typename T,                                                             //
    std::enable_if_t<!std::is_convertible<std::string, T>::value, int> = 0, //
    std::enable_if_t<std::is_same<T, RapidjsonValue>::value, int> = 0       //
    >
T from_string(const std::string &t, bool raise_error = true);

////////////////////////////// TO/FROM RAPIDJSON DECLARATION //////////////
// RAPIDJSON0: * -> json
template <                                                   //
    typename T,                                              //
    std::enable_if_t<!HAS_TO_JSON<T>::Has, int> = 0,         //
    std::enable_if_t<!HAS_TB_TO_RAPIDJSON<T>::Has, int> = 0, //
    std::enable_if_t<!IS_VECTOR_LIKE<T>{}, int> = 0,         //
    std::enable_if_t<!IS_ARRAY_LIKE<T>{}, int> = 0,          //
    std::enable_if_t<!IS_PAIR_LIKE<T>{}, int> = 0,           //
    std::enable_if_t<!IS_TUPLE_LIKE<T>{}, int> = 0,          //
    std::enable_if_t<!IS_SET_LIKE<T>{}, int> = 0,            //
    std::enable_if_t<!IS_MAP_LIKE<T>{}, int> = 0,            //
    std::enable_if_t<!std::is_integral<T>::value, int> = 0   //
    >
RapidjsonValue to_rapidjson(T const &item, RapidjsonAllocator &allocator);
// RAPIDJSON1: {t.to_rapidjson(allocator)} -> json
template <                                         //
    typename T,                                    //
    std::enable_if_t<HAS_TO_JSON<T>::Has, int> = 0 //
    >
RapidjsonValue to_rapidjson(T const &item, RapidjsonAllocator &allocator);
// RAPIDJSON2: {t.to_rapidjson} -> json
template <typename T,                                            //
          std::enable_if_t<!HAS_TO_JSON<T>::Has, int> = 0,       //
          std::enable_if_t<HAS_TB_TO_RAPIDJSON<T>::Has, int> = 0 //
          >
RapidjsonValue to_rapidjson(T const &item, RapidjsonAllocator &allocator);
// RAPIDJSON3: [vector] -> [json]
template <                                         //
    typename V,                                    //
    std::enable_if_t<IS_VECTOR_LIKE<V>{}, int> = 0 //
    >
RapidjsonValue to_rapidjson(V const &items, RapidjsonAllocator &allocator);
// RAPIDJSON4: [array] -> [json]
template <                                        //
    typename A,                                   //
    std::enable_if_t<IS_ARRAY_LIKE<A>{}, int> = 0 //
    >
RapidjsonValue to_rapidjson(A const &items, RapidjsonAllocator &allocator);
// RAPIDJSON5: [F, S] -> [F->json, S->json]
template <                                       //
    typename P,                                  //
    std::enable_if_t<IS_PAIR_LIKE<P>{}, int> = 0 //
    >
RapidjsonValue to_rapidjson(P const &items, RapidjsonAllocator &allocator);
// RAPIDJSON6: [T1, ...] -> [T1->json, ...]
template <                                        //
    typename T,                                   //
    std::enable_if_t<IS_TUPLE_LIKE<T>{}, int> = 0 //
    >
RapidjsonValue to_rapidjson(T const &items, RapidjsonAllocator &allocator);
// RAPIDJSON7: {set} -> [json]
template <                                      //
    typename T,                                 //
    std::enable_if_t<IS_SET_LIKE<T>{}, int> = 0 //
    >
RapidjsonValue to_rapidjson(T const &items, RapidjsonAllocator &allocator);
// RAPIDJSON8: {k -> v} -> {json(k) -> json(v)}
template <                                       //
    typename KV,                                 //
    std::enable_if_t<IS_MAP_LIKE<KV>{}, int> = 0 //
    >
RapidjsonValue to_rapidjson(KV const &items, RapidjsonAllocator &allocator);
template <      //
    typename T, //
    std::enable_if_t<std::is_integral<T>::value, int> = 0,
    std::enable_if_t<!std::is_same<T, bool>::value, int> = 0 //
    >
RapidjsonValue to_rapidjson(T const &items, RapidjsonAllocator &allocator);
template <                                                  //
    typename T,                                             //
    std::enable_if_t<std::is_same<T, bool>::value, int> = 0 //
    >
RapidjsonValue to_rapidjson(T const &items, RapidjsonAllocator &allocator);
// RAPIDJSON9: provide an allocator
template <typename T> //
RapidjsonValue to_rapidjson(T const &data);
// RAPIDJSON0: json -> *
template <                                                    //
    typename T,                                               //
    std::enable_if_t<!HAS_FROM_JSON<T>::Has, int> = 0,        //
    std::enable_if_t<!HAS_FROM_JSON_STATIC<T>::Has, int> = 0, //
    std::enable_if_t<!IS_VECTOR_LIKE<T>{}, int> = 0,          //
    std::enable_if_t<!IS_ARRAY_LIKE<T>{}, int> = 0,           //
    std::enable_if_t<!IS_PAIR_LIKE<T>{}, int> = 0,            //
    std::enable_if_t<!IS_TUPLE_LIKE<T>{}, int> = 0,           //
    std::enable_if_t<!IS_SET_LIKE<T>{}, int> = 0,             //
    std::enable_if_t<!IS_MAP_LIKE<T>{}, int> = 0,             //
    std::enable_if_t<!std::is_integral<T>::value, int> = 0    //
    >
T from_rapidjson(const RapidjsonValue &json);
// RAPIDJSON1: json -> {t.from_rapidjson}
template <                                           //
    typename T,                                      //
    std::enable_if_t<HAS_FROM_JSON<T>::Has, int> = 0 //
    >
T from_rapidjson(const RapidjsonValue &json);
// RAPIDJSON2: json -> {T.from_rapidjson}
template <                                                  //
    typename T,                                             //
    std::enable_if_t<!HAS_FROM_JSON<T>::Has, int> = 0,      //
    std::enable_if_t<HAS_FROM_JSON_STATIC<T>::Has, int> = 0 //
    >
T from_rapidjson(const RapidjsonValue &json);
// RAPIDJSON3: [json] -> [vector]
template <                                         //
    typename T,                                    //
    std::enable_if_t<IS_VECTOR_LIKE<T>{}, int> = 0 //
    >
T from_rapidjson(const RapidjsonValue &json);
// RAPIDJSON4: {k -> v} -> {json->Tk -> v->Tv}
template <                                        //
    typename T,                                   //
    std::enable_if_t<IS_ARRAY_LIKE<T>{}, int> = 0 //
    >
T from_rapidjson(const RapidjsonValue &json);
// RAPIDJSON5: [json] -> [pair]
template <                                       //
    typename T,                                  //
    std::enable_if_t<IS_PAIR_LIKE<T>{}, int> = 0 //
    >
T from_rapidjson(const RapidjsonValue &json);
// RAPIDJSON6: [json] -> [tuple]
template <                                        //
    typename T,                                   //
    std::enable_if_t<IS_TUPLE_LIKE<T>{}, int> = 0 //
    >
T from_rapidjson(const RapidjsonValue &json);
// RAPIDJSON7: [json] -> [set]
template <                                      //
    typename T,                                 //
    std::enable_if_t<IS_SET_LIKE<T>{}, int> = 0 //
    >
T from_rapidjson(const RapidjsonValue &json);
// RAPIDJSON8: {k -> v} -> {str/json->Tk -> v->Tv}
template <                                      //
    typename T,                                 //
    std::enable_if_t<IS_MAP_LIKE<T>{}, int> = 0 //
    >
T from_rapidjson(const RapidjsonValue &json);
template <                                                 //
    typename T,                                            //
    std::enable_if_t<std::is_integral<T>::value, int> = 0, //
    std::enable_if_t<!std::is_same<T, bool>::value, int> = 0>
T from_rapidjson(const RapidjsonValue &json);
template <      //
    typename T, //
    std::enable_if_t<std::is_same<T, bool>::value, int> = 0>
T from_rapidjson(const RapidjsonValue &json);

////////////////////////////// TO/FROM STRING IMPLEMENTATION /////////////
// STRING1: const char *, string -> string
template <typename T,                                                       //
          std::enable_if_t<std::is_convertible<T, std::string>::value, int> //
          >
std::string to_string(const T &t, bool indent)
{
    return t;
}
// STRING2: T -> json -> string
template <typename T,                                                         //
          std::enable_if_t<!std::is_convertible<T, std::string>::value, int>, //
          std::enable_if_t<!std::is_same<T, RapidjsonValue>::value, int>      //
          >
std::string to_string(const T &t, bool indent)
{
    if (detail::const_char_array_or_not(t)) {
        bool fullwidth = indent;
        return detail::const_char_array_to_string(t, fullwidth);
    }
    return dump(to_rapidjson(t), indent);
}
// STRING3: json -> stringify
template <typename T,                                                         //
          std::enable_if_t<!std::is_convertible<T, std::string>::value, int>, //
          std::enable_if_t<std::is_same<T, RapidjsonValue>::value, int>       //
          >
std::string to_string(const T &json, bool indent)
{
    return dump(json, indent);
}
// STRING4: string -> T
template <typename T,                                                       //
          std::enable_if_t<std::is_convertible<std::string, T>::value, int> //
          >
T from_string(const std::string &t, bool raise_error)
{
    return t;
}
// STRING5: string -> json -> T
template <typename T,                                                         //
          std::enable_if_t<!std::is_convertible<std::string, T>::value, int>, //
          std::enable_if_t<!std::is_same<T, RapidjsonValue>::value, int>      //
          >
T from_string(const std::string &t, bool raise_error)
{
    return from_rapidjson<T>(parse(t, raise_error));
}
// STRING6: string -> json
template <typename T,                                                         //
          std::enable_if_t<!std::is_convertible<std::string, T>::value, int>, //
          std::enable_if_t<std::is_same<T, RapidjsonValue>::value, int>       //
          >
T from_string(const std::string &t, bool raise_error)
{
    return parse(t, raise_error);
}

////////////////////////////// TO/FROM RAPIDJSON IMPLEMENTATION ///////////
// RAPIDJSON1: {t.to_rapidjson(allocator)} -> json
template <                                     //
    typename T,                                //
    std::enable_if_t<HAS_TO_JSON<T>::Has, int> //
    >
RapidjsonValue to_rapidjson(T const &item, RapidjsonAllocator &allocator)
{
    return item.to_rapidjson(allocator);
}
// RAPIDJSON2: {t.to_rapidjson} -> json
template <typename T,                                        //
          std::enable_if_t<!HAS_TO_JSON<T>::Has, int>,       //
          std::enable_if_t<HAS_TB_TO_RAPIDJSON<T>::Has, int> //
          >
RapidjsonValue to_rapidjson(T const &item, RapidjsonAllocator &allocator)
{
    return item.to_rapidjson();
}
// RAPIDJSON3: [vector] -> [json]
template <                                     //
    typename T,                                //
    std::enable_if_t<IS_VECTOR_LIKE<T>{}, int> //
    >
RapidjsonValue to_rapidjson(T const &items, RapidjsonAllocator &allocator)
{
    RapidjsonValue ret(rapidjson::kArrayType);
    ret.Reserve(items.size(), allocator);
    for (auto &e : items) {
        ret.PushBack(to_rapidjson(e, allocator), allocator);
    }
    return ret;
}
// RAPIDJSON4: [array] -> [json]
template <                                    //
    typename T,                               //
    std::enable_if_t<IS_ARRAY_LIKE<T>{}, int> //
    >
RapidjsonValue to_rapidjson(T const &items, RapidjsonAllocator &allocator)
{
    RapidjsonValue ret(rapidjson::kArrayType);
    ret.Reserve(items.size(), allocator);
    for (auto &e : items) {
        ret.PushBack(to_rapidjson(e, allocator), allocator);
    }
    return ret;
}
// RAPIDJSON5: [F, S] -> [json]
template <                                   //
    typename T,                              //
    std::enable_if_t<IS_PAIR_LIKE<T>{}, int> //
    >
RapidjsonValue to_rapidjson(T const &p, RapidjsonAllocator &allocator)
{
    RapidjsonValue ret(rapidjson::kArrayType);
    ret.Reserve(2, allocator);
    ret.PushBack(to_rapidjson(p.first, allocator), allocator);
    ret.PushBack(to_rapidjson(p.second, allocator), allocator);
    return ret;
}
// RAPIDJSON6: [T1, ...] -> [T1->json, ...]
template <                                    //
    typename T,                               //
    std::enable_if_t<IS_TUPLE_LIKE<T>{}, int> //
    >
RapidjsonValue to_rapidjson(T const &t, RapidjsonAllocator &allocator)
{
    RapidjsonValue ret(rapidjson::kArrayType);
    ret.Reserve(std::tuple_size<T>::value, allocator);
    tuple_for_each(t, [&](auto &e) {
        ret.PushBack(to_rapidjson(e, allocator), allocator);
    });
    return ret;
}
// RAPIDJSON7: {set} -> [json]
template <                                  //
    typename T,                             //
    std::enable_if_t<IS_SET_LIKE<T>{}, int> //
    >
RapidjsonValue to_rapidjson(T const &t, RapidjsonAllocator &allocator)
{
    RapidjsonValue ret(rapidjson::kArrayType);
    ret.Reserve(t.size(), allocator);
    for (auto &e : t) {
        ret.PushBack(to_rapidjson(e, allocator), allocator);
    }
    return ret;
}
// RAPIDJSON8: {k -> v} -> {json(k) -> json(v)}
template <                                   //
    typename KV,                             //
    std::enable_if_t<IS_MAP_LIKE<KV>{}, int> //
    >
RapidjsonValue to_rapidjson(KV const &items, RapidjsonAllocator &allocator)
{
    RapidjsonValue ret(rapidjson::kObjectType);
    for (auto &e : items) {
        auto key = to_string(e.first);
        ret.AddMember(RapidjsonValue(key.data(), key.size(), allocator),
                      to_rapidjson(e.second, allocator), allocator);
    }
    return ret;
}
template <                                               //
    typename T,                                          //
    std::enable_if_t<std::is_integral<T>::value, int>,   //
    std::enable_if_t<!std::is_same<T, bool>::value, int> //
    >
RapidjsonValue to_rapidjson(T const &num, RapidjsonAllocator &allocator)
{
    if (sizeof(T) < sizeof(int64_t)) {
        return std::is_signed<T>::value
                   ? RapidjsonValue(static_cast<int32_t>(num))
                   : RapidjsonValue(static_cast<uint32_t>(num));
    } else {
        return std::is_signed<T>::value
                   ? RapidjsonValue(static_cast<int64_t>(num))
                   : RapidjsonValue(static_cast<uint64_t>(num));
    }
}
template <typename T, std::enable_if_t<std::is_same<T, bool>::value, int>>
RapidjsonValue to_rapidjson(T const &items, RapidjsonAllocator &allocator)
{
    return RapidjsonValue(static_cast<bool>(items));
}
// RAPIDJSON9: provide an allocator
template <typename T> //
RapidjsonValue to_rapidjson(T const &data)
{
    RapidjsonAllocator allocator;
    return to_rapidjson(data, allocator);
}
// RAPIDJSON0: json -> *
// RAPIDJSON1: json -> {t.from_rapidjson}
template <                                       //
    typename T,                                  //
    std::enable_if_t<HAS_FROM_JSON<T>::Has, int> //
    >
T from_rapidjson(const RapidjsonValue &json)
{
    return T().from_rapidjson(json);
}
// RAPIDJSON2: json -> {T.from_rapidjson}
template <                                              //
    typename T,                                         //
    std::enable_if_t<!HAS_FROM_JSON<T>::Has, int>,      //
    std::enable_if_t<HAS_FROM_JSON_STATIC<T>::Has, int> //
    >
T from_rapidjson(const RapidjsonValue &json)
{
    return T::from_rapidjson(json);
}
// RAPIDJSON3: [json] -> [vector]
template <                                     //
    typename T,                                //
    std::enable_if_t<IS_VECTOR_LIKE<T>{}, int> //
    >
T from_rapidjson(const RapidjsonValue &json)
{
    T ret;
    ret.reserve(json.Size());
    for (auto &e : json.GetArray()) {
        ret.push_back(from_rapidjson<typename T::value_type>(e));
    }
    return ret;
}
// RAPIDJSON4: [json] -> [array]
template <                                    //
    typename T,                               //
    std::enable_if_t<IS_ARRAY_LIKE<T>{}, int> //
    >
T from_rapidjson(const RapidjsonValue &json)
{
    T ret;
    for (size_t i = 0; i < ret.size(); ++i) {
        ret[i] = from_rapidjson<typename T::value_type>(json[i]);
    }
    return ret;
}
// RAPIDJSON5: [json] -> [F, S]
template <                                   //
    typename T,                              //
    std::enable_if_t<IS_PAIR_LIKE<T>{}, int> //
    >
T from_rapidjson(const RapidjsonValue &json)
{
    return std::make_pair(from_rapidjson<typename T::first_type>(json[0]),
                          from_rapidjson<typename T::second_type>(json[1]));
}
// RAPIDJSON6: [json] -> [tuple]
template <                                    //
    typename T,                               //
    std::enable_if_t<IS_TUPLE_LIKE<T>{}, int> //
    >
T from_rapidjson(const RapidjsonValue &json)
{
    T t;
    int index = 0;
    tuple_for_each(t, [&](auto &e) {
        e = from_rapidjson<typename std::decay<decltype(e)>::type>(
            json[index++]);
    });
    return t;
}
// RAPIDJSON7: [json] -> [set]
template <                                  //
    typename T,                             //
    std::enable_if_t<IS_SET_LIKE<T>{}, int> //
    >
T from_rapidjson(const RapidjsonValue &json)
{
    T ret;
    for (auto &e : json.GetArray()) {
        ret.insert(from_rapidjson<typename T::key_type>(e));
    }
    return ret;
}
// RAPIDJSON8: {k -> v} -> {str/json->Tk -> v->Tv}
template <                                  //
    typename T,                             //
    std::enable_if_t<IS_MAP_LIKE<T>{}, int> //
    >
T from_rapidjson(const RapidjsonValue &json)
{
    T ret;
    for (auto &p : json.GetObject()) {
        auto key = from_string<typename T::key_type>(
            std::string(p.name.GetString(), p.name.GetStringLength()));
        auto value = from_rapidjson<typename T::mapped_type>(p.value);
        ret.emplace(key, value);
    }
    return ret;
}
template <                                             //
    typename T,                                        //
    std::enable_if_t<std::is_integral<T>::value, int>, //
    std::enable_if_t<!std::is_same<T, bool>::value, int>>
T from_rapidjson(const RapidjsonValue &json)
{
    if (sizeof(T) < sizeof(int64_t)) {
        return std::is_signed<T>::value ? static_cast<T>(json.GetInt())
                                        : static_cast<T>(json.GetUint());
    } else {
        return std::is_signed<T>::value ? static_cast<T>(json.GetInt64())
                                        : static_cast<T>(json.GetUint64());
    }
}
template <      //
    typename T, //
    std::enable_if_t<std::is_same<T, bool>::value, int>>
T from_rapidjson(const RapidjsonValue &json)
{
    return json.GetBool();
}
} // namespace cubao

#endif
