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
        // lambda capture structured binding is C++20 extension,
        // so we can't use it here
        game::comp::Movement& movement = view.get<game::comp::Movement>(entity);
        game::comp::Position& position = view.get<game::comp::Position>(entity);

        const auto& time = app::sys::Time::GetInstance();
        const float time_square = time.GetDeltaTimeInSeconds() * time.GetDeltaTimeInSeconds();

        // update position
        // in order to notify the collision system but not introduce a new way
        reg.patch<game::comp::Position>(entity, [&](game::comp::Position& position) {
            position.m_Position.x += movement.m_Velocity.x * time.GetDeltaTimeInSeconds() + 0.5f * movement.m_Acceleration.x * time_square;
            position.m_Position.y += movement.m_Velocity.y * time.GetDeltaTimeInSeconds() + 0.5f * movement.m_Acceleration.y * time_square;
        });

        // update velocity
        reg.patch<game::comp::Movement>(entity, [&](game::comp::Movement& movement) {
            movement.m_Velocity.x += movement.m_Acceleration.x * time.GetDeltaTimeInSeconds();
            movement.m_Velocity.y += movement.m_Acceleration.y * time.GetDeltaTimeInSeconds();
        });
    }
}
