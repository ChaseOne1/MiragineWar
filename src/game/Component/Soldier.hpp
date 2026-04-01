#pragma once

namespace game::comp {
struct Soldier
{ };
}

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off
srefl_class(game::comp::Soldier)
// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
