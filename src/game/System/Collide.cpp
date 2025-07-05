#include "Collide.hpp"
#include "game/Component/Collision.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/Movement.hpp"

using namespace game::sys;
using namespace mathfu;

Collide::Collide()
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();
    reg.on_construct<game::comp::Collision>().connect<&Collide::AddToTree>(this);
    reg.on_update<game::comp::Movement>().connect<&Collide::UpdateInTree>(this);
}

void Collide::AddToTree(entt::registry& reg, entt::entity entity)
{
    game::comp::Collision& collision = reg.get<game::comp::Collision>(entity);
    vec2& position = reg.get<game::comp::Transform>(entity).m_Position;
    //m_Tree.Insert(entity, box);
}

void Collide::UpdateInTree(entt::registry&, entt::entity)
{
}

void Collide::Tick()
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();
    auto view = reg.view<game::comp::Transform, game::comp::Movement, game::comp::Collision>();

    for (auto entity : view) {
    }
}
