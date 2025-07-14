#include "Scene.hpp"
#include "app/Resources.hpp"
#include "app/resources/AllInOneRes.hpp"
#include "app/resources/AllInOneIndex.hpp"
#include "app/Component/Render/Texture.hpp"
#include "app/System/ZIndex.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/Collision.hpp"

using namespace game::sys;
using namespace entt;
using namespace mathfu;

Scene::Scene()
{
    entt::registry& reg = utility::Registry::GetInstance().GetRegistry();
    app::Resources& rsc = app::Resources::GetInstance();
    try {
        for (int i = 0; i < 3; ++i) {
            entity ground = reg.create();
            const auto& trans = reg.emplace<game::comp::Transform>(ground, vec2 { 384.f + i * 128.f, 128.f }, vec2 { 64.f, 128.f });
            const auto& coll = reg.emplace<game::comp::Collision>(ground, mathfu::vec4 { 0.f, 0.f, 64.f, 128.f });
            std::shared_ptr<SDL_Texture> ground_img = rsc.Require<SDL_Texture>(app::res::GROUND_IMG, app::idx::ENV_IDX);
            auto& ground_txr = reg.emplace<app::comp::Texture>(ground, std::move(ground_img),
                std::nullopt,
                std::make_optional<SDL_FRect>(), app::sys::ZINDEX_GROUND);
        }
    } catch (toml::parse_error e) {
        SDL_Log("%s\n", e.description().data());
    }
}

void Scene::Tick()
{
}
