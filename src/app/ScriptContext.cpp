#include "ScriptContext.hpp"
#include "ScriptManager.hpp"

using namespace app;

ScriptContext::ScriptContext(std::string_view script_filename)
    : m_LuaEnv(app::ScriptManager::GetLuaState(), sol::create, app::ScriptManager::GetLuaState().globals())
    , m_ScriptFileName(script_filename)
{
    sol::load_result script { app::ScriptManager::GetLuaState().load_file(app::ScriptManager::GetScriptFilePathString(m_ScriptFileName)) };
    if (!script.valid()) {
        SDL_Log("load %s failed, status: %d", m_ScriptFileName.data(), script.status());
        throw std::runtime_error { "failed to load the file" };
    }

    sol::protected_function script_func = script.get<sol::protected_function>();
    sol::set_environment(m_LuaEnv, script_func);
    sol::protected_function_result result = script_func();
    if (!result.valid()) {
        SDL_Log("do %s failed, status: %d", m_ScriptFileName.data(), result.status());
        throw std::runtime_error { "failed to script the file" };
    }
}

void ScriptContext::Initialize()
{
    // TODO: safety check
    m_LuaEnv[SLM_Initialize]();
}
