#include "MainMenuScene.hpp"

#include "app/EventBus.hpp"
#include "app/Layout.hpp"
#include "app/Resources.hpp"
#include "game/System/Scene.hpp"
#include "app/ScriptManager.hpp"
#include "utility/Registry.hpp"
#include "app/ScriptLib.hpp"

using namespace game::logic;

static constexpr std::string_view gs_ScriptFileName("MainMenuScene.lua");

MainMenuScene::MainMenuScene()
    : ScriptContext(gs_ScriptFileName)
{
    utility::Registry::GetInstance().RegisterToEnv(m_LuaEnv);
    app::Resources::GetInstance().RegisterToEnv(m_LuaEnv);
    app::EventBus::GetInstance().RegisterToEnv(m_LuaEnv);
    app::Layout::GetInstance().RegisterToEnv(m_LuaEnv);
    app::ScriptLib::GetInstance().RegisterToEnv(m_LuaEnv);

    m_LuaEnv[app::ScriptManager::SLM_Initialize]();
    game::sys::Scene::GetInstance();
}
