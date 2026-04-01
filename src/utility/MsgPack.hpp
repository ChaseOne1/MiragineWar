#pragma once

namespace utility::mp {
namespace detail {
    enum class endian
    {
#if defined(_MSC_VER) && !defined(__clang__)
        little = 0,
        big = 1,
        native = little
#else
        little = __ORDER_LITTLE_ENDIAN__,
        big = __ORDER_BIG_ENDIAN__,
        native = __BYTE_ORDER__
#endif
    };

    enum type : uint8_t
    {
        PFIXINT = 0x00, NFIXINT = 0xE0,
        UINT8 = 0xCC, INT8 = 0xD0, UINT16 = 0xCD, INT16 = 0xD1,
        UINT32 = 0xCE, INT32 = 0xD2, UINT64 = 0xCF, INT64 = 0xD3,
        NIL = 0xC0, TTURE = 0xC3, TFALSE = 0xC2,
        FLOAT = 0xCA, DOUBLE = 0xCB,
        FIXARRAY = 0x90, ARRAY16 = 0xDC, ARRAY32 = 0xDD,
        FIXMAP = 0x80, MAP16 = 0xDE, MAP32 = 0XDF,
        FIXSTR = 0xA0, STR8 = 0xD9, STR16 = 0xDA, STR32 = 0xDB,
    };

    template <typename T>
    inline void write_bytes(std::string& out, const T& value) noexcept
    {
        static_assert(std::is_arithmetic_v<T>, "T must be arithmetic");
        static_assert(!std::is_same_v<T, bool>, "bool is not supported for write_bytes");

        const std::uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
        if constexpr (endian::native == endian::little) {
            for (int i = sizeof(T) - 1; i >= 0; --i) {
                out.push_back(ptr[i]);
            }
        } else {
            out.append(reinterpret_cast<const char*>(ptr), sizeof(T));
        }
    }

    template <typename T>
    inline T read_bytes(std::string_view& data)
    {
        static_assert(std::is_arithmetic_v<T>, "T must be arithmetic type");
        static_assert(!std::is_same_v<T, bool>, "bool is not supported for read_bytes");

        if (data.size() < sizeof(T))
            throw std::runtime_error("decode: buffer underflow");

        T val {};
        if constexpr (endian::native == endian::little) {
            for (size_t i = 0; i < sizeof(T); ++i)
                reinterpret_cast<std::uint8_t*>(&val)[sizeof(T) - 1 - i] = static_cast<uint8_t>(data[i]);
        } else {
            std::memcpy(&val, data.data(), sizeof(T));
        }

        data.remove_prefix(sizeof(T));
        return val;
    }

    inline void encode_value(std::string& data, const sol::object& value) noexcept;
    inline sol::object decode_value(std::string_view& data, const sol::state& state);

    template <typename T, typename = void>
    struct has_reserve : std::false_type {};

    template <typename T>
    struct has_reserve<T, std::void_t<decltype(std::declval<T>().reserve(0))>> : std::true_type {};

    template <typename T>
    inline constexpr bool has_reserve_v = has_reserve<T>::value;
}

namespace encode {
    inline void integer(std::string& data, std::int64_t value) noexcept
    {
        if (value >= 0) {
            if (value <= std::numeric_limits<std::int8_t>::max()) {
                data.push_back(value);
            } else if (value <= std::numeric_limits<std::uint8_t>::max()) {
                data.push_back(detail::type::UINT8);
                data.push_back(value);
            } else if (value <= std::numeric_limits<std::uint16_t>::max()) {
                data.push_back(detail::type::UINT16);
                detail::write_bytes(data, static_cast<std::uint16_t>(value));
            } else if (value <= std::numeric_limits<std::uint32_t>::max()) {
                data.push_back(detail::type::UINT32);
                detail::write_bytes(data, static_cast<std::uint32_t>(value));
            } else {
                data.push_back(detail::type::UINT64);
                detail::write_bytes(data, static_cast<uint64_t>(value));
            }
        } else {
            if (value >= -32) {
                data.push_back(value);
            } else if (value >= std::numeric_limits<int8_t>::min()) {
                data.push_back(detail::type::INT8);
                data.push_back(value);
            } else if (value >= std::numeric_limits<int16_t>::min()) {
                data.push_back(detail::type::INT16);
                detail::write_bytes(data, static_cast<std::int16_t>(value));
            } else if (value >= std::numeric_limits<int32_t>::min()) {
                data.push_back(detail::type::INT32);
                detail::write_bytes(data, static_cast<int32_t>(value));
            } else {
                data.push_back(detail::type::INT64);
                detail::write_bytes(data, static_cast<std::int64_t>(value));
            }
        }
    }

