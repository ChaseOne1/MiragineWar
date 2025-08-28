#include "Move.hpp"
#include "game/Component/Movement.hpp"
#include "game/Component/Transform.hpp"
#include "app/System/Time.hpp"
#include "game/World.hpp"
#include "game/System/Visible.hpp"
#include "utility/Registry.hpp"

using namespace game::sys;
using namespace entt;
using namespace mathfu;

void Move::Tick()
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();
    auto view = reg.view<game::comp::Movement, game::comp::Transform>();
    for (auto entity : view) {
        // lambda capture structured binding is C++20 extension,
        // so we can't use it here
        game::comp::Movement& movement = view.get<game::comp::Movement>(entity);
        vec2& position = view.get<game::comp::Transform>(entity).m_Position;

        const auto& time = app::sys::Time::GetInstance();
        const float time_square = time.DeltaTime().count() / 1000.f * time.DeltaTime().count() / 1000.f;

        // update position
        // in order to notify the collision system but not introduce a new way
        reg.patch<game::comp::Transform>(entity, [&](game::comp::Transform& transform) {
            const vec2 last_posn = transform.m_Position;

            transform.m_Position.x += movement.m_Velocity.x * time.DeltaTime().count() / 1000.f + 0.5f * movement.m_Acceleration.x * time_square;
            transform.m_Position.y += movement.m_Velocity.y * time.DeltaTime().count() / 1000.f + 0.5f * movement.m_Acceleration.x * time_square;
            transform.m_Position.x = std::clamp(transform.m_Position.x, 0.f, World::msc_fWidth);
            transform.m_Position.y = std::clamp(transform.m_Position.y, 0.f, World::msc_fHeight);

            Visible::GetInstance().OnPositionUpdate(entity, last_posn);
        });

        // update velocity
        reg.patch<game::comp::Movement>(entity, [&](game::comp::Movement& movement) {
            movement.m_Velocity.x += movement.m_Acceleration.x * time.DeltaTime().count() / 1000.f;
            movement.m_Velocity.y += movement.m_Acceleration.y * time.DeltaTime().count() / 1000.f;
        });
    }
}
