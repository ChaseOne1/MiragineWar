#include "game/System/Scene.hpp"
#include "app/Resources.hpp"
#include "app/resources/AllInOneRes.hpp"
#include "app/resources/AllInOneIndex.hpp"
#include "app/Component/Texture.hpp"
#include "app/Component/ZIndex.hpp"
#include "game/Component/Transform.hpp"
#include "utility/Registry.hpp"

using namespace game::sys;
using namespace entt;
using namespace mathfu;

Scene::Scene()
{
    entt::registry& reg = utility::Registry::GetInstance().GetRegistry();
    app::Resources& rsc = app::Resources::GetInstance();
    for (int i = 0; i < 3; ++i) {
        entity ground = reg.create();
        std::shared_ptr<SDL_Texture> ground_tex = rsc.Require<SDL_Texture>(app::res::GROUND_IMG, app::idx::ENV_IDX);
        reg.emplace<app::comp::Texture>(ground, std::move(ground_tex), std::nullopt);
        reg.emplace<app::comp::ZIndex>(ground, app::comp::ZIndexVal::ZINDEX_GROUND);
        reg.emplace<game::comp::Transform>(ground, vec2 { 384.f + i * 128.f, 128.f }, vec2 { 64.f, 128.f });
    }
}