    inline void floating(std::string& data, double value) noexcept
    {
        if (value == static_cast<float>(value)) {
            data.push_back(detail::type::FLOAT);
            detail::write_bytes(data, static_cast<float>(value));
        } else {
            data.push_back(detail::type::DOUBLE);
            detail::write_bytes(data, static_cast<double>(value));
        }
    }

    inline void boolean(std::string& data, bool value) noexcept
    {
        if (value) data.push_back(detail::type::TTURE);
        else data.push_back(detail::type::TFALSE);
    }

    inline void nil(std::string& data) noexcept
    {
        data.push_back(detail::type::NIL);
    }

    inline void string(std::string& data, sol::string_view sv) noexcept
    {
        if (sv.size() < 32) {
            data.push_back(detail::type::FIXSTR | (sv.size() & 0xFF));
        } else if (sv.size() <= 0xFF) {
            data.push_back(detail::type::STR8);
            data.push_back(sv.size());
        } else if (sv.size() <= 0xFFFF) {
            data.push_back(detail::type::STR16);
            detail::write_bytes(data, static_cast<std::uint16_t>(sv.size()));
        } else {
            data.push_back(detail::type::STR32);
            detail::write_bytes(data, static_cast<std::uint32_t>(sv.size()));
        }
        data.append(sv);
    }

    template <typename KVContainer>
    inline void map(std::string& data, const KVContainer& container) noexcept
    {
        static_assert(std::is_arithmetic_v<typename KVContainer::key_type>
                || std::is_same_v<typename KVContainer::key_type, std::string>,
            "Key type must be arithmetic/string");
        static_assert(std::is_arithmetic_v<typename KVContainer::mapped_type>
                || std::is_same_v<typename KVContainer::mapped_type, std::string>,
            "Value type must be arithmetic/string");

        const size_t map_size = container.size();
        if (map_size <= 15) {
            data.push_back(static_cast<uint8_t>(detail::type::FIXMAP) | static_cast<uint8_t>(map_size));
        } else if (map_size <= std::numeric_limits<uint16_t>::max()) {
            data.push_back(static_cast<uint8_t>(detail::type::MAP16));
            detail::write_bytes(data, static_cast<uint16_t>(map_size));
        } else if (map_size <= std::numeric_limits<uint32_t>::max()) {
            data.push_back(static_cast<uint8_t>(detail::type::MAP32));
            detail::write_bytes(data, static_cast<uint32_t>(map_size));
        } else {
            LOGE("map too large (exceeds 2^32-1)");
            return;
        }

        for (const auto& [key, value] : container) {
            if constexpr (std::is_integral_v<typename KVContainer::key_type>) {
                encode::integer(data, static_cast<int64_t>(key));
            } else if constexpr (std::is_floating_point_v<typename KVContainer::key_type>) {
                encode::floating(data, static_cast<double>(key));
            } else if constexpr (std::is_same_v<typename KVContainer::key_type, std::string>) {
                encode::string(data, sol::string_view { key.data(), key.size() });
            }

            if constexpr (std::is_integral_v<typename KVContainer::mapped_type>) {
                encode::integer(data, static_cast<int64_t>(value));
            } else if constexpr (std::is_floating_point_v<typename KVContainer::mapped_type>) {
                encode::floating(data, static_cast<double>(value));
            } else if constexpr (std::is_same_v<typename KVContainer::mapped_type, std::string>) {
                encode::string(data, sol::string_view { value.data(), value.size() });
            }
        }
    }

