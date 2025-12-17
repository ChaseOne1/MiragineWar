#include "FPS.hpp"
#include "app/Settings.hpp"
#include "app/System/Time.hpp"

using namespace app;

FPS::FPS() : m_FPS("FPS: 0")
{
    auto& settings = app::Settings::GetSettings();
    m_FPS.SetPosition(settings["app"]["fps_pos_x"], settings["app"]["fps_pos_y"]);
    m_FPS.SetColor(255,255,255);
    m_FPS.SetLogic([this]() {
        m_FPS.SetString(std::string("FPS: ") + std::to_string(1000.f / app::sys::Time::RealDeltaTime().count()));
    });
}

