#include "Visible.hpp"
#include "game/Camera.hpp"
#include "game/World.hpp"
#include "game/Component/Visible.hpp"
#include "game/Component/Position.hpp"

using namespace game::sys;
using namespace entt;

// TODO: only tick after camera movement
void Visible::Tick()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    const World& world = World::GetInstance();
    game::Camera& cam = game::Camera::GetInstance();

    const auto& cam_posn = reg.get<game::comp::Position>(cam.GetCameraEntity()).m_Position;
    const utility::math::Vector2& half_fov = cam.GetHalfFOV();
    SDL_FRect view_field { cam_posn.x - half_fov.x, cam_posn.y - half_fov.y, cam_posn.x + half_fov.x, cam_posn.y + half_fov.y };
    std::vector<entity> visibles(world.SearchVisibleObjects(view_field.x, view_field.y, view_field.w, view_field.h));

    for (auto& ent : visibles) {
        reg.emplace<game::comp::Visible>(ent);
    }
}

void Visible::CleanUp()
{
    registry& reg = utility::Registry::GetInstance().GetRegistry();
    auto view = reg.view<game::comp::Visible>();
    for (auto ent : view) {
        reg.erase<game::comp::Visible>(ent);
    }
}
