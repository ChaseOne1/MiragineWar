#pragma once
#include "utility/LTGrid.hpp"
#include "game/World.hpp"

namespace game::sys {
class Visible : public utility::Singleton<Visible>
{
    friend class utility::Singleton<Visible>;

private:
    utility::LTGrid<entt::entity> m_WorldLTGrid { game::World::msc_fWidth, game::World::msc_fHeight,
        game::World::msc_fLooseWidth, game::World::msc_fLooseHeight, game::World::msc_fTightWidth, game::World::msc_fTightHeight };

private:
    Visible();
    ~Visible();

    void OnTransformConstruct(entt::registry&, entt::entity);
    void OnTransformUpdate(entt::registry&, entt::entity);
    void OnTransformDestroy(entt::registry&, entt::entity);

public:
    void Tick();
    void CleanUp();
    // NOTE: we use this funciton to move object cheaply
    // TODO: maybe there is a better way than calling manually
    void OnPositionUpdate(entt::entity ent, const mathfu::vec2& last);
    void OnSizeUpdate(entt::entity ent);
};
}
