#pragma once
#include "utility/Topics.hpp"

namespace utility {
class ScriptManager : public Singleton<ScriptManager>
{
    friend class Singleton<ScriptManager>;

public:
    inline static const std::filesystem::path SCRIPT_ROOT_PATH = "scripts/";

public:
    static sol::state& GetLuaState() noexcept { return GetInstance().m_LuaState; }

    static void SetupFileChangeDetection();

    using FileChangeHandler_t = std::function<void()>;
    template<typename F>
    static TopicsSubscriberID Subscribe(const std::filesystem::path& path, F&& callback)
    {
        if (!std::filesystem::exists(path)) return TopicsSubscriberIDNull;

        GetInstance().m_FileLastWriteTimes.emplace(path, std::filesystem::last_write_time(path));

        return GetInstance().m_subscribers.Subscribe(path, std::forward<F>(callback));
    }

    static void Unsubscribe(const std::filesystem::path& path, TopicsSubscriberID sid)
    {
        GetInstance().m_subscribers.Unsubscribe(path, sid);
        GetInstance().m_FileLastWriteTimes.erase(path);
    }

    static std::filesystem::path GetScriptFilePath(std::string_view file_name) { return SCRIPT_ROOT_PATH / file_name; }
    static std::string GetScriptFilePathString(std::string_view file_name) { return (SCRIPT_ROOT_PATH / file_name).string(); }

private:
    ScriptManager();
    ~ScriptManager() = default;

    Topics<std::filesystem::path, FileChangeHandler_t> m_subscribers;

    bool DetectChanges();

    sol::object LuaRequire(sol::string_view, sol::environment&);

private:
    sol::state m_LuaState;
    sol::function m_fnRequirer;
    std::unordered_map<std::filesystem::path, std::filesystem::file_time_type> m_FileLastWriteTimes;
};
}
