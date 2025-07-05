#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include "app/EventBus.hpp"
#include "app/Layout.hpp"

#include "app/System/Clickable.hpp"
#include "app/System/Input.hpp"
#include "app/System/RenderCallback.hpp"
#include "app/System/TextureRender.hpp"
#include "app/System/Time.hpp"
//-------------------------------------------
#include "game/Game.hpp"
#include "game/Camera.hpp"
#include "game/World.hpp"

#include "game/System/Move.hpp"
#include "game/System/Logic.hpp"
#include "game/System/Visible.hpp"

SDL_AppResult SDL_AppInit(void**, int, char**)
{
    SDL_SetAppMetadata(app::AppMetaData::NAME, app::AppMetaData::VERSION, nullptr);
    if (!SDL_Init(SDL_INIT_VIDEO))
        return SDL_APP_FAILURE;

    app::Window::GetInstance();
    app::Renderer::GetInstance();
    app::Layout::GetInstance();
    app::sys::Input::GetInstance();

    utility::Registry::GetInstance(); // Construct registry
    app::sys::Clickable::GetInstance();
    app::sys::TextureRender::GetInstance();

    game::sys::Visible::GetInstance();
    game::World::GetInstance();
    game::Game::GetInstance();
    game::Camera::GetInstance();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void*)
{
    app::sys::Time::GetInstance().Tick();
    //SDL_Log("fps: %.2f\n", 1.f / app::sys::Time::GetInstance().GetDeltaTime());

    while (app::sys::Time::GetInstance().ConsumeTick()) {
        // Fixed Update
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
    app::sys::TextureRender::GetInstance().Tick();
    app::sys::PostRender::GetInstance().Tick();

    SDL_RenderPresent(renderer);
    // Render End

    app::EventBus::GetInstance().CleanUp();
    app::sys::Input::GetInstance().CleanUp();
    game::sys::Visible::GetInstance().CleanUp();
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
}
