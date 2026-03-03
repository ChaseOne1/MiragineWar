#pragma once
#include "utility/Topics.hpp"

namespace app {
class ScriptManager : public utility::Singleton<ScriptManager>
{
    friend class Singleton<ScriptManager>;
    struct FileWatchInfo
    {
        std::filesystem::file_time_type last_write_time;
        std::string module_name;
    };

public:
    inline static const std::filesystem::path SCRIPT_ROOT_PATH = "scripts/";

public:
    static sol::state& GetLuaState() noexcept { return GetInstance().m_LuaState; }
    static std::filesystem::path GetScriptFilePath(std::string_view file_name) { return SCRIPT_ROOT_PATH / file_name; }
    static std::string GetScriptFilePathString(std::string_view file_name) { return (SCRIPT_ROOT_PATH / file_name).string(); }

private:
    ScriptManager();
    ~ScriptManager() = default;

    bool DetectChanges();

    sol::object LuaRequire(sol::string_view, sol::environment&);

private:
    sol::state m_LuaState;
    sol::function m_fnRequirer;
    std::unordered_map<std::filesystem::path, FileWatchInfo> m_FileWatchInfos;
};
}
