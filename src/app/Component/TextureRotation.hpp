#pragma once

namespace app::comp {
struct TextureRotation
{
    float m_fAngle = 0.f;
    std::optional<SDL_FPoint> m_RotationPivot;
};
}
