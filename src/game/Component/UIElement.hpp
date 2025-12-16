#pragma once

namespace game::comp {
struct UIElement
{};
}

#include "mirrow/srefl/srefl_begin.hpp"
srefl_class(game::comp::UIElement,
    ctors(
        ctor()
    )
 )
#include "mirrow/srefl/srefl_end.hpp"
