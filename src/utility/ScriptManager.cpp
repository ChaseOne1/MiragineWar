#include "ScriptManager.hpp"
#include "app/System/Timer.hpp"

using namespace utility;

ScriptManager::ScriptManager()
{
    m_LuaState.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string, sol::lib::table);

    m_fnRequirer = m_LuaState["require"];
    const std::string curr_path = m_LuaState["package"]["path"];
    m_LuaState["package"]["path"] = curr_path + ";" + SCRIPT_ROOT_PATH.string() + "?.lua";
}

void ScriptManager::SetupFileChangeDetection()
{
    app::sys::Timer::AddTimer(std::chrono::milliseconds(1000u), [] { return GetInstance().DetectChanges(); });
}

bool ScriptManager::DetectChanges()
{
    // we use this container to store the changed files
    // because if someone unsubscribe, it will cause the iterator to become invalid
    std::forward_list<std::reference_wrapper<const std::filesystem::path>> paths;

    for(auto begin = m_subscribers.cbegin(); begin != m_subscribers.cend(); ++begin)
    {
        auto& path = begin->first;
        auto& lwt = m_FileLastWriteTimes.at(path);

        auto current_lwt = std::filesystem::last_write_time(path);
        if (current_lwt <= lwt) continue;

        LOGI("the file {} has been changed", path.string());
        paths.emplace_front(path);
        lwt = current_lwt;
    }

    for(auto& path : paths) m_subscribers.Publish(path);

    return true;
}
