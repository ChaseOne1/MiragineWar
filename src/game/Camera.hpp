#pragma once
#include "utility/Registry.hpp"
#include "app/Settings.hpp"

namespace game {
class Camera : public utility::Singleton<Camera>
{
    friend class utility::Singleton<Camera>;

private:
    entt::entity m_Camera = utility::Registry::GetInstance().GetRegistry().create();

    inline static const mathfu::vec2 msc_Velocity { static_cast<float>(**app::Settings::GetInstance().GetSettings().at_path("Camera.speed_x").as_floating_point()),
        static_cast<float>(**app::Settings::GetInstance().GetSettings().at_path("Camera.speed_y").as_floating_point()) };

private:
    Camera();
    ~Camera() = default;

public:
    entt::entity GetCameraEntity() const noexcept { return m_Camera; }
    mathfu::mat3 GetCameraCenterTransformMatrix() const noexcept;
    mathfu::mat3 GetCameraLeftTopTransformMatrix() const noexcept;
};
}
