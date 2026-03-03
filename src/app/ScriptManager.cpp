#include "ScriptManager.hpp"
#include "app/ScriptLib.hpp"
#include "app/System/Timer.hpp"

using namespace app;

ScriptManager::ScriptManager()
{
    m_LuaState.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string, sol::lib::table);

    // override require for hot-reloading
    m_fnRequirer = m_LuaState["require"];
    const std::string curr_path = m_LuaState["package"]["path"];
    m_LuaState["package"]["path"] = curr_path + ";" + SCRIPT_ROOT_PATH.string() + "?.lua";
    m_LuaState["require"] = [](const sol::string_view name, sol::this_environment env) { return ScriptManager::GetInstance().LuaRequire(std::string(name), env); };

    // TODO: disable this function if release mode
    app::sys::Timer::AddTimer(std::chrono::milliseconds(1000u), [] { return GetInstance().DetectChanges(); });
}

static sol::load_result LoadFileAsModule(const std::string& path, sol::environment& env)
{
    auto& lua = ScriptManager::GetLuaState();
    sol::load_status status = static_cast<sol::load_status>(luaL_loadfilex(lua, path.c_str(),
        sol::to_string(sol::load_mode::any).c_str()));
    if (status == sol::load_status::ok) {
        env.push();
        lua_setupvalue(lua, -2, 1);
    }
    return sol::load_result(lua, sol::absolute_index(lua, -1), 1, 1, status);
}

bool ScriptManager::DetectChanges()
{
    for (auto& [path, info] : m_FileWatchInfos) {
        auto current_lwt = std::filesystem::last_write_time(path);
        if (current_lwt <= info.last_write_time) continue;

        const auto our_clock_tp = std::chrono::time_point_cast<app::sys::Time::Clock_t::duration>(
            current_lwt - decltype(current_lwt)::clock::now() + app::sys::Time::Clock_t::now());
        SDL_Log("ScriptManager::DetectChanges(): the file %s has been changed at %lld",
            path.string().data(),
            std::chrono::time_point_cast<std::chrono::seconds>(our_clock_tp).time_since_epoch().count()
        );

        auto& lua = GetLuaState();
        std::string module_name = info.module_name;
        sol::environment temp_env(lua, sol::create, lua.globals());
        sol::load_result result = LoadFileAsModule(path.string(), temp_env);

        if (result.valid()) {
            sol::table new_module = result();
            sol::table old_module = lua["package"]["loaded"][module_name];
            for (auto& kv : new_module) old_module[kv.first] = kv.second;
            if (sol::object meta = new_module[sol::metatable_key]; meta.valid())
                old_module[sol::metatable_key] = meta;
        } else {
            SDL_Log("ScriptManager::DetectChanges(): failed to reload module %s from %s, status: %d",
                module_name.c_str(), path.string().c_str(), result.status()
            );
        }

        info.last_write_time = current_lwt;
    }

    return true;
}

<<<<<<< HEAD
sol::object ScriptManager::LuaRequire(sol::string_view name, sol::environment& env)
=======
sol::object ScriptManager::LuaRequire(std::string_view name, sol::environment& env)
>>>>>>> 343712d6ff595ccd56d5f2e647c983ac9f822329
{
    if (sol::object module = GetLuaState()["package"]["loaded"][name]; module.valid())
        return module;

    std::string path(name.size(), '\0');
    std::replace_copy(name.cbegin(), name.cend(), path.begin(), '.', '/');
    path = SCRIPT_ROOT_PATH.string() + path + ".lua";
<<<<<<< HEAD

    sol::load_result result = LoadFileAsModule(path, env);
=======
    if (!m_FileChanged.GetSubscriberCount(path)) {
        Subscribe(path, [name = std::string(name)] {
            // NOTE: if the module already has a reloading handler, it should do something like this.
            // update the cache in package loaded for downstream dependencies
            if (sol::function func = GetLuaState()[SGM_ReloadModule].get_or<sol::function>(sol::nil); func.valid())
                func(name);
        });
    }

    const sol::load_status status = static_cast<sol::load_status>(luaL_loadfilex(GetLuaState(), path.c_str(),
        sol::to_string(sol::load_mode::any).c_str()));
    if (status == sol::load_status::ok) {
        static_cast<void>(env.push());
        lua_setupvalue(GetLuaState(), -2, 1);
    }
    sol::load_result result(GetLuaState(), sol::absolute_index(GetLuaState(), -1), 1, 1, status);
>>>>>>> 343712d6ff595ccd56d5f2e647c983ac9f822329
    if (!result.valid()) {
        SDL_Log("do %s failed when require, status: %d", path.data(), result.status());
        throw std::runtime_error { "failed to script the file when require" };
    }

<<<<<<< HEAD
    sol::object new_module = result();
    GetLuaState()["package"]["loaded"][name] = new_module;

    auto last_write = std::filesystem::last_write_time(path);
    m_FileWatchInfos[path] = FileWatchInfo { last_write, std::string(name) };

    return new_module;
=======
    return GetLuaState()["package"]["loaded"][name] = result();
>>>>>>> 343712d6ff595ccd56d5f2e647c983ac9f822329
}
