#include "World.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/Collision.hpp"

using namespace game;
using namespace entt;
using namespace mathfu;

World::World()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    //  we plan to manage the object that both has position and collision
    reg.on_construct<game::comp::Collision>().connect<&World::OnCollisionConstruct>(this);
    // TODO: for movalbe object, add update listener
}
void World::OnCollisionConstruct(entt::registry& registry, entt::entity entity)
{
    const auto [tfm, coll] = registry.try_get<game::comp::Transform, game::comp::Collision>(entity);
    // this listener is only called when Collision component is constructed,
    // so there is no need to check the Position
    if (!tfm) return;

    const vec4 collbox = coll->Transform2World(tfm->m_Position.xy());
    m_LTGrid.Insert(entity, collbox.x, collbox.y, collbox.z, collbox.w);
}

std::vector<entity> World::SearchVisibleObjects(float l, float t, float r, float b) const
{
    return m_LTGrid.Serach(l, t, r, b);
}