    inline void map(std::string& data, const sol::table& tbl) noexcept
    {
        std::size_t map_size = 0;
        for (const auto& [k, v] : tbl) ++map_size;

        if (map_size <= 15) {
            data.push_back(static_cast<uint8_t>(detail::type::FIXMAP) | static_cast<uint8_t>(map_size));
        } else if (map_size <= std::numeric_limits<uint16_t>::max()) {
            data.push_back(static_cast<uint8_t>(detail::type::MAP16));
            detail::write_bytes(data, static_cast<uint16_t>(map_size));
        } else if (map_size <= std::numeric_limits<uint32_t>::max()) {
            data.push_back(static_cast<uint8_t>(detail::type::MAP32));
            detail::write_bytes(data, static_cast<uint32_t>(map_size));
        } else {
            LOGE("map too large (exceeds 2^32-1)");
            return;
        }

        for (const auto& [k, v] : tbl) {
            detail::encode_value(data, sol::object(tbl.lua_state(), k));
            detail::encode_value(data, sol::object(tbl.lua_state(), v));
        }
    }

    inline void array(std::string& data, const sol::table& tbl) noexcept
    {
        const size_t array_size = tbl.size();

        if (array_size <= 15) {
            data.push_back(static_cast<uint8_t>(detail::type::FIXARRAY) | static_cast<uint8_t>(array_size));
        } else if (array_size <= std::numeric_limits<uint16_t>::max()) {
            data.push_back(static_cast<uint8_t>(detail::type::ARRAY16));
            detail::write_bytes(data, static_cast<uint16_t>(array_size));
        } else if (array_size <= std::numeric_limits<uint32_t>::max()) {
            data.push_back(static_cast<uint8_t>(detail::type::ARRAY32));
            detail::write_bytes(data, static_cast<uint32_t>(array_size));
        } else {
            LOGE("array too large (exceeds 2^32-1)");
            return;
        }

        for (size_t i = 1; i <= array_size; ++i) {
            sol::object elem = tbl[i];
            if (!elem.valid()) {
                LOGW("missing element at index {}", i);
                detail::encode_value(data, sol::nil);
                continue;
            }
            detail::encode_value(data, elem);
        }
    }
}

namespace detail {
    inline void encode_value(std::string& data, const sol::object& value) noexcept
    {
        switch (value.get_type()) {
        case sol::type::boolean:
            encode::boolean(data, value.as<bool>());
            break;
        case sol::type::number:
            if (value.is<lua_Integer>()) {
                encode::integer(data, value.as<int64_t>());
            } else {
                encode::floating(data, value.as<double>());
            }
            break;
        case sol::type::lua_nil:
            encode::nil(data);
            break;
        case sol::type::string:
            encode::string(data, value.as<sol::string_view>());
            break;
        case sol::type::table: {
            sol::table tbl = value.as<sol::table>();

            // check type
            bool is_array = true;
            size_t array_size = tbl.size();
            for (size_t i = 1; i <= array_size; ++i) {
                if (!tbl[i].valid()) {
                    is_array = false;
                    break;
                }
            }
            for (const auto& [k, v] : tbl) {
                if (!k.is<lua_Integer>() || k.as<lua_Integer>() < 1 || k.as<lua_Integer>() > array_size) {
                    is_array = false;
                    break;
                }
            }

            if (is_array) encode::array(data, tbl);
            else encode::map(data, tbl);

            break;
        }
        default:
            LOGW("unsupported type ({})", static_cast<int>(value.get_type()));
            break;
        }
    }
}

inline void pack(std::string& data, const sol::variadic_args& args)
{
    data.reserve(args.size() * 8); // just an estimate

    for (const auto& value : args)
        detail::encode_value(data, value);
}

inline std::string pack(const sol::variadic_args& args)
{
    std::string data;
    pack(data, args);
    return data;
}

namespace decode {
    inline lua_Integer fix_integer(std::string_view& data)
    {
        if (data.empty()) {
            throw std::runtime_error("decode::fix_integer: empty buffer");
        }

        const uint8_t byte = data[0];
        data.remove_prefix(1);
        return byte;
    }

