#pragma once

#include "mirrow/srefl/srefl_begin.hpp"

srefl_class(mathfu::vec2,
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

#include "mirrow/srefl/srefl_end.hpp"