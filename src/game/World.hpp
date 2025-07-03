#pragma once
#include "utility/LTGrid.hpp"

namespace game {
class World : public utility::Singleton<World>
{
    friend class utility::Singleton<World>;

private:
    utility::LTGrid<entt::entity> m_LTGrid { msc_fWidth, msc_fHeight,
        msc_fLooseWidth, msc_fLooseHeight, msc_fTightWidth, msc_fTightHeight };

private:
    World();
    ~World() = default;

    void OnPosnConstruct(entt::registry&, entt::entity);

public:
    static constexpr float msc_fWidth = 1024.f, msc_fHeight = 256.f;
    static constexpr float msc_fLooseWidth = 64.f, msc_fLooseHeight = 8.f,
                           msc_fTightWidth = 32.f, msc_fTightHeight = 4.f;

public:
    std::vector<entt::entity> SearchVisibleObjects(float l, float t, float r, float b) const;
};
}
