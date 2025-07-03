#pragma once
#include "utility/Math/Vector2.hpp"

namespace game {
class Camera : public utility::Singleton<Camera>
{
    friend class utility::Singleton<Camera>;

private:
    entt::entity m_Camera = utility::Registry::GetInstance().GetRegistry().create();
    utility::math::Vector2 m_HalfFOV { 50.f, 50.f };

    static constexpr utility::math::Vector2 msc_Velocity { 100.f, 25.f };

private:
    Camera();
    ~Camera() = default;

public:
    entt::entity GetCameraEntity() noexcept { return m_Camera; }
    const utility::math::Vector2& GetHalfFOV() const noexcept { return m_HalfFOV; }
};
}
