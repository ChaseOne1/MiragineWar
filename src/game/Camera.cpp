#include "Camera.hpp"
#include "game/Component/Position.hpp"
#include "game/Component/Movement.hpp"
#include "app/System/Input.hpp"

using namespace game;
using namespace entt;

Camera::Camera()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();

    // TODO: a more suitable place
    reg.emplace<game::comp::Position>(m_Camera, 0.f, 100.f);
    reg.emplace<game::comp::Movement>(m_Camera, 0.f, 0.f, 0.f, 0.f);

    app::sys::Input::GetInstance().Subsrcibe(app::Key::CAM_MOVE_LEFT, [this]() {
        auto& input = app::sys::Input::GetInstance();
        registry& reg = utility::Registry::GetInstance().GetRegistry();
        if (input.IsPressed(app::Key::CAM_MOVE_LEFT)) {
            reg.patch<game::comp::Movement>(m_Camera, [](game::comp::Movement& movement) { movement.m_Velocity.x = -msc_Velocity.x; });

            const auto& posn = reg.get<game::comp::Position>(m_Camera);
            SDL_Log("posn: x=%.2f y=%.2f\n", posn.m_Position.x, posn.m_Position.y);
        } else {
            reg.patch<game::comp::Movement>(m_Camera, [](game::comp::Movement& movement) { movement.m_Velocity.x = 0.f; });
        }
    });

    app::sys::Input::GetInstance().Subsrcibe(app::Key::CAM_MOVE_RIGHT, [this]() {
        auto& input = app::sys::Input::GetInstance();
        registry& reg = utility::Registry::GetInstance().GetRegistry();
        if (input.IsPressed(app::Key::CAM_MOVE_RIGHT)) {
            reg.patch<game::comp::Movement>(m_Camera, [](game::comp::Movement& movement) { movement.m_Velocity.x = msc_Velocity.x; });
            const auto& posn = reg.get<game::comp::Position>(m_Camera);
            SDL_Log("posn: x=%.2f y=%.2f\n", posn.m_Position.x, posn.m_Position.y);
        } else {
            reg.patch<game::comp::Movement>(m_Camera, [](game::comp::Movement& movement) { movement.m_Velocity.x = 0.f; });
        }
    });
}
