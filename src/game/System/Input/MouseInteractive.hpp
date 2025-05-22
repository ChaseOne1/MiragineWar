#pragma once
#include "utility/LooseQuadTree.hpp"

namespace game {
namespace sys {
    class MouseInteractive : public utility::Singleton<MouseInteractive>, public System
    {
        friend class utility::Singleton<MouseInteractive>;
        friend class Input;

    private:
        utility::LooseQuadTree<entt::entity> m_Interactives; // NOTE: maybe bug when two different interactive things overlap
                                                     // TODO: z-index
                                                     // static and dynamic interactive should sotre in different tree
        entt::entity m_Pointed;
        entt::entity m_Clicked;

    private:
        MouseInteractive();
        void Tick() override;
        void Add(entt::registry&, entt::entity);
        void Remove(entt::registry&, entt::entity);
        //TODO: Update

    public:
        void ClearClicked() noexcept { m_Clicked = entt::null; }
        entt::entity GetPointed() const noexcept { return m_Pointed; }
        entt::entity GetClicked() const noexcept { return m_Clicked; }
    };
}
}
