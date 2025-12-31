#pragma once

#include "sol/environment.hpp"

namespace app {
class ScriptContext
{
public:
    explicit ScriptContext(std::string_view script_name);
    virtual ~ScriptContext() = default;

protected:
    sol::environment m_LuaEnv;
    const std::string_view m_ScriptFileName;

protected:
    virtual void OnScriptFileChanged();
};
} // app