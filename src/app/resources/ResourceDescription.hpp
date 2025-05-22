#pragma once
#include <cstdint>
#include "GUID.hpp"

namespace app {
enum struct ResourceType : std::uint8_t
{
    PNG,
    JPG,
    TTF,
    MP3,
    WAV,
    BIN,
};
using ResType = ResourceType;

struct ResourceDescription
{
public:
    GUID_t m_Guid, m_FromPak;
    std::size_t m_nPakOffset;
    std::size_t m_nSize;
    ResourceType m_eType;

public:
    operator GUID_t() const { return m_Guid; }
};
using ResDesc = ResourceDescription;
}
