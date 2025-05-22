#pragma once

namespace app {
using GUID_t = std::size_t;

inline GUID_t GenerateGUID(std::string_view name)
{
    return std::hash<std::string_view>()(name);
}

struct GUIDHash
{
    constexpr inline std::size_t operator()(GUID_t val) const noexcept { return val; }
};
}
