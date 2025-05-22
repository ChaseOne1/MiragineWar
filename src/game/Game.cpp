#include "Game.hpp"
#include "app/Component/Clickable.hpp"
#include "app/Resources.hpp"
#include "app/resources/AllInOneRes.hpp"
#include "app/resources/AllInOneIndex.hpp"
#include "game/Component/Logic.hpp"
#include "game/Component/Movement.hpp"
#include "game/Component/Position.hpp"
#include "app/Component/Render/Texture.hpp"
#include "app/Component/render/RenderCallback.hpp"
#include "app/Layout.hpp"

using namespace game;
using namespace entt;

Game::Game()
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();
    reg.emplace<game::comp::Logic>(m_Game, std::bind(&Game::LogicInit, this), []() { }, []() { });
}

Game::~Game()
{
    auto& reg = utility::Registry::GetInstance().GetRegistry();
    reg.destroy(m_Game);
}

void Game::LogicInit()
{
    entt::registry& reg = utility::Registry::GetInstance().GetRegistry();
    app::Resources& ress = app::Resources::GetInstance();

    // set login menu
    try {
        app::Layout& layout = app::Layout::GetInstance();
        std::shared_ptr<toml::table> layouts = ress.Require<toml::table>(app::res::BOOT_LO, app::idx::MISC_IDX);

        // title
        entity title = reg.create();
        std::shared_ptr<SDL_Texture> title_img = ress.Require<SDL_Texture>(app::res::TITLE_IMG, app::idx::MISC_IDX);
        auto& title_texture = reg.emplace<app::comp::Texture>(title, std::move(title_img), 0u,
            std::optional<SDL_FRect>(std::nullopt),
            std::make_optional(layout.Transform(layouts->at_path("title"))));
        // TODO: save the retVal to unsubscribe
        app::EventBus::GetInstance().Subscribe(SDL_EVENT_WINDOW_RESIZED, [=, &title_texture]() {
            SDL_Event* event = app::EventBus::GetInstance().GetEvent();
            app::Layout& layout = app::Layout::GetInstance();
            title_texture.m_DstRect = layout.Transform(layouts->at_path("title"));
        });

        // dialog background
        entity dialog = reg.create();
        std::shared_ptr<SDL_Texture> dialog_bg = ress.Require<SDL_Texture>(app::res::DIALOG_BG_IMG, app::idx::MISC_IDX);
        auto& dialog_texture = reg.emplace<app::comp::Texture>(dialog, std::move(dialog_bg), 0u,
            std::optional<SDL_FRect>(std::nullopt),
            std::make_optional(layout.Transform(layouts->at_path("login_dialog"))));
        // TODO: save the retVal to unsubsrcibe
        app::EventBus::GetInstance().Subscribe(SDL_EVENT_WINDOW_RESIZED, [=, &dialog_texture]() {
            SDL_Event* event = app::EventBus::GetInstance().GetEvent();
            app::Layout& layout = app::Layout::GetInstance();
            dialog_texture.m_DstRect = layout.Transform(layouts->at_path("login_dialog"));
        });

    } catch (toml::parse_error e) {
        SDL_Log("%s\n", e.description().data());
    }

    // set button
    // reg.emplace<app::comp::Clickable>(m_Game, SDL_Rect { 0, 0, 200, 200 }, []() { SDL_Log("Clicked!"); });
}
