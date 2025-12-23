#pragma once
#include "utility/Topics.hpp"

namespace app {
class ScriptManager : public utility::Singleton<ScriptManager>
{
    friend class Singleton<ScriptManager>;

public:
    inline static const std::filesystem::path SCRIPT_ROOT_PATH = "scripts/";

    // standard local method
    static constexpr std::string_view SLM_Initialize = "Initialize";
    static constexpr std::string_view SLM_PreReload = "PreReload";
    static constexpr std::string_view SLM_PostReload = "PostReload";
    // standard global method
    static constexpr std::string_view SGM_ReloadModule = "ReloadModule";

public:
    static sol::state& GetLuaState() noexcept { return GetInstance().m_LuaState; }
    static std::filesystem::path GetScriptFilePath(std::string_view file_name) { return SCRIPT_ROOT_PATH / file_name; }
    static std::string GetScriptFilePathString(std::string_view file_name) { return (SCRIPT_ROOT_PATH / file_name).string(); }

    template <typename F>
    static utility::TopicsSubscriberID Subscribe(const std::filesystem::path& scriptFilePath, F&& callback)
    {
        if (scriptFilePath.empty()) return utility::TopicsSubscriberIDNull;
        GetInstance().m_FileLastWrite[scriptFilePath] = std::filesystem::last_write_time(scriptFilePath);
        return GetInstance().m_FileChanged.Subscribe(scriptFilePath, std::forward<F>(callback));
    }

    static void Unsubscribe(const std::filesystem::path& scriptFilePath, utility::TopicsSubscriberID sid)
    {
        GetInstance().m_FileChanged.Unsubscribe(scriptFilePath, sid);
        GetInstance().m_FileLastWrite.erase(scriptFilePath);
    }

private:
    ScriptManager();
    ~ScriptManager() = default;

    bool DetectChanges();
    sol::object LuaRequire(const std::string&);

private:
    sol::state m_LuaState;
    utility::Topics<std::filesystem::path> m_FileChanged;
    std::unordered_map<std::filesystem::path, std::filesystem::file_time_type> m_FileLastWrite;
};
}