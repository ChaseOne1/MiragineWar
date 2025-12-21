#include "MainMenuScene.hpp"
#include "game/System/Scene.hpp"
#include "app/ScriptManager.hpp"

using namespace game::logic;

static constexpr std::string_view gs_ScriptFileName("main_menu_scene.lua");

MainMenuScene::MainMenuScene()
    : m_LuaEnv(app::ScriptManager::GetLuaState(), sol::create, app::ScriptManager::GetLuaState().globals())
{
    sol::load_result script { app::ScriptManager::GetLuaState().load_file(app::ScriptManager::GetScriptFilePathString(gs_ScriptFileName)) };
    if (!script.valid()) {
        SDL_Log("load %s failed, status: %d", gs_ScriptFileName.data(), m_LuaScript.status());
        throw std::runtime_error { "failed to load the file" };
    }

    sol::protected_function script_func = script.get<sol::protected_function>();
    sol::set_environment(m_LuaEnv, script_func);
    sol::protected_function_result result = script_func();
    if (!result.valid()) {
        SDL_Log("do file %s failed, status: %d", gs_ScriptFileName.data(), result.status());
        throw std::runtime_error { "failed to script the file" };
    }
    app::ScriptManager::Subscribe(app::ScriptManager::GetScriptFilePath(gs_ScriptFileName), [this]() { this->OnScriptFileChanged(); });

    m_LuaEnv[app::ScriptManager::SCM_Initialize]();
    game::sys::Scene::GetInstance();
}

void MainMenuScene::OnScriptFileChanged()
{
    sol::load_result script = app::ScriptManager::GetLuaState().load_file(app::ScriptManager::GetScriptFilePathString(gs_ScriptFileName));
    if (!script.valid()) {
        SDL_Log("reload %s failed, status: %d", gs_ScriptFileName.data(), script.status());
        return;
    }

    m_LuaEnv[app::ScriptManager::SCM_PreReload]();

    sol::protected_function script_func = script.get<sol::protected_function>();
    sol::set_environment(m_LuaEnv, script_func);
    sol::protected_function_result result = script();
    if (!result.valid()) {
        SDL_Log("do file %s failed when reload, status: %d", gs_ScriptFileName.data(), result.status());
        throw std::runtime_error { "failed to script the file when reload" };
    }

    m_LuaEnv[app::ScriptManager::SCM_PostReload]();
}
