#pragma once

namespace app::comp {
struct TextureRotation
{
    float m_fAngle = 0.f;
    std::optional<SDL_FPoint> m_RotationPivot;
};
}

#include "mirrow/srefl/srefl_begin.hpp"
// clang-format off
srefl_class(app::comp::TextureRotation,
    fields(
        field(&app::comp::TextureRotation::m_fAngle),
        field(&app::comp::TextureRotation::m_RotationPivot)
    )
)
// clang-format on
#include "mirrow/srefl/srefl_end.hpp"
