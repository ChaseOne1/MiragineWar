#pragma once
#include "utility/LooseQuadTree.hpp"
#include "utility/RTree.hpp"

namespace app::sys {
class Clickable : public utility::Singleton<Clickable>
{
    friend class utility::Singleton<Clickable>;

private:
    utility::LooseQuadTree<entt::entity> m_Clickables;
    // utility::RTree<entt::entity> m_Clickables;
    entt::entity* m_Clicked = nullptr;

private:
    Clickable();
    ~Clickable() = default;

    void AddClickable(entt::registry& reg, entt::entity entity);
    void RemoveClickable(entt::registry& reg, entt::entity entity);

public:
    void Tick();
    entt::entity* GetClicked() noexcept { return m_Clicked; }
};
}
