#pragma once
#include "utility/LooseQuadTree.hpp"

namespace game::sys {
class Collide : public utility::Singleton<Collide>
{
    friend class utility::Singleton<Collide>;

private:
    utility::LooseQuadTree<entt::entity> m_Tree;

private:
    Collide();
    ~Collide() = default;

    void AddToTree(entt::registry&, entt::entity);
    void UpdateInTree(entt::registry&, entt::entity);

public:
    void Tick();
};
}
