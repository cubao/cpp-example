#define RAPIDJSON_HAS_STDSTRING

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "value.hpp"

namespace mbgl
{
namespace style
{
namespace expression
{

type::Type typeOf(const Value &value)
{
    return value.match(
        [&](bool) -> type::Type { return type::Boolean; },
        [&](double) -> type::Type { return type::Number; },
        [&](const std::string &) -> type::Type { return type::String; },
        [&](const NullValue &) -> type::Type { return type::Null; },
        [&](const std::unordered_map<std::string, Value> &) -> type::Type {
            return type::Object;
        },
        [&](const std::vector<Value> &arr) -> type::Type {
            optional<type::Type> itemType;
            for (const auto &item : arr) {
                const type::Type t = typeOf(item);
                if (!itemType) {
                    itemType = {t};
                } else if (*itemType == t) {
                    continue;
                } else {
                    itemType = {type::Value};
                    break;
                }
            }

            return type::Array(itemType.value_or(type::Value), arr.size());
        });
}

std::string toString(const Value &value)
{
    return value.match([](const NullValue &) { return std::string(); },
                       [](const std::string &s) { return s; }, // avoid quoting
                       [](const auto &v_) { return stringify(v_); });
}

void writeJSON(rapidjson::Writer<rapidjson::StringBuffer> &writer,
               const Value &value)
{
    value.match(
        [&](const NullValue &) { writer.Null(); },
        [&](bool b) { writer.Bool(b); },
        [&](double f) {
            // make sure integer values are stringified without trailing
            // ".0".
            f == std::floor(f) ? writer.Int(f) : writer.Double(f);
        },
        [&](const std::string &s) { writer.String(s.data(), s.size()); },
        [&](const std::vector<Value> &arr) {
            writer.StartArray();
            for (const auto &item : arr) {
                writeJSON(writer, item);
            }
            writer.EndArray();
        },
        [&](const std::unordered_map<std::string, Value> &obj) {
            writer.StartObject();
            for (const auto &entry : obj) {
                writer.Key(entry.first.c_str());
                writeJSON(writer, entry.second);
            }
            writer.EndObject();
        });
}

std::string stringify(const Value &value)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writeJSON(writer, value);
    return buffer.GetString();
}

struct FromMBGLValue
{
    Value operator()(const std::vector<mbgl::Value> &v)
    {
        std::vector<Value> result;
        result.reserve(v.size());
        for (const auto &item : v) {
            result.emplace_back(toExpressionValue(item));
        }
        return result;
    }

    Value operator()(const std::unordered_map<std::string, mbgl::Value> &v)
    {
        std::unordered_map<std::string, Value> result;
        result.reserve(v.size());
        for (const auto &entry : v) {
            result.emplace(entry.first, toExpressionValue(entry.second));
        }
        return result;
    }

    Value operator()(const std::string &s) { return s; }
    Value operator()(const bool b) { return b; }
    Value operator()(const NullValue) { return Null; }
    Value operator()(const double v) { return v; }
    Value operator()(const uint64_t &v) { return static_cast<double>(v); }
    Value operator()(const int64_t &v) { return static_cast<double>(v); }
};

Value ValueConverter<mbgl::Value>::toExpressionValue(const mbgl::Value &value)
{
    return mbgl::Value::visit(value, FromMBGLValue());
}

mbgl::Value ValueConverter<mbgl::Value>::fromExpressionValue(const Value &value)
{
    return value.match(
        [&](const std::vector<Value> &values) -> mbgl::Value {
            std::vector<mbgl::Value> converted;
            converted.reserve(values.size());
            for (const Value &v : values) {
                converted.emplace_back(fromExpressionValue(v));
            }
            return converted;
        },
        [&](const std::unordered_map<std::string, Value> &values)
            -> mbgl::Value {
            std::unordered_map<std::string, mbgl::Value> converted;
            converted.reserve(values.size());
            for (const auto &entry : values) {
                converted.emplace(entry.first,
                                  fromExpressionValue(entry.second));
            }
            return converted;
        },
        [&](const auto &a) -> mbgl::Value { return a; });
}

Value ValueConverter<float>::toExpressionValue(const float value)
{
    return static_cast<double>(value);
}

optional<float> ValueConverter<float>::fromExpressionValue(const Value &value)
{
    return value.template is<double>()
               ? static_cast<float>(value.template get<double>())
               : optional<float>();
}

template <typename T, typename Container>
std::vector<Value> toArrayValue(const Container &value)
{
    std::vector<Value> result;
    result.reserve(value.size());
    for (const T &item : value) {
        result.push_back(ValueConverter<T>::toExpressionValue(item));
    }
    return result;
}

template <typename T, std::size_t N>
Value ValueConverter<std::array<T, N>>::toExpressionValue(
    const std::array<T, N> &value)
{
    return toArrayValue<T>(value);
}

template <typename T, std::size_t N>
optional<std::array<T, N>>
ValueConverter<std::array<T, N>>::fromExpressionValue(const Value &value)
{
    return value.match(
        [&](const std::vector<Value> &v) -> optional<std::array<T, N>> {
            if (v.size() != N)
                return optional<std::array<T, N>>();
            std::array<T, N> result;
            auto it = result.begin();
            for (const Value &item : v) {
                optional<T> convertedItem =
                    ValueConverter<T>::fromExpressionValue(item);
                if (!convertedItem) {
                    return optional<std::array<T, N>>();
                }
                *it = *convertedItem;
                it = std::next(it);
            }
            return result;
        },
        [&](const auto &) { return optional<std::array<T, N>>(); });
}

template <typename T>
Value ValueConverter<std::vector<T>>::toExpressionValue(
    const std::vector<T> &value)
{
    return toArrayValue<T>(value);
}

template <typename T>
optional<std::vector<T>>
ValueConverter<std::vector<T>>::fromExpressionValue(const Value &value)
{
    return value.match(
        [&](const std::vector<Value> &v) -> optional<std::vector<T>> {
            std::vector<T> result;
            result.reserve(v.size());
            for (const Value &item : v) {
                optional<T> convertedItem =
                    ValueConverter<T>::fromExpressionValue(item);
                if (!convertedItem) {
                    return optional<std::vector<T>>();
                }
                result.push_back(*convertedItem);
            }
            return result;
        },
        [&](const auto &) { return optional<std::vector<T>>(); });
}

template <typename T>
Value ValueConverter<T, std::enable_if_t<std::is_enum<T>::value>>::
    toExpressionValue(const T &value)
{
    return std::string(Enum<T>::toString(value));
}

template <typename T>
optional<T> ValueConverter<T, std::enable_if_t<std::is_enum<T>::value>>::
    fromExpressionValue(const Value &value)
{
    return value.match([&](const std::string &v) { return Enum<T>::toEnum(v); },
                       [&](const auto &) { return optional<T>(); });
}

template <typename T> type::Type valueTypeToExpressionType()
{
    return ValueConverter<T>::expressionType();
}

template <> type::Type valueTypeToExpressionType<Value>()
{
    return type::Value;
}
template <> type::Type valueTypeToExpressionType<NullValue>()
{
    return type::Null;
}
template <> type::Type valueTypeToExpressionType<bool>()
{
    return type::Boolean;
}
template <> type::Type valueTypeToExpressionType<double>()
{
    return type::Number;
}
template <> type::Type valueTypeToExpressionType<std::string>()
{
    return type::String;
}
template <>
type::Type valueTypeToExpressionType<std::unordered_map<std::string, Value>>()
{
    return type::Object;
}
template <> type::Type valueTypeToExpressionType<std::vector<Value>>()
{
    return type::Array(type::Value);
}

// used only for the special (and private) "error" expression
template <> type::Type valueTypeToExpressionType<type::ErrorType>()
{
    return type::Error;
}

// for to_rgba expression
template type::Type valueTypeToExpressionType<std::array<double, 4>>();
template struct ValueConverter<std::array<double, 4>>;

template type::Type valueTypeToExpressionType<std::array<float, 2>>();
template struct ValueConverter<std::array<float, 2>>;

template type::Type valueTypeToExpressionType<std::array<float, 4>>();
template struct ValueConverter<std::array<float, 4>>;

template type::Type valueTypeToExpressionType<std::vector<float>>();
template struct ValueConverter<std::vector<float>>;

template type::Type valueTypeToExpressionType<std::vector<std::string>>();
template struct ValueConverter<std::vector<std::string>>;

} // namespace expression
} // namespace style
} // namespace mbgl