    inline sol::object fix_integer(std::string_view& data, const sol::state& state)
    {
        if (data.empty()) {
            throw std::runtime_error("decode::fix_integer: empty buffer");
        }

        const uint8_t byte = data[0];
        data.remove_prefix(1);
        return sol::make_object(state, static_cast<lua_Integer>(byte));
    }

    inline lua_Integer integer(std::string_view& data)
    {
        if (data.empty()) {
            throw std::runtime_error("decode::integer: empty buffer");
        }
        
        const uint8_t type = data[0];
        data.remove_prefix(1);

        switch (type) {
        case detail::UINT8:     return detail::read_bytes<uint8_t>(data);
        case detail::UINT16:    return detail::read_bytes<uint16_t>(data);
        case detail::UINT32:    return detail::read_bytes<uint32_t>(data);
        case detail::UINT64:    return detail::read_bytes<uint64_t>(data);
        case detail::INT8:      return detail::read_bytes<int8_t>(data);
        case detail::INT16:     return detail::read_bytes<int16_t>(data);
        case detail::INT32:     return detail::read_bytes<int32_t>(data);
        case detail::INT64:     return detail::read_bytes<int64_t>(data);
        default:
            LOGW("invalid integer type: {}", type);
            return 0;
        }
    }

    inline sol::object integer(std::string_view& data, const sol::state& state)
    {
        if (data.empty()) {
            throw std::runtime_error("decode::integer: empty buffer");
        }

        const uint8_t type = data[0];
        data.remove_prefix(1);

        switch (type) {
        case detail::UINT8:     return sol::make_object(state, detail::read_bytes<uint8_t>(data));
        case detail::UINT16:    return sol::make_object(state, detail::read_bytes<uint16_t>(data));
        case detail::UINT32:    return sol::make_object(state, detail::read_bytes<uint32_t>(data));
        case detail::UINT64:    return sol::make_object(state, detail::read_bytes<uint64_t>(data));
        case detail::INT8:      return sol::make_object(state, detail::read_bytes<int8_t>(data));
        case detail::INT16:     return sol::make_object(state, detail::read_bytes<int16_t>(data));
        case detail::INT32:     return sol::make_object(state, detail::read_bytes<int32_t>(data));
        case detail::INT64:     return sol::make_object(state, detail::read_bytes<int64_t>(data));
        default:
            LOGW("invalid integer type: {}", type);
            return sol::nil;
        }
    }

    inline lua_Number floating(std::string_view& data)
    {
        if (data.empty()) {
            throw std::runtime_error("decode::floating: empty buffer");
        }

        const std::uint8_t type = data[0];
        data.remove_prefix(1);

        if (type == detail::type::FLOAT) return detail::read_bytes<float>(data);
        if (type == detail::type::DOUBLE) return detail::read_bytes<double>(data);

        LOGW("invalid floating type: {}", type);
        return 0.0;
    }

    inline sol::object floating(std::string_view& data, const sol::state& state)
    {
        if (data.empty()) {
            throw std::runtime_error("decode::floating: empty buffer");
        }

        const std::uint8_t type = data[0];
        data.remove_prefix(1);

        if (type == detail::type::FLOAT) return sol::make_object(state, detail::read_bytes<float>(data));
        if (type == detail::type::DOUBLE) return sol::make_object(state, detail::read_bytes<double>(data));

        LOGW("invalid floating type: {}", type);
        return sol::nil;
    }

