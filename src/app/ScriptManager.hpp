#pragma once

namespace app {
class ScriptManager : public utility::Singleton<ScriptManager>
{
    friend class Singleton<ScriptManager>;

public:
    inline static const std::filesystem::path SCRIPT_ROOT_PATH = "scripts/";

public:
    static sol::state& GetLuaState() noexcept { return GetInstance().m_LuaState; }
    static std::string GetScriptFilePath(std::string_view file_name){ return (SCRIPT_ROOT_PATH / file_name).string(); }
private:
    ScriptManager();
    ~ScriptManager() = default;

private:
    sol::state m_LuaState;
};
}
