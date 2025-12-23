#include "ScriptManager.hpp"
#include "app/System/Timer.hpp"

using namespace app;

static sol::function gsc_fnRequirer;
static constexpr std::string_view gsc_ScriptEngineFileName = "ScriptEngine.lua";

ScriptManager::ScriptManager()
{
    m_LuaState.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string, sol::lib::table);

    // override require for hot-reloading
    gsc_fnRequirer = m_LuaState["require"];
    const std::string curr_path = m_LuaState["package"]["path"];
    m_LuaState["package"]["path"] = curr_path + ";" + SCRIPT_ROOT_PATH.string() + "?.lua";
    m_LuaState["require"] = [](const std::string_view name) { return ScriptManager::GetInstance().LuaRequire(std::string(name)); };

    // inject global functions
    m_LuaState.script_file(GetScriptFilePathString(gsc_ScriptEngineFileName));

    // TODO: disable this function if release mode
    app::sys::Timer::AddTimer(std::chrono::milliseconds(1000u), []() { return ScriptManager::GetInstance().DetectChanges(); });
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

sol::object ScriptManager::LuaRequire(const std::string& name)
{
    const std::string path{ GetScriptFilePathString(name + ".lua") };

    if (!m_FileChanged.GetSubscriberCount(path)) {
        Subscribe(path, [=]() {
            // NOTE: if the module already has a reloading handler, it should do something like this.
            // update the cache in package loaded for downstream dependencies
            static_cast<void>(GetLuaState()[SGM_ReloadModule].get<sol::function>()(name));
        });
    }

    return gsc_fnRequirer(name);
}