#pragma once

namespace game::comp {
struct SoldierTag
{ };
}

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off
srefl_class(game::comp::SoldierTag)
// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
