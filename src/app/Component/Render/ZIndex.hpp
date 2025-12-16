#pragma once

namespace app::comp {
enum ZIndexVal : uint16_t
{
    ZINDEX_GROUND = 0u,

    ZINDEX_SOLDIER = 512u,

    ZINDEX_UIELEMENT = 1024u,
};

struct ZIndex
{
     std::underlying_type_t<ZIndexVal> m_Index;
};
}

#include "mirrow/srefl/srefl_begin.hpp"
srefl_class(app::comp::ZIndex,
    ctors(
        ctor(std::underlying_type_t<app::comp::ZIndexVal>)
    )
)
#include "mirrow/srefl/srefl_end.hpp"
