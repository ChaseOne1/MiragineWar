#pragma once

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off

srefl_class(mathfu::vec2,
    ctors(
        ctor(float, float)
    )
    fields(
        field(&mathfu::vec2::x),
        field(&mathfu::vec2::y)
    )
)

srefl_class(SDL_FPoint,
    ctors(
        ctor(float, float)
    )
    fields(
        field(&SDL_FPoint::x),
        field(&SDL_FPoint::y)
    )
)

srefl_class(SDL_FRect,
    ctors(
        ctor(float, float, float, float)
    )
    fields(
        field(&SDL_FRect::x),
        field(&SDL_FRect::y),
        field(&SDL_FRect::w),
        field(&SDL_FRect::h)
    )
)

// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
