#include "MouseInteractive.hpp"
#include "app/Renderer.hpp"
#include "utility/Registry.hpp"
#include "app/System/Input.hpp"
#include "app/EventBus.hpp"
#include "game/Component/Clickable.hpp"
#include "game/Component/Pointable.hpp"
#include "app/Component/Render/ZIndex.hpp"

using namespace game::sys;
using namespace entt;

MouseInteractive::MouseInteractive()
    : m_Clickables(std::make_unique<InteractiveContainer>(app::Renderer::GetInstance().GetRenderSize().x / 2.f,
          app::Renderer::GetInstance().GetRenderSize().y / 2.f,
          app::Renderer::GetInstance().GetRenderSize().x / 2.f,
          app::Renderer::GetInstance().GetRenderSize().y / 2.f))
    , m_Pointables(std::make_unique<InteractiveContainer>(app::Renderer::GetInstance().GetRenderSize().x / 2.f,
          app::Renderer::GetInstance().GetRenderSize().y / 2.f,
          app::Renderer::GetInstance().GetRenderSize().x / 2.f,
          app::Renderer::GetInstance().GetRenderSize().y / 2.f))
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();
    reg.on_construct<comp::Clickable>().connect<&MouseInteractive::OnClickableConstruct>(this);
    reg.on_update<comp::Clickable>().connect<&MouseInteractive::OnClickableUpdate>(this);
    reg.on_destroy<comp::Clickable>().connect<&MouseInteractive::OnClickableDestroy>(this);

    reg.on_construct<comp::Pointable>().connect<&MouseInteractive::OnPointableConstruct>(this);
    reg.on_update<comp::Pointable>().connect<&MouseInteractive::OnPointableUpdate>(this);
    reg.on_destroy<comp::Pointable>().connect<&MouseInteractive::OnPointableDestroy>(this);

    auto& input = app::sys::Input::GetInstance();
    input.Subsrcibe(app::Key::MOUSE_SELECT, std::bind(&MouseInteractive::OnClick, this, std::placeholders::_1));
    app::EventBus::GetInstance().Subscribe(SDL_EVENT_MOUSE_MOTION, std::bind(&MouseInteractive::OnMotion, this, std::placeholders::_1));
    app::EventBus::GetInstance().Subscribe(SDL_EVENT_WINDOW_RESIZED, std::bind(&MouseInteractive::OnWindowResize, this, std::placeholders::_1));
}

void MouseInteractive::OnClick(const SDL_Event* event)
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();

    auto entities = m_Clickables->query(event->button.x, event->button.y);
    if (entities.empty()) return;

    std::sort(entities.begin(), entities.end(),
        [](const entity& lhs, const entity& rhs) { return utility::Registry::GetInstance().GetRegistry().get<app::comp::ZIndex>(lhs).m_Index
                                                       > utility::Registry::GetInstance().GetRegistry().get<app::comp::ZIndex>(rhs).m_Index; });

    if (event->button.down) {
        if (auto& callback = reg.get<game::comp::Clickable>(entities.front()).m_ClickInCallback)
            callback();
    } else {
        if (auto& callback = reg.get<game::comp::Clickable>(entities.front()).m_ClickOutCallback)
            callback();
    }
}

void MouseInteractive::OnMotion(const SDL_Event* event)
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();

    auto entities = m_Pointables->query(event->motion.x, event->motion.y);
    if (entities.empty()) {
        if (auto* pointable = reg.try_get<game::comp::Pointable>(m_Pointed); pointable && pointable->m_PointOutCallback)
            pointable->m_PointOutCallback();
        m_Pointed = entt::null;
        return;
    }

    std::sort(entities.begin(), entities.end(),
        [](const entity& lhs, const entity& rhs) { return utility::Registry::GetInstance().GetRegistry().get<app::comp::ZIndex>(lhs).m_Index
                                                       > utility::Registry::GetInstance().GetRegistry().get<app::comp::ZIndex>(rhs).m_Index; });

    if (m_Pointed != entities.front()) {
        if (auto* pointable = reg.try_get<game::comp::Pointable>(m_Pointed); pointable && pointable->m_PointOutCallback)
            pointable->m_PointOutCallback();
        m_Pointed = entities.front();
        if (auto& callback = reg.get<game::comp::Pointable>(m_Pointed).m_PointInCallback; callback)
            callback();
    }
}

void MouseInteractive::OnWindowResize(const SDL_Event*)
{
    m_Clickables = std::make_unique<InteractiveContainer>(app::Renderer::GetInstance().GetRenderSize().x / 2.f,
        app::Renderer::GetInstance().GetRenderSize().y / 2.f,
        app::Renderer::GetInstance().GetRenderSize().x / 2.f,
        app::Renderer::GetInstance().GetRenderSize().y / 2.f);

    m_Pointables = std::make_unique<InteractiveContainer>(app::Renderer::GetInstance().GetRenderSize().x / 2.f,
        app::Renderer::GetInstance().GetRenderSize().y / 2.f,
        app::Renderer::GetInstance().GetRenderSize().x / 2.f,
        app::Renderer::GetInstance().GetRenderSize().y / 2.f);
}

void MouseInteractive::OnClickableConstruct(entt::registry& registry, entt::entity entity)
{
    assert(registry.all_of<app::comp::ZIndex>(entity));
    const auto& rect = registry.get<game::comp::Clickable>(entity).m_Area;
    m_Clickables->insert(entity, rect.cx, rect.cy, rect.hw, rect.hh);
}

void MouseInteractive::OnClickableUpdate(entt::registry& registry, entt::entity entity)
{
    OnClickableDestroy(registry, entity);
    OnClickableConstruct(registry, entity);
}

void MouseInteractive::OnClickableDestroy(entt::registry& registry, entt::entity entity)
{
    m_Clickables->remove(entity);
}

void MouseInteractive::OnPointableUpdate(entt::registry& registry, entt::entity entity)
{
    OnPointableDestroy(registry, entity);
    OnPointableConstruct(registry, entity);
}

void MouseInteractive::OnPointableConstruct(entt::registry& registry, entt::entity entity)
{
    assert(registry.all_of<app::comp::ZIndex>(entity));
    const auto& rect = registry.get<game::comp::Pointable>(entity).m_Area;
    m_Pointables->insert(entity, rect.cx, rect.cy, rect.hw, rect.hh);
}

void MouseInteractive::OnPointableDestroy(entt::registry& registry, entt::entity entity)
{
    m_Pointables->remove(entity);
}
