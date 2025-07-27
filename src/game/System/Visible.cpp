#include "Visible.hpp"
#include "game/Camera.hpp"
#include "game/Component/Visible.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/UIElement.hpp"
#include "app/Component/Render/Texture.hpp"
#include "app/Component/Render/Text.hpp"

using namespace game::sys;
using namespace entt;
using namespace mathfu;

static void OnUIElementConstruct(entt::registry& reg, entt::entity ent)
{
    reg.emplace<game::comp::Visible>(ent);
}

Visible::Visible()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    reg.on_construct<game::comp::UIElement>().connect<OnUIElementConstruct>();
    reg.on_construct<game::comp::Transform>().connect<&Visible::OnTransformConstruct>(this);
    reg.on_destroy<game::comp::Transform>().connect<&Visible::OnTransformDestroy>(this);
}

static bool CheckValid(const registry& reg, entity ent)
{
    return reg.all_of<game::comp::UIElement>(ent) || !reg.any_of<app::comp::Texture, app::comp::Text>(ent);
}

void Visible::OnTransformConstruct(entt::registry& reg, entt::entity ent)
{
    if (CheckValid(reg, ent)) return;

    auto& tsfm = reg.get<game::comp::Transform>(ent);
    m_WorldLTGrid.Insert(ent, tsfm.m_Position.x, tsfm.m_Position.y, tsfm.m_HalfSize.x, tsfm.m_HalfSize.y);
}

void Visible::OnTransformUpdate(entt::entity ent, const game::comp::Transform& last, const game::comp::Transform& now)
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    if (CheckValid(reg, ent)) return;

    m_WorldLTGrid.Move(ent, last.m_Position.x, last.m_Position.y, now.m_Position.x, now.m_Position.y);
}

void Visible::OnTransformDestroy(entt::registry& reg, entt::entity ent)
{
    if (reg.any_of<app::comp::Texture, app::comp::Text>(ent) || reg.all_of<game::comp::UIElement>(ent)) return;

    auto& tsfm = reg.get<game::comp::Transform>(ent);
    m_WorldLTGrid.Delete(ent, tsfm.m_Position.x, tsfm.m_Position.y);
}

void Visible::Tick()
{
    m_WorldLTGrid.Optimize();

    registry& reg = utility::Registry::GetInstance().GetRegistry();
    game::comp::Transform& cam_tsfom = reg.get<game::comp::Transform>(game::Camera::GetInstance().GetCameraEntity());

    const vec2& cam_posn = cam_tsfom.m_Position;
    const vec2& half_fov = cam_tsfom.m_HalfSize;
    const vec4 view_field { cam_posn.x - half_fov.x, cam_posn.y - half_fov.y, cam_posn.x + half_fov.x, cam_posn.y + half_fov.y };
    std::vector<entity> visibles(m_WorldLTGrid.Serach(view_field.x, view_field.y, view_field.z, view_field.w));

    for (auto& ent : visibles) {
        reg.emplace<game::comp::Visible>(ent);
    }
}

void Visible::CleanUp()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    auto view = reg.view<game::comp::Visible>();

    for (auto ent : view) {
        if (reg.all_of<game::comp::UIElement>(ent)) continue;
        reg.erase<game::comp::Visible>(ent);
    }
}
