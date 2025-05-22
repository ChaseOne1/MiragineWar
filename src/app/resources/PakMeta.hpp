#pragma once
#include "GUID.hpp"
#include <cstddef>
#include <cstdint>

namespace app {
struct PakHeader
{
    using ResNum_t = std::size_t;

    static constexpr std::uint32_t msc_nMagic = 82413u;
    static constexpr std::uint32_t msc_nVersion = 1;

    std::uint32_t m_nMagic = msc_nMagic;
    std::uint32_t m_nVersion = msc_nVersion;
    ResNum_t m_nResEntryNum;

    PakHeader() = default;

    PakHeader(ResNum_t res_num)
        : m_nResEntryNum(res_num)
    { }

    PakHeader(std::uint32_t magic, std::uint32_t version, ResNum_t res_num)
        : m_nMagic(magic)
        , m_nVersion(version)
        , m_nResEntryNum(res_num)
    { }
};


struct PakIndexEntry
{
    using Size_t = std::size_t;
    using Offset_t = std::size_t;

    GUID_t m_nGuid;
    GUID_t m_nPakGuid;
    Size_t m_nSize;
    Offset_t m_nOffset;
    ResType m_eType;
};
}
