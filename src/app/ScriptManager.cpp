#include "ScriptManager.hpp"
#include "app/System/Timer.hpp"

using namespace app;

ScriptManager::ScriptManager()
{
    m_LuaState.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string, sol::lib::table);
    // TODO: disable this function if release mode
    app::sys::Timer::AddTimer(std::chrono::milliseconds(1000u), [this]() { return this->DetectChanges(); });
}

bool ScriptManager::DetectChanges()
{
    for (auto ps = m_FileChanged.cbegin(); ps != m_FileChanged.cend(); ++ps) {
        if (auto lwt = std::filesystem::last_write_time(ps->first), &llwt = m_FileLastWrite[ps->first]; lwt > llwt) {
            const auto our_clock_tp = std::chrono::time_point_cast<app::sys::Time::Clock_t::duration>
                (lwt - decltype(lwt)::clock::now() + app::sys::Time::Clock_t::now());
            SDL_Log("ScriptManager::DetectChanges(): the file %s has been changed at %lld",
                ps->first.string().data(), std::chrono::time_point_cast<std::chrono::seconds>(our_clock_tp).time_since_epoch().count());
            m_FileChanged.Publish(ps->first);
            llwt = lwt;
        }
    }

    return true;
}