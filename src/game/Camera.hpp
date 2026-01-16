#pragma once
#include "utility/Registry.hpp"
#include "app/Settings.hpp"

namespace game {
class Camera : public utility::Singleton<Camera>
{
    friend class utility::Singleton<Camera>;

private:
    entt::entity m_Camera = utility::Registry::GetInstance().GetRegistry().create();

    const mathfu::vec2 mc_Velocity { app::Settings::GetSettings()["Camera"]["speed_x"], app::Settings::GetSettings()["Camera"]["speed_y"] };

private:
    Camera();
    ~Camera() = default;

public:
    entt::entity GetCameraEntity() const noexcept { return m_Camera; }
    mathfu::mat3 GetCameraCenterTransformMatrix() const noexcept;
    mathfu::mat3 GetCameraLeftTopTransformMatrix() const noexcept;
};
}
