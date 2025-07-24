#pragma once
#include "utility/LTGrid.hpp"
#include "game/Component/Transform.hpp"
#include "game/World.hpp"

namespace game::sys {
class Visible : public utility::Singleton<Visible>
{
    friend class utility::Singleton<Visible>;

private:
    utility::LTGrid<entt::entity> m_LTGrid { game::World::msc_fWidth, game::World::msc_fHeight,
        game::World::msc_fLooseWidth, game::World::msc_fLooseHeight, game::World::msc_fTightWidth, game::World::msc_fTightHeight };

private:
    Visible();
    ~Visible() = default;

    void OnTransformConstruct(entt::registry&, entt::entity);
    void OnTransformDestroy(entt::registry&, entt::entity);

public:
    void Tick();
    void CleanUp();
    //TODO: maybe there is a better way than calling manually
    void OnTransformUpdate(entt::entity ent, const game::comp::Transform& last, const comp::Transform& now);
};
}
