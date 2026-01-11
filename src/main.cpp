#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include "app/EventBus.hpp"
#include "app/Layout.hpp"
#include "app/Resources.hpp"
#include "app/TextEngine.hpp"

#include "app/System/Input.hpp"
#include "app/System/RenderCallback.hpp"
#include "app/System/Render.hpp"
#include "app/System/Time.hpp"
#include "app/System/Timer.hpp"
#include "app/System/Network.hpp"
//-------------------------------------------
#include "app/ScriptManager.hpp"
#include "game/Game.hpp"
#include "game/Camera.hpp"

#include "game/System/Move.hpp"
#include "game/System/Logic.hpp"
#include "game/System/Visible.hpp"
#include "game/System/GarbageCollection.hpp"
#include "game/System/MouseInteractive.hpp"

SDL_AppResult SDL_AppInit(void**, int, char**)
{
    SDL_SetAppMetadata(app::AppMetaData::NAME, app::AppMetaData::VERSION, nullptr);
    if (!SDL_Init(SDL_INIT_VIDEO) || !TTF_Init())
        return SDL_APP_FAILURE;

    app::Window::GetInstance();
    app::Renderer::GetInstance();
    app::TextEngine::GetInstance();
    app::ScriptManager::GetInstance();
    utility::Registry::GetInstance(); // Registry begin
    app::Layout::GetInstance(); // EventBus begin
    app::sys::Input::GetInstance();
    app::sys::Timer::GetInstance();
    app::sys::Network::GetInstance();

    app::sys::Render::GetInstance();

    game::sys::MouseInteractive::GetInstance();
    game::sys::Visible::GetInstance();
    game::Game::GetInstance();
    game::Camera::GetInstance();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void*)
{
    app::sys::Time::GetInstance().Tick();
    app::sys::Timer::GetInstance().Tick();
    app::sys::Network::GetInstance().Tick();

    uint8_t tick_count = 0u;
    constexpr uint8_t max_tick_count = 6u;
    while (app::sys::Time::GetInstance().FixedTick()) {
        // Fixed Update
        // if(tick_count > max_tick_count) continue; else ++tick_count;
        game::sys::Move::GetInstance().Tick();
        // game::sys::Collide::GetInstance().Tick();
        // game::sys::Trigger::GetInstance().Tick();
    }

    game::sys::Logic::GetInstance().Tick();
    game::sys::Visible::GetInstance().Tick();

    // Render Begin
    SDL_Renderer* renderer = app::Renderer::GetInstance().GetSDLRenderer();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // Tick
    app::sys::PreRender::GetInstance().Tick();
    app::sys::Render::GetInstance().Tick();
    app::sys::PostRender::GetInstance().Tick();

    SDL_RenderPresent(renderer);
    // Render End

    app::EventBus::GetInstance().CleanUp();
    game::sys::Visible::GetInstance().CleanUp();
    game::sys::GarbageCollection::GetInstance().Tick();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void*, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    app::EventBus::GetInstance().Tick(event);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void*, SDL_AppResult result)
{
    // release TTF_Font before TTF_Quit, or SIGSEGV
    utility::Registry::GetInstance().GetRegistry().clear();
    app::Resources::GetInstance().CleanUp();

    // fine, you win
    // TTF_Quit();

    // SDL_Quit(); not required in the case of callback main
}
