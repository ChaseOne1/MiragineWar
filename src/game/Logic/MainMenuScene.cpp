#include "MainMenuScene.hpp"
#include "app/Resources.hpp"
#include "app/Layout.hpp"
#include "app/Component/Render/Texture.hpp"
#include "app/resources/AllInOneRes.hpp"
#include "app/resources/AllInOneIndex.hpp"
#include "app/resources/AnimSeqFrames.hpp"
#include "app/Component/Render/ZIndex.hpp"
#include "game/System/Scene.hpp"
#include "game/Component/UIElement.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/Movement.hpp"
#include "game/Camera.hpp"
#include "utility/Random.hpp"
#include "game/Utility/Soldier.hpp"

using namespace game::logic;
using namespace entt;
using namespace mathfu;
using namespace game::util;

MainMenuScene::MainMenuScene()
{
    SetupWorldScene();
    SetupTitle();
    SetupSoldiers();
}

void MainMenuScene::SetupWorldScene()
{
    game::sys::Scene::GetInstance();
}

void MainMenuScene::SetupTitle()
{
    registry& registry = utility::Registry::GetInstance().GetRegistry();
    app::Resources& resources = app::Resources::GetInstance();
    const app::Layout& layouts = app::Layout::GetInstance();

    registry.emplace<game::comp::UIElement>(m_Title);

    const std::shared_ptr<SDL_Texture>& title_img = resources.Require<SDL_Texture>(app::res::TITLE_IMG, app::idx::MISC_IDX);
    registry.emplace<app::comp::Texture>(m_Title, title_img, std::nullopt);
    registry.emplace<app::comp::ZIndex>(m_Title, app::comp::ZIndex::ZINDEX_UIELEMENT);

    const std::shared_ptr<toml::table>& boot_layouts = resources.Require<toml::table>(app::res::BOOT_LO, app::idx::MISC_IDX);
    const SDL_FRect& layout = layouts.Transform(*boot_layouts->at("title").as_table());
    registry.emplace<game::comp::Transform>(m_Title, vec2 { layout.x + layout.w / 2.f, layout.y + layout.h / 2.f }, vec2 { layout.w / 2.f, layout.h / 2.f });
}

void MainMenuScene::SetupSoldiers()
{
    using namespace app::res;
    registry& registry = utility::Registry::GetInstance().GetRegistry();
    app::Resources& resources = app::Resources::GetInstance();
    const toml::table& settings = *app::Settings::GetInstance().GetSettings()["MainMenuScene"].as_table();
    constexpr std::array<app::GUID_t, 16u> soldiers_guid = {
        RECRUIT_ASF, VETERAN_ASF, ZOMBIE_ASF, SAMURAI_ASF,
        SWORDSMAN_ASF, NINJA_ASF, SISTER_ASF, ARMORED_SWORDSMAN_ASF,
        MONK_ASF, VAMPIRE_ASF, KNIGHT_ASF, IMMORTAL_ASF,
        ARCANIST_ASF, GENERAL_ASF, DREADLORD_ASF, HIGHLORD_ASF
    };

    std::mt19937& gen = utility::Random::GetInstance().GetGenerator();
    std::uniform_int_distribution<std::size_t> guid_dist(0, 15);
    std::uniform_real_distribution<float> posn_dist(0.f, 1.f);
    const vec2& cam_center = registry.get<game::comp::Transform>(game::Camera::GetInstance().GetCameraEntity()).m_Position;
    const float distance_x_from = **settings.at_path("soldiers_distance_x.from").as_floating_point(),
                distance_x_to = **settings.at_path("soldiers_distance_x.to").as_floating_point(),
                distance_hy = **settings["soldiers_distance_hy"].as_floating_point();

    m_Soldiers.reserve(**settings["soldiers_num"].as_integer());
    for (int i = 0; i < m_Soldiers.capacity(); ++i) {
        const int8_t mirrored = i > m_Soldiers.capacity() - 1 >> 1 ? 1 : -1;

        auto& soldier = m_Soldiers.emplace_back(soldiers_guid[guid_dist(gen)],
            vec2 { cam_center.x + mirrored * (distance_x_from - posn_dist(gen) * (distance_x_from - distance_x_to)),
                cam_center.y - distance_hy + posn_dist(gen) * distance_hy * 2 },
            app::AnimSeqFrames::ANIM_RUN, mirrored < 0);

        // Movement
        soldier.SetMovement(vec2 { -mirrored * settings["soldiers_speed"].value_or(32.f), 0.f });

        // Logic
        const float posn_to = cam_center.x + mirrored * (distance_x_to + posn_dist(gen) * settings["soldiers_float_length"].value_or(50.f));
        // we don't capture components because deleting the movement component will invalidate those references
        soldier.SetLogic([&soldier, posn_to, mirrored]() mutable {
            auto& registry = utility::Registry::GetInstance().GetRegistry();
            entity soldier_ent = soldier.GetEntity();
            const vec2& posn = registry.get<game::comp::Transform>(soldier_ent).m_Position;
            if (-mirrored * posn.x > -mirrored * posn_to && registry.all_of<game::comp::Movement>(soldier_ent)) {
                registry.erase<game::comp::Movement>(soldier_ent);
                soldier.SetAnimation(app::AnimSeqFrames::ANIM_IDLE);
            }
        });
    }
}
