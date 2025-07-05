#include "Camera.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/Movement.hpp"
#include "game/World.hpp"
#include "app/System/Input.hpp"

using namespace game;
using namespace entt;
using namespace mathfu;

Camera::Camera()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();

    reg.emplace<game::comp::Transform>(m_Camera, vec2 { World::msc_fWidth / 2.f, World::msc_fHeight / 2.f});
    reg.emplace<game::comp::Movement>(m_Camera, vec2 { 0.f, 0.f }, vec2 { 0.f, 0.f });

    app::sys::Input::GetInstance().Subsrcibe(app::Key::CAM_MOVE_UP, [this]() {
        auto& input = app::sys::Input::GetInstance();
        registry& reg = utility::Registry::GetInstance().GetRegistry();
        if (input.IsPressed(app::Key::CAM_MOVE_UP)) {
            reg.patch<game::comp::Movement>(m_Camera, [](game::comp::Movement& movement) { movement.m_Velocity.y = -msc_Velocity.y; });
        } else {
            reg.patch<game::comp::Movement>(m_Camera, [](game::comp::Movement& movement) { movement.m_Velocity.y = 0.f; });
        }
    });

    app::sys::Input::GetInstance().Subsrcibe(app::Key::CAM_MOVE_DOWN, [this]() {
        auto& input = app::sys::Input::GetInstance();
        registry& reg = utility::Registry::GetInstance().GetRegistry();
        if (input.IsPressed(app::Key::CAM_MOVE_DOWN)) {
            reg.patch<game::comp::Movement>(m_Camera, [](game::comp::Movement& movement) { movement.m_Velocity.y = msc_Velocity.y; });
        } else {
            reg.patch<game::comp::Movement>(m_Camera, [](game::comp::Movement& movement) { movement.m_Velocity.y = 0.f; });
        }
    });

    app::sys::Input::GetInstance().Subsrcibe(app::Key::CAM_MOVE_LEFT, [this]() {
        auto& input = app::sys::Input::GetInstance();
        registry& reg = utility::Registry::GetInstance().GetRegistry();
        if (input.IsPressed(app::Key::CAM_MOVE_LEFT)) {
            reg.patch<game::comp::Movement>(m_Camera, [](game::comp::Movement& movement) { movement.m_Velocity.x = -msc_Velocity.x; });
        } else {
            reg.patch<game::comp::Movement>(m_Camera, [](game::comp::Movement& movement) { movement.m_Velocity.x = 0.f; });
        }
    });

    app::sys::Input::GetInstance().Subsrcibe(app::Key::CAM_MOVE_RIGHT, [this]() {
        auto& input = app::sys::Input::GetInstance();
        registry& reg = utility::Registry::GetInstance().GetRegistry();
        if (input.IsPressed(app::Key::CAM_MOVE_RIGHT)) {
            reg.patch<game::comp::Movement>(m_Camera, [](game::comp::Movement& movement) { movement.m_Velocity.x = msc_Velocity.x; });
        } else {
            reg.patch<game::comp::Movement>(m_Camera, [](game::comp::Movement& movement) { movement.m_Velocity.x = 0.f; });
        }
    });
}

mat3 Camera::GetCameraCenterTransformMatrix() const noexcept
{
    return mat3::FromTranslationVector(
        -utility::Registry::GetInstance().GetRegistry().get<game::comp::Transform>(m_Camera).m_Position.xy());
}

mat3 Camera::GetCameraLeftTopTransformMatrix() const noexcept
{
    return mat3::FromTranslationVector(
        -(utility::Registry::GetInstance().GetRegistry().get<game::comp::Transform>(m_Camera).m_Position.xy() - m_HalfFOV));
}
