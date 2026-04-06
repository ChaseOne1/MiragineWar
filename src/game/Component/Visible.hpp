#pragma once

namespace game::comp {
struct Visible
{ };
}

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off
srefl_class(game::comp::Visible,
    ctors(
        ctor()
    )
)
// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
