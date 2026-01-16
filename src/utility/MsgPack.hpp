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
        ARRAY16 = 0xDC, ARRAY32 = 0xDD,
        MAP16 = 0xDE, MAP32 = 0XDF,
        FIXSTR = 0xA0, STR8 = 0xD9, STR16 = 0xDA, STR32 = 0xDB,
    };

    template <typename T>
    inline void write_bytes(std::string& out, const T& value)
    {
        static_assert(std::is_arithmetic_v<T>, "T must be arithmetic");
        const std::uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
        if constexpr (endian::native == endian::little) {
            for (int i = sizeof(T) - 1; i >= 0; --i) {
                out.push_back(ptr[i]);
            }
        } else {
            for (size_t i = 0; i < sizeof(T); ++i) {
                out.push_back(ptr[i]);
            }
        }
    }

    template <typename T>
    inline T read_bytes(std::string_view& data)
    {
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
}

namespace encode {
    inline void integer(std::string& data, std::int64_t value)
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

    inline void floating(std::string& data, double value)
    {
        if(value == static_cast<float>(value)){
            data.push_back(detail::type::FLOAT);
            detail::write_bytes(data, static_cast<float>(value));
        }else{
            data.push_back(detail::type::DOUBLE);
            detail::write_bytes(data, static_cast<double>(value));
        }
    }

    inline void boolean(std::string& data, bool value)
    {
        if(value) data.push_back(detail::type::TTURE);
        else data.push_back(detail::type::TFALSE);
    }

    inline void nil(std::string& data)
    {
        data.push_back(detail::type::NIL);
    }

    inline void string(std::string& data, std::string_view sv)
    {
        if (sv.size() < 32) {
            data.push_back(detail::type::FIXSTR | (sv.size() & 0xFF));
        }else if(sv.size() <= 0xFF){
            data.push_back(detail::type::STR8);
            data.push_back(sv.size());
        }else if(sv.size() <= 0xFFFF){
            data.push_back(detail::type::STR16);
            detail::write_bytes(data, static_cast<std::uint16_t>(sv.size()));
        }else{
            data.push_back(detail::type::STR32);
            detail::write_bytes(data, static_cast<std::uint32_t>(sv.size()));
        }
        data.insert(data.end(), sv.data(), sv.data() + sv.size());
    }

}

inline void pack(std::string& data, const sol::variadic_args& args)
{
    for (const auto& value : args) {
        switch (value.get_type()) {
        case sol::type::boolean:
            encode::boolean(data, value.as<bool>());
            break;
        case sol::type::number:
            if (value.is<lua_Integer>()) encode::integer(data, value.as<int64_t>());
            else encode::floating(data, value.as<double>());
            break;
        case sol::type::lua_nil:
            encode::nil(data);
            break;
        case sol::type::string:
            encode::string(data, value.as<std::string_view>());
            break;
        default:
            SDL_Log("utility::pack: cannot handle the type: %d", value.get_type());
            break;
        }
    }
}

inline std::string pack(const sol::variadic_args& args)
{
    std::string data;
    data.reserve(args.size() * 8); // just an estimate

    pack(data, args);

    return data;
}

namespace decode {
    inline lua_Integer fix_integer(std::string_view& data)
    {
        const uint8_t byte = data[0];
        data.remove_prefix(1);
        return byte;
    }

    inline sol::object fix_integer(std::string_view& data, const sol::state& state)
    {
        const uint8_t byte = data[0];
        data.remove_prefix(1);
        return sol::make_object(state, static_cast<lua_Integer>(byte));
    }

    inline lua_Integer integer(std::string_view& data)
    {
        const uint8_t type = data[0];
        data.remove_prefix(1);

        switch (type) {
        case detail::UINT8:
            return detail::read_bytes<uint8_t>(data);
        case detail::UINT16:
            return detail::read_bytes<uint16_t>(data);
        case detail::UINT32:
            return detail::read_bytes<uint32_t>(data);
        case detail::UINT64:
            return detail::read_bytes<uint64_t>(data);
        case detail::INT8:
            return detail::read_bytes<int8_t>(data);
        case detail::INT16:
            return detail::read_bytes<int16_t>(data);
        case detail::INT32:
            return detail::read_bytes<int32_t>(data);
        case detail::INT64:
            return detail::read_bytes<int64_t>(data);
        default:
            SDL_Log("utility::decode::integer: invalid integer type: %d", type);
            return 0;
        }
    }

    inline sol::object integer(std::string_view& data, const sol::state& state)
    {
        const uint8_t type = data[0];
        data.remove_prefix(1);

        switch (type) {
        case detail::UINT8:
            return sol::make_object(state, detail::read_bytes<uint8_t>(data));
        case detail::UINT16:
            return sol::make_object(state, detail::read_bytes<uint16_t>(data));
        case detail::UINT32:
            return sol::make_object(state, detail::read_bytes<uint32_t>(data));
        case detail::UINT64:
            return sol::make_object(state, detail::read_bytes<uint64_t>(data));
        case detail::INT8:
            return sol::make_object(state, detail::read_bytes<int8_t>(data));
        case detail::INT16:
            return sol::make_object(state, detail::read_bytes<int16_t>(data));
        case detail::INT32:
            return sol::make_object(state, detail::read_bytes<int32_t>(data));
        case detail::INT64:
            return sol::make_object(state, detail::read_bytes<int64_t>(data));
        default:
            SDL_Log("utility::decode::integer: invalid integer type: %d", type);
            return sol::nil;
        }
    }