    inline bool boolean(std::string_view& data)
    {
        if (data.empty()) {
            throw std::runtime_error("decode::boolean: empty buffer");
        }

        const std::uint8_t type = data[0];
        data.remove_prefix(1);

        if (type == detail::type::TTURE) return true;
        if (type == detail::type::TFALSE) return false;

        LOGW("invalid boolean type: {}", type);
        return false;
    }

    inline sol::object boolean(std::string_view& data, const sol::state& state)
    {
        if (data.empty()) {
            throw std::runtime_error("decode::boolean: empty buffer");
        }

        const std::uint8_t type = data[0];
        data.remove_prefix(1);

        if (type == detail::type::TTURE) return sol::make_object(state, true);
        if (type == detail::type::TFALSE) return sol::make_object(state, false);

        LOGW("invalid boolean type: {}", type);
        return sol::nil;
    }

    inline sol::object nil(std::string_view& data)
    {
        if (data.empty()) {
            throw std::runtime_error("decode::nil: empty buffer");
        }

        data.remove_prefix(1);
        return sol::nil;
    }

    inline std::string_view fixstring(std::string_view& data)
    {
        if (data.empty()) {
            throw std::runtime_error("decode::string: empty buffer");
        }

        const std::uint8_t type = data[0];
        data.remove_prefix(1);

        const std::size_t len = type & 0x1F;

        if (data.size() < len) {

            LOGW("string length exceeds buffer: expected length = {}, buffer size = {}",
                len, data.size());
            return {};
        }

        const sol::string_view str { data.data(), len };
        data.remove_prefix(len);
        return str;
    }

    inline std::string_view string(std::string_view& data)
    {
        if (data.empty()) {
            throw std::runtime_error("decode::string: empty buffer");
        }

        const std::uint8_t type = data[0];
        data.remove_prefix(1);

        std::size_t len = 0;
        switch (type) {
        case detail::type::STR8:  len = detail::read_bytes<uint8_t>(data); break;
        case detail::type::STR16: len = detail::read_bytes<uint16_t>(data); break;
        case detail::type::STR32: len = detail::read_bytes<uint32_t>(data); break;
        default:
            LOGW("invalid string type: {}", type);
            return {};
        }

        if (data.size() < len) {
            LOGW("string length exceeds buffer:"
                    "expected length = {} buffer size = {}",
                len, data.size());
            return {};
        }

        const std::string_view str { data.data(), len };
        data.remove_prefix(len);
        return str;
    }

    inline sol::object fixstring(std::string_view& data, const sol::state& state)
    {
        if (data.empty()) {
            throw std::runtime_error("decode::string: empty buffer");
        }

        const std::uint8_t type = data[0];
        data.remove_prefix(1);

        const std::size_t len = type & 0x1F;

        if (data.size() < len) {

            LOGW("string length exceeds buffer: expected length = {}, buffer size = {}",
                len, data.size());
            return sol::nil;
        }

        const sol::string_view str { data.data(), len };
        data.remove_prefix(len);
        return sol::make_object(state, str);
    }

    inline sol::object string(std::string_view& data, const sol::state& state)
    {
        if (data.empty()) {
            throw std::runtime_error("decode::string: empty buffer");
        }

        const std::uint8_t type = data[0];
        data.remove_prefix(1);

        std::size_t len = 0;
        switch (type) {
        case detail::type::STR8:  len = detail::read_bytes<uint8_t>(data); break;
        case detail::type::STR16: len = detail::read_bytes<uint16_t>(data); break;
        case detail::type::STR32: len = detail::read_bytes<uint32_t>(data); break;
        default:
            LOGW("invalid string type: {}", type);
            return sol::nil;
        }

        if (data.size() < len) {
            LOGW("string length exceeds buffer:"
                    "expected length = {}, buffer size = {}",
                len, data.size());
            return sol::nil;
        }

        const sol::string_view str { data.data(), len };
        data.remove_prefix(len);
        return sol::make_object(state, str);
    }

