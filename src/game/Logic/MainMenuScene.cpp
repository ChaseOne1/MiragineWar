#include "MainMenuScene.hpp"
#include "app/Resources.hpp"
#include "app/Layout.hpp"
#include "app/Component/Render/Texture.hpp"
#include "app/Component/Render/RenderCallback.hpp"
#include "app/resources/AllInOneRes.hpp"
#include "app/resources/AllInOneIndex.hpp"
#include "app/Component/Render/ZIndex.hpp"
#include "app/System/Time.hpp"
#include "game/System/Scene.hpp"
#include "game/Component/UIElement.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/Movement.hpp"
#include "game/Component/Logic.hpp"
#include "game/Camera.hpp"
#include "game/World.hpp"
#include "app/Component/Render/TextureGrid.hpp"
#include "utility/Random.hpp"

using namespace game::logic;
using namespace entt;
using namespace mathfu;

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
    const toml::table& settings = *app::Settings::GetInstance().GetSettings()["MainMenuScene"].as_table(); // TODO: initialize with at_path MainMenuScene
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
        entity& soldier = m_Soldiers.emplace_back(registry.create());
        const int8_t mirrored = i > m_Soldiers.capacity() >> 1 ? 1 : -1;

        // Texture
        std::shared_ptr<app::AnimSeqFrames> texture_asf = resources.Require<app::AnimSeqFrames>(soldiers_guid[guid_dist(gen)], app::idx::SOLDIERS_IDX);
        auto& texture_comp = registry.emplace<app::comp::Texture>(soldier, texture_asf->m_Frames,
            SDL_FRect { 0.f, static_cast<float>(texture_asf->GetGlobalCoordY(app::AnimSeqFrames::ANIM_RUN)),
                mirrored * static_cast<float>(texture_asf->m_Info[app::AnimSeqFrames::ANIM_RUN].width),
                static_cast<float>(texture_asf->m_Info[app::AnimSeqFrames::ANIM_RUN].height) });

        // Transform
        const float size_ratio = **settings["soldiers_size_ratio"].as_floating_point();
        auto& transform_comp = registry.emplace<game::comp::Transform>(soldier,
            vec2 { cam_center.x + mirrored * (distance_x_from - posn_dist(gen) * (distance_x_from - distance_x_to)),
                cam_center.y - distance_hy + posn_dist(gen) * distance_hy * 2 },
            vec2 { texture_asf->m_Info[app::AnimSeqFrames::ANIM_RUN].width / 2.f * size_ratio,
                texture_asf->m_Info[app::AnimSeqFrames::ANIM_RUN].height / 2.f * size_ratio });

        // ZIndex
        registry.emplace<app::comp::ZIndex>(soldier, app::comp::ZIndex(app::comp::ZIndex::ZINDEX_SOLDIER + transform_comp.m_Position.y / game::World::GetInstance().msc_fHeight * 100u));

        // Movement
        registry.emplace<game::comp::Movement>(soldier,
            vec2 { -mirrored * static_cast<float>(**settings["soldiers_speed"].as_floating_point()), 0.f });

        struct Context
        {
            uint64_t m_start_time = 0u;
            std::shared_ptr<app::AnimSeqFrames> m_asf;
            app::AnimSeqFrames::ANIM m_anim = app::AnimSeqFrames::ANIM_IDLE;
        };
        std::shared_ptr<Context> texture_ctx = std::make_shared<Context>(
            Context { app::sys::Time::GetInstance().GetRealTime(), texture_asf, app::AnimSeqFrames::ANIM_RUN });

        // Logic
        const float posn_to = cam_center.x + mirrored * (distance_x_to + posn_dist(gen) * **settings["soldiers_float_length"].as_floating_point());
        // we don't capture components because deleting the movement component will invalidate those references
        registry.emplace<game::comp::Logic>(soldier, [soldier, posn_to, texture_ctx, size_ratio, mirrored]() {
            auto& registry = utility::Registry::GetInstance().GetRegistry();
            auto& transform_comp = registry.get<game::comp::Transform>(soldier);
            if (-mirrored * transform_comp.m_Position.x > -mirrored * posn_to && registry.all_of<game::comp::Movement>(soldier)) {
                registry.erase<game::comp::Movement>(soldier);

                transform_comp.m_HalfSize.x = texture_ctx->m_asf->m_Info[app::AnimSeqFrames::ANIM_IDLE].width / 2.f * size_ratio;
                transform_comp.m_HalfSize.y = texture_ctx->m_asf->m_Info[app::AnimSeqFrames::ANIM_IDLE].height / 2.f * size_ratio;

                auto& texture_comp = registry.get<app::comp::Texture>(soldier);
                texture_comp.m_SrcFRect->y = texture_ctx->m_asf->GetGlobalCoordY(app::AnimSeqFrames::ANIM_IDLE);
                texture_comp.m_SrcFRect->w = mirrored * texture_ctx->m_asf->m_Info[app::AnimSeqFrames::ANIM_IDLE].width;
                texture_comp.m_SrcFRect->h = texture_ctx->m_asf->m_Info[app::AnimSeqFrames::ANIM_IDLE].height;

                texture_ctx->m_start_time = app::sys::Time::GetInstance().GetRealTime();
                texture_ctx->m_anim = app::AnimSeqFrames::ANIM_IDLE;
            }
        });

        // PreRender
        // we don't capture the texture component because deleting the movement component will invalidate the reference
        registry.emplace<app::comp::PreRender>(soldier, [texture_ctx, soldier]() {
            const uint16_t passed_frame = (app::sys::Time::GetInstance().GetRealTime() - texture_ctx->m_start_time)
                * texture_ctx->m_asf->m_Info[texture_ctx->m_anim].speed;
            utility::Registry::GetInstance().GetRegistry().get<app::comp::Texture>(soldier).m_SrcFRect->x
                = texture_ctx->m_asf->m_Info[texture_ctx->m_anim].GetCoordX(passed_frame);
        });
    }
}
