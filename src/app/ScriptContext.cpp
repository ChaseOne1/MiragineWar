#include "ScriptContext.hpp"
#include "ScriptManager.hpp"

using namespace app;

ScriptContext::ScriptContext(std::string_view script_name)
    : m_LuaEnv(app::ScriptManager::GetLuaState(), sol::create, app::ScriptManager::GetLuaState().globals())
    , m_ScriptFileName(script_name)
{
    // inject meta information for some usages
    m_LuaEnv["Module"] = app::ScriptManager::GetLuaState().create_table_with("version", 0, "name", m_ScriptFileName);

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

    app::ScriptManager::Subscribe(app::ScriptManager::GetScriptFilePath(m_ScriptFileName), [this] { this->OnScriptFileChanged(); });
}

void ScriptContext::OnScriptFileChanged()
{
    sol::load_result script = app::ScriptManager::GetLuaState().load_file(app::ScriptManager::GetScriptFilePathString(m_ScriptFileName));
    if (!script.valid()) {
        SDL_Log("reload %s failed, status: %d", m_ScriptFileName.data(), script.status());
        return;
    }

    // here we can call directly because the function is at the top of the stack,
    // otherwise we will get an exception related to calling nil
    m_LuaEnv[app::ScriptManager::SLM_PreReload]();

    sol::protected_function script_func = script.get<sol::protected_function>();
    sol::set_environment(m_LuaEnv, script_func);
    sol::protected_function_result result = script_func();
    if (!result.valid()) {
        SDL_Log("do %s failed when reload, status: %d", m_ScriptFileName.data(), result.status());
        throw std::runtime_error { "failed to script the file when reload" };
    }

    m_LuaEnv[app::ScriptManager::SLM_PostReload]();
}
