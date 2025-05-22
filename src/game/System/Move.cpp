#include "Move.hpp"
#include "game/Component/Movement.hpp"
#include "game/Component/Position.hpp"
#include "app/System/Time.hpp"

using namespace game::sys;
using namespace entt;
using namespace utility;

void Move::Tick()
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();
    auto view = reg.view<game::comp::Movement, game::comp::Position>();

    for (auto entity : view) {
        // capture structured binding is C++20 extension
        game::comp::Movement& movement = view.get<game::comp::Movement>(entity);
        game::comp::Position& position = view.get<game::comp::Position>(entity);

        const auto& time = app::sys::Time::GetInstance();
        float time_square = time.GetDeltaTime() * time.GetDeltaTime();

        // update position
        // in order to notify the collision system but not introduce a new way
        reg.patch<game::comp::Position>(entity, [&](game::comp::Position& position) {
            position.m_Position.x += movement.m_Velocity.x * time.GetDeltaTime() + 0.5f * movement.m_Acceleration.x * time_square;
            position.m_Position.y += movement.m_Velocity.y * time.GetDeltaTime() + 0.5f * movement.m_Acceleration.y * time_square;
        });

        // update velocity
        reg.patch<game::comp::Movement>(entity, [&](game::comp::Movement& movement) {
            movement.m_Velocity.x += movement.m_Acceleration.x * time.GetDeltaTime();
            movement.m_Velocity.y += movement.m_Acceleration.y * time.GetDeltaTime();
        });
    }
}