    inline lua_Number floating(std::string_view& data)
    {
        const std::uint8_t type = data[0];
        data.remove_prefix(1);
        if (type == detail::type::FLOAT) return detail::read_bytes<float>(data);
        if (type == detail::type::DOUBLE) return detail::read_bytes<double>(data);
        SDL_Log("utility::decode::floating: invalid floating type: %d", type);
        return 0.0;
    }

    inline sol::object floating(std::string_view& data, const sol::state& state)
    {
        const std::uint8_t type = data[0];
        data.remove_prefix(1);
        if (type == detail::type::FLOAT) return sol::make_object(state, detail::read_bytes<float>(data));
        if (type == detail::type::DOUBLE) return sol::make_object(state, detail::read_bytes<double>(data));
        SDL_Log("utility::decode::floating: invalid floating type: %d", type);
        return sol::nil;
    }

    inline bool boolean(std::string_view& data)
    {
        const std::uint8_t type = data[0];
        data.remove_prefix(1);

        if (type == detail::type::TTURE) return true;
        if (type == detail::type::TFALSE) return false;

        SDL_Log("utility::decode::boolean: invalid boolean type: %d", type);
        return false;
    }

    inline sol::object boolean(std::string_view& data, const sol::state& state)
    {
        const std::uint8_t type = data[0];
        data.remove_prefix(1);

        if (type == detail::type::TTURE) return sol::make_object(state, true);
        if (type == detail::type::TFALSE) return sol::make_object(state, false);

        SDL_Log("utility::decode::boolean: invalid boolean type: %d", type);
        return sol::nil;
    }

    inline sol::object nil(std::string_view& data)
    {
        data.remove_prefix(1);
        return sol::nil;
    }

    inline sol::string_view fix_string(std::string_view& data)
    {
        const std::size_t len = data[0] & 0x1F;
        data.remove_prefix(1);

        if (data.size() < len) {
            SDL_Log("utility::decode::fix_string: string length exceeds buffer:"
                    "expected length = %zu, buffer size = %zu",
                len, data.size());
            return {};
        }

        const std::string_view str { data.data(), len };
        data.remove_prefix(len);
        return str;
    }

    inline sol::object fix_string(std::string_view& data, const sol::state& state)
    {
        const std::size_t len = data[0] & 0x1F;
        data.remove_prefix(1);

        if (data.size() < len) {
            SDL_Log("utility::decode::fix_string: string length exceeds buffer:"
                    "expected length = %zu, buffer size = %zu",
                len, data.size());
            return sol::nil;
        }

        const std::string_view str { data.data(), len };
        data.remove_prefix(len);
        return sol::make_object(state, str);
    }

    inline std::string_view string(std::string_view& data)
    {
        const std::uint8_t type = data[0];
        data.remove_prefix(1);

        std::size_t len = 0;
        if (type == detail::type::STR8) len = detail::read_bytes<uint8_t>(data);
        else if (type == detail::type::STR16) len = detail::read_bytes<uint16_t>(data);
        else if (type == detail::type::STR32) len = detail::read_bytes<uint32_t>(data);
        else {
            SDL_Log("utility::decode::string: invalid string type");
            return {};
        }

        if (data.size() < len) {
            SDL_Log("utility::decode::fix_string: string length exceeds buffer:"
                    "expected length = %zu, buffer size = %zu",
                len, data.size());
            return {};
        }

        const std::string_view str { data.data(), len };
        data.remove_prefix(len);
        return str;
    }

    inline sol::object string(std::string_view& data, const sol::state& state)
    {
        const std::uint8_t type = data[0];
        data.remove_prefix(1);

        std::size_t len = 0;
        if (type == detail::type::STR8) len = detail::read_bytes<uint8_t>(data);
        else if (type == detail::type::STR16) len = detail::read_bytes<uint16_t>(data);
        else if (type == detail::type::STR32) len = detail::read_bytes<uint32_t>(data);
        else {
            SDL_Log("utility::decode::string: invalid string type");
            return sol::nil;
        }

        if (data.size() < len) {
            SDL_Log("utility::decode::fix_string: string length exceeds buffer:"
                    "expected length = %zu, buffer size = %zu",
                len, data.size());
            return sol::nil;
        }

        const std::string_view str { data.data(), len };
        data.remove_prefix(len);
        return sol::make_object(state, str);
    }
}

inline std::vector<sol::object> unpack(std::string_view data, const sol::state& state)
{
    std::vector<sol::object> result;
    result.reserve(1);

    while (!data.empty()) {
        switch (static_cast<uint8_t>(data[0])) {
        case detail::type::UINT8: case detail::type::UINT16:
        case detail::type::UINT32: case detail::type::UINT64:
        case detail::type::INT8: case detail::type::INT16:
        case detail::type::INT32: case detail::type::INT64:
            result.emplace_back(decode::integer(data, state));
            break;
        case detail::type::FLOAT: case detail::type::DOUBLE:
            result.emplace_back(decode::floating(data, state));
            break;
        case detail::TTURE: case detail::type::TFALSE:
            result.emplace_back(decode::boolean(data, state));
            break;
        case detail::type::NIL:
            result.emplace_back(decode::nil(data));
            break;
        case detail::type::STR8: case detail::type::STR16:
        case detail::type::STR32:
            result.emplace_back(decode::string(data, state));
            break;
        default:
            if ((data[0] & 0x80) == detail::type::PFIXINT || (data[0] & 0xE0) == detail::type::NFIXINT) {
                result.emplace_back(decode::fix_integer(data, state));
            } else if ((data[0] & 0xE0) == detail::type::FIXSTR) {
                result.emplace_back(decode::fix_string(data, state));
            } else {
                SDL_Log("unknown type code: %d", data[0]);
                data.remove_prefix(1);
            }
            break;
        }
    }

    return result;
}
}