    inline sol::table map(std::string_view& data, const sol::state& state)
    {
        if (data.empty()) {
            throw std::runtime_error("decode::map: empty buffer");
        }

        const uint8_t header = data[0];
        data.remove_prefix(1);

        size_t map_size = 0;
        if ((header & 0xF0) == static_cast<uint8_t>(detail::type::FIXMAP)) {
            map_size = header & 0x0F;
        } else if (static_cast<detail::type>(header) == detail::type::MAP16) {
            map_size = detail::read_bytes<uint16_t>(data);
        } else if (static_cast<detail::type>(header) == detail::type::MAP32) {
            map_size = detail::read_bytes<uint32_t>(data);
        } else {
            LOGW("invalid header ({:02X})", header);
            return sol::nil;
        }

        sol::table tbl = state.create_table(state);
        for (size_t i = 0; i < map_size; ++i) {
            if (data.empty()) {
                LOGW("unexpected EOF (key {})", i);
                break;
            }
            sol::object key = detail::decode_value(data, state);

            if (data.empty()) {
                LOGW("unexpected EOF (value {})", i);
                break;
            }
            sol::object val = detail::decode_value(data, state);

            tbl[key] = val;
        }

        return tbl;
    }

    inline sol::table array(std::string_view& data, const sol::state& state)
    {
        if (data.empty()) {
            throw std::runtime_error("decode::array: empty buffer");
        }

        const uint8_t header = data[0];
        data.remove_prefix(1);

        size_t array_size = 0;
        if ((header & 0xF0) == static_cast<uint8_t>(detail::type::FIXARRAY)) {
            array_size = header & 0x0F;
        } else if (static_cast<detail::type>(header) == detail::type::ARRAY16) {
            array_size = detail::read_bytes<uint16_t>(data);
        } else if (static_cast<detail::type>(header) == detail::type::ARRAY32) {
            array_size = detail::read_bytes<uint32_t>(data);
        } else {
            LOGW("invalid header ({:02X})", header);
            return sol::nil;
        }

        sol::table tbl = state.create_table(state);
        for (size_t i = 0; i < array_size; ++i) {
            if (data.empty()) {
                LOGW("unexpected EOF (element {})", i);
                tbl[i + 1] = sol::nil;
                continue;
            }
            sol::object elem = detail::decode_value(data, state);
            tbl[i + 1] = elem;
        }

        return tbl;
    }
}

namespace detail
{
    inline sol::object decode_value(std::string_view& data, const sol::state& state)
    {
        if (data.empty()) {
            LOGW("empty buffer");
            return sol::nil;
        }

        const uint8_t type_byte = data[0];

        using detail::type;

        if ((type_byte & 0x80) == 0 || (type_byte & 0xE0) == 0xE0) {
            return decode::fix_integer(data, state);
        }

        if ((type_byte & 0xE0) == FIXSTR) {
            return decode::fixstring(data, state);
        } 

        if ((type_byte & 0xF0) == FIXARRAY || type_byte == ARRAY16 || type_byte == ARRAY32) {
            return decode::array(data, state);
        }

        if ((type_byte & 0xF0) == FIXMAP || type_byte == MAP16 || type_byte == MAP32) {
            return decode::map(data, state);
        }

        switch (type_byte) {
            case UINT8: case UINT16: case UINT32: case UINT64:
            case INT8:  case INT16:  case INT32:  case INT64:
            return decode::integer(data, state);
        case FLOAT: case DOUBLE:
            return decode::floating(data, state);
        case TTURE: case TFALSE:
            return decode::boolean(data, state);
        case NIL:
            return decode::nil(data);
        case STR8: case STR16: case STR32:
            return decode::string(data, state);
        default:
            LOGW("unknown type ({:02X})", type_byte);
            data.remove_prefix(1);
            return sol::nil;
        }
    }
}

inline std::vector<sol::object> unpack(std::string_view data, const sol::state& state)
{
    std::vector<sol::object> result;
    result.reserve(1);

    while (!data.empty()) {
        result.emplace_back(detail::decode_value(data, state));
    }

    return result;
}
}
