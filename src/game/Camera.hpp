#pragma once

namespace game {
class Camera : public utility::Singleton<Camera>
{
    friend class utility::Singleton<Camera>;

private:
    entt::entity m_Camera = utility::Registry::GetInstance().GetRegistry().create();
    mathfu::vec2 m_HalfFOV { 188.f, 128.f };//TODO: use the Transform:Size to instead fov

    inline static const mathfu::vec2 msc_Velocity { 100.f, 100.f };

private:
    Camera();
    ~Camera() = default;

public:
    entt::entity GetCameraEntity() noexcept { return m_Camera; }
    const mathfu::vec2& GetHalfFOV() const noexcept { return m_HalfFOV; }
    mathfu::mat3 GetCameraCenterTransformMatrix() const noexcept;
    mathfu::mat3 GetCameraLeftTopTransformMatrix() const noexcept;
};
}
