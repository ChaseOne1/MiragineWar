#include "Visible.hpp"
#include "game/Camera.hpp"
#include "game/World.hpp"
#include "game/Component/Visible.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/UIElement.hpp"

using namespace game::sys;
using namespace entt;
using namespace mathfu;

Visible::Visible()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    reg.on_construct<game::comp::UIElement>().connect<&Visible::OnUIElementConstruct>(this);
}

void Visible::OnUIElementConstruct(registry& reg, entity ent)
{
    reg.emplace<game::comp::Visible>(ent);
}

// TODO: only tick after camera movement
void Visible::Tick()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    const World& world = World::GetInstance();
    game::Camera& cam = game::Camera::GetInstance();

    const vec2& cam_posn = reg.get<game::comp::Transform>(cam.GetCameraEntity()).m_Position;
    const vec2& half_fov = cam.GetHalfFOV();
    vec4 view_field { cam_posn.x - half_fov.x, cam_posn.y - half_fov.y, cam_posn.x + half_fov.x, cam_posn.y + half_fov.y };
    std::vector<entity> visibles(world.SearchVisibleObjects(view_field.x, view_field.y, view_field.z, view_field.w));

    for (auto& ent : visibles) {
        // TODO: maybe check if it has texture component
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
