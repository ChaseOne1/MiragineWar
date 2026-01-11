#pragma once
#include "app/ScriptComponent.hpp"

namespace app::comp {
enum class ZIndexVal : uint16_t
{
    ZINDEX_GROUND = 0u,

    ZINDEX_SOLDIER = 512u,

    ZINDEX_UIELEMENT = 1024u,
};

struct ZIndex : app::ScriptComponent<ZIndex>
{
    float m_Index;

    explicit ZIndex(ZIndexVal i) : m_Index(static_cast<decltype(m_Index)>(i)) {}
    explicit ZIndex(float i) : m_Index(i) {}
};
}

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off
srefl_class(app::comp::ZIndex,
    ctors(
        ctor(float)
    )
    fields(
        field(&app::comp::ZIndex::m_Index)
    )
)
// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
