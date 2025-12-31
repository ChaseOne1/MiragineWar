#include "Game.hpp"

#include "app/ScriptManager.hpp"
#include "game/Component/Logic.hpp"
#include "game/Logic/MainMenuScene.hpp"
#include "game/System/NetGame.hpp"

using namespace game;
using namespace entt;

Game::Game()
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();
    reg.emplace<game::comp::Logic>(m_Game, std::bind(&Game::LogicInit, this), []() { }, []() { });

    // app::ScriptManager::Subscribe(app::ScriptManager::GetScriptFilePath("game.lua"), [](){});
}

void Game::LogicInit()
{
    game::logic::MainMenuScene::GetInstance();
    game::sys::NetGame::GetInstance();
}

// void LogicInit()
//{
//     entt::registry& reg = utility::Registry::GetInstance().GetRegistry();
//     app::Resources& ress = app::Resources::GetInstance();
//
//     // set login menu
//     try {
//         app::Layout& layout = app::Layout::GetInstance();
//         std::shared_ptr<toml::table> layouts = ress.Require<toml::table>(app::res::BOOT_LO, app::idx::MISC_IDX);
//
//         // title
//         entity title = reg.create();
//         reg.emplace<game::comp::UIElement>(title);
//         std::shared_ptr<AnimSeqFrames> title_img = ress.Require<AnimSeqFrames>(app::res::PERSON_IMG, app::idx::MISC_IDX);
//         auto& title_texture = reg.emplace<app::comp::Texture>(title, title_img->m_Frames, 0u,
//             std::make_optional(SDL_FRect { 0.f, 0.f, static_cast<float>(title_img->m_Idle.width), static_cast<float>(title_img->m_Idle.height) }),
//             std::make_optional(layout.Transform(layouts->at_path("title"))));
//         reg.emplace<app::comp::PreRender>(title, [=, &title_texture]() {
//             static uint64_t start_time = app::sys::Time::GetInstance().GetRealTime();
//             static AnimSeqFrames::AnimInfo* info = &title_img->m_Idle;
//
//             // switch animation automatically
//             if (app::sys::Time::GetInstance().GetRealTime() - start_time > 1000) {
//                 title_texture.m_SrcFRect->y += info->height;
//                 info += 1;
//                 start_time = app::sys::Time::GetInstance().GetRealTime();
//                 if (info == (&title_img->m_Die + 1)) {
//                     info = &title_img->m_Idle;
//                     title_texture.m_SrcFRect->y = 0.f;
//                 }
//                 title_texture.m_SrcFRect->w = info->width;
//                 title_texture.m_SrcFRect->h = info->height;
//             }
//
//             const int passed_frame = (app::sys::Time::GetInstance().GetRealTime() - start_time) * info->speed;
//             const uint8_t curr_frame = passed_frame % info->count;
//             constexpr uint8_t frame_padding = 1;
//             title_texture.m_SrcFRect->x = curr_frame * (title_texture.m_SrcFRect->w + frame_padding); // padding
//         });
//
//         // TODO: save the retVal to unsubscribe
//         app::EventBus::GetInstance().Subscribe(SDL_EVENT_WINDOW_RESIZED, [=, &title_texture]() {
//             SDL_Event* event = app::EventBus::GetInstance().GetEvent();
//             app::Layout& layout = app::Layout::GetInstance();
//             title_texture.m_DstFRect = layout.Transform(layouts->at_path("title"));
//         });
//
//     } catch (toml::parse_error e) {
//         SDL_Log("%s\n", e.description().data());
//     }
//
//     game::sys::Scene::GetInstance();
//     // set button
//     // reg.emplace<app::comp::Clickable>(m_Game, SDL_Rect { 0, 0, 200, 200 }, []() { SDL_Log("Clicked!"); });
// }
