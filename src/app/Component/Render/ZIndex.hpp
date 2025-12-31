#pragma once

namespace app::comp {
enum class ZIndexVal : uint16_t
{
    ZINDEX_GROUND = 0u,

    ZINDEX_SOLDIER = 512u,

    ZINDEX_UIELEMENT = 1024u,
};

struct ZIndex
{
    std::underlying_type_t<ZIndexVal> m_Index;

    explicit ZIndex(ZIndexVal i) : m_Index(static_cast<decltype(m_Index)>(i)) {}
    explicit ZIndex(std::underlying_type_t<ZIndexVal> i) : m_Index(i) {}
};
}

#include "mirrow/srefl/srefl_begin.hpp"
srefl_class(app::comp::ZIndex,
    ctors(
        ctor(std::underlying_type_t<app::comp::ZIndexVal>)
    )
    fields(
        field(&app::comp::ZIndex::m_Index)
    )
)
#include "mirrow/srefl/srefl_end.hpp"