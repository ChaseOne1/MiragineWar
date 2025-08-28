#pragma once
#include "utility/LooseQuadtree.hpp"

namespace game {
namespace sys {
    class MouseInteractive : public utility::Singleton<MouseInteractive>
    {
        friend class utility::Singleton<MouseInteractive>;

    private:
        using InteractiveContainer = utility::LooseQuadtree<entt::entity>;
        std::unique_ptr<InteractiveContainer> m_Clickables;
        std::unique_ptr<InteractiveContainer> m_Pointables;
        entt::entity m_Pointed;
        // entt::entity m_Clicked;

    private:
        MouseInteractive();

        void OnClick(const SDL_Event* event);
        void OnMotion(const SDL_Event* event);
        void OnWindowResize(const SDL_Event*);


        void OnClickableConstruct(entt::registry&, entt::entity);
        void OnClickableUpdate(entt::registry&, entt::entity);
        void OnClickableDestroy(entt::registry&, entt::entity);

        void OnPointableConstruct(entt::registry&, entt::entity);
        void OnPointableUpdate(entt::registry&, entt::entity);
        void OnPointableDestroy(entt::registry&, entt::entity);
        // TODO: Update

    public:
        // void ClearClicked() noexcept { m_Clicked = entt::null; }
        //  entt::entity GetPointed() const noexcept { return m_Pointed; }
        //  entt::entity GetClicked() const noexcept { return m_Clicked; }
    };
}
}
