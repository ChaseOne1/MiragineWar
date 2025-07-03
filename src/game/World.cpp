#include "World.hpp"
#include "game/Component/Position.hpp"
#include "game/Component/Collision.hpp"

using namespace game;
using namespace entt;

World::World()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    //  we plan to manage the object that both has position and collision
    reg.on_construct<game::comp::Collision>().connect<&World::OnPosnConstruct>(this);
    // TODO: for movalbe object, add update listener
}
void World::OnPosnConstruct(entt::registry& registry, entt::entity entity)
{
    const auto [posn, coll] = registry.try_get<game::comp::Position, game::comp::Collision>(entity);
    // this listener is only called when Collision component is constructed,
    // so there is no need to check the Position
    if (!posn) return;

    m_LTGrid.Insert(entity, posn->m_Position.x + coll->m_CollisionBox.x, posn->m_Position.y + coll->m_CollisionBox.y,
        coll->m_CollisionBox.w, coll->m_CollisionBox.h);
}

std::vector<entity> World::SearchVisibleObjects(float l, float t, float r, float b) const
{
    return m_LTGrid.Serach(l, t, r, b);
}
