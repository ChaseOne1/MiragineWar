#include "MouseInteractive.hpp"
#include "app/Layout.hpp"
#include "app/Renderer.hpp"
#include "game/Component/UIElement.hpp"
#include "utility/Registry.hpp"
#include "app/System/Input.hpp"
#include "app/EventBus.hpp"
#include "game/Component/UIClickable.hpp"
#include "game/Component/UIPointable.hpp"
#include "app/Component/ZIndex.hpp"

using namespace game::sys;
using namespace entt;

MouseInteractive::MouseInteractive()
    : m_Clickables(std::make_unique<InteractiveContainer_t>(app::Renderer::GetInstance().GetRenderSize().x / 2.f,
          app::Renderer::GetInstance().GetRenderSize().y / 2.f,
          app::Renderer::GetInstance().GetRenderSize().x / 2.f,
          app::Renderer::GetInstance().GetRenderSize().y / 2.f))
    , m_Pointables(std::make_unique<InteractiveContainer_t>(app::Renderer::GetInstance().GetRenderSize().x / 2.f,
          app::Renderer::GetInstance().GetRenderSize().y / 2.f,
          app::Renderer::GetInstance().GetRenderSize().x / 2.f,
          app::Renderer::GetInstance().GetRenderSize().y / 2.f))
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();
    reg.on_construct<comp::UIClickable>().connect<&MouseInteractive::OnClickableConstruct>(this);
    reg.on_update<comp::UIClickable>().connect<&MouseInteractive::OnClickableUpdate>(this);
    reg.on_destroy<comp::UIClickable>().connect<&MouseInteractive::OnClickableDestroy>(this);

    reg.on_construct<comp::UIPointable>().connect<&MouseInteractive::OnPointableConstruct>(this);
    reg.on_update<comp::UIPointable>().connect<&MouseInteractive::OnPointableUpdate>(this);
    reg.on_destroy<comp::UIPointable>().connect<&MouseInteractive::OnPointableDestroy>(this);

    app::sys::Input::GetInstance().Subsrcibe(app::Key::MOUSE_SELECT, std::bind(&MouseInteractive::OnClick, this, std::placeholders::_1));
    app::EventBus::GetInstance().Subscribe(SDL_EVENT_MOUSE_MOTION, std::bind(&MouseInteractive::OnMotion, this, std::placeholders::_1));
    app::EventBus::GetInstance().Subscribe(SDL_EVENT_WINDOW_RESIZED, std::bind(&MouseInteractive::OnWindowResize, this, std::placeholders::_1));
}

MouseInteractive::~MouseInteractive()
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();
    reg.on_destroy<comp::UIClickable>().disconnect<&MouseInteractive::OnClickableDestroy>(this);
    reg.on_destroy<comp::UIPointable>().disconnect<&MouseInteractive::OnPointableDestroy>(this);
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
        if (auto& callback = reg.get<game::comp::UIClickable>(entities.front()).m_ClickInCallback)
            callback();
    } else {
        if (auto& callback = reg.get<game::comp::UIClickable>(entities.front()).m_ClickOutCallback)
            callback();
    }
}

void MouseInteractive::OnMotion(const SDL_Event* event)
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();

    auto entities = m_Pointables->query(event->motion.x, event->motion.y);
    if (entities.empty()) {
        if (auto* pointable = reg.try_get<game::comp::UIPointable>(m_Pointed); pointable && pointable->m_PointOutCallback)
            pointable->m_PointOutCallback();
        m_Pointed = entt::null;
        return;
    }

    std::sort(entities.begin(), entities.end(),
        [](const entity& lhs, const entity& rhs) { return utility::Registry::GetInstance().GetRegistry().get<app::comp::ZIndex>(lhs).m_Index
                                                       > utility::Registry::GetInstance().GetRegistry().get<app::comp::ZIndex>(rhs).m_Index; });

    if (m_Pointed != entities.front()) {
        if (auto* pointable = reg.try_get<game::comp::UIPointable>(m_Pointed); pointable && pointable->m_PointOutCallback)
            pointable->m_PointOutCallback();
        m_Pointed = entities.front();
        if (auto& callback = reg.get<game::comp::UIPointable>(m_Pointed).m_PointInCallback; callback)
            callback();
    }
}

void MouseInteractive::OnWindowResize(const SDL_Event*)
{
    m_Clickables = std::make_unique<InteractiveContainer_t>(
        app::Renderer::GetInstance().GetRenderSize().x / 2.f,
        app::Renderer::GetInstance().GetRenderSize().y / 2.f,
        app::Renderer::GetInstance().GetRenderSize().x / 2.f,
        app::Renderer::GetInstance().GetRenderSize().y / 2.f);

    auto& registry = utility::Registry::GetRegistry();
    const auto cview = registry.view<game::comp::UIClickable>();
    for (const auto ent : cview) {
        const auto& rect = cview.get<game::comp::UIClickable>(ent).m_Area;
        const auto* ui = registry.try_get<game::comp::UIElement>(ent);
        const mathfu::vec4 area { app::Layout::Transform(ui ? ui->m_Anchor : AnchorNull, rect) };
        m_Clickables->insert(ent, area.x, area.y, area.z, area.w);
    }

    m_Pointables = std::make_unique<InteractiveContainer_t>(
        app::Renderer::GetInstance().GetRenderSize().x / 2.f,
        app::Renderer::GetInstance().GetRenderSize().y / 2.f,
        app::Renderer::GetInstance().GetRenderSize().x / 2.f,
        app::Renderer::GetInstance().GetRenderSize().y / 2.f);

    const auto pview = utility::Registry::GetRegistry().view<game::comp::UIPointable>();
    for (const auto ent : pview) {
        const auto& rect = pview.get<game::comp::UIPointable>(ent).m_Area;
        const auto* ui = registry.try_get<game::comp::UIElement>(ent);
        const mathfu::vec4 area { app::Layout::Transform(ui ? ui->m_Anchor : AnchorNull, rect) };
        m_Pointables->insert(ent, area.x, area.y, area.z, area.w);
    }
}

void MouseInteractive::OnClickableConstruct(entt::registry& registry, entt::entity entity)
{
    assert(registry.all_of<app::comp::ZIndex>(entity));
    const auto& rect = registry.get<game::comp::UIClickable>(entity).m_Area;
    const auto* ui = registry.try_get<game::comp::UIElement>(entity);
    const mathfu::vec4 area { app::Layout::Transform(ui ? ui->m_Anchor : AnchorNull, rect) };
    m_Clickables->insert(entity, area.x, area.y, area.z, area.w);
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
    const auto& rect = registry.get<game::comp::UIPointable>(entity).m_Area;
    const auto* ui = registry.try_get<game::comp::UIElement>(entity);
    const mathfu::vec4 area { app::Layout::Transform(ui ? ui->m_Anchor : AnchorNull, rect) };
    m_Pointables->insert(entity, area.x, area.y, area.z, area.w);
}

void MouseInteractive::OnPointableDestroy(entt::registry& registry, entt::entity entity)
{
    m_Pointables->remove(entity);
}
