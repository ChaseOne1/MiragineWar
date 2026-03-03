#pragma once

namespace app {
class ScriptContext
{
public:
    explicit ScriptContext(std::string_view script_name);
    virtual ~ScriptContext() = default;


    // standard local method
    static constexpr std::string_view SLM_Initialize = "init";
    static constexpr std::string_view SLM_Reload = "reload";

protected:
    sol::environment m_LuaEnv;
    const std::string_view m_ScriptFileName;

    void Initialize();
};
} // app
