#include "FPS.hpp"

#include "System/Timer.hpp"
#include "app/Settings.hpp"
#include "app/System/Time.hpp"

using namespace app;

FPS::FPS() : m_FPS("FPS: 0")
{
    auto& settings = app::Settings::GetSettings();
    m_FPS.SetPosition(settings.at_path("app.fps_pos_x").value_or(0.f), settings.at_path("app.fps_pos_y").value_or(0.f));
    m_FPS.SetColor(255,255,255);
    m_FPS.SetLogic([this]() {
        m_FPS.SetString(std::string("FPS: ") + std::to_string(1000.f / app::sys::Time::RealDeltaTime().count()));
    });

    auto& timer = app::sys::Timer::GetInstance();
    const app::sys::Timer::TimerId timerId = timer.AddTimer(std::chrono::seconds(1), [](){SDL_Log("onTimer"); return true;}, true);
    SDL_Log("add timer id = %d", timerId);
}

