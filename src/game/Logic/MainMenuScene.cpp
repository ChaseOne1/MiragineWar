#include "MainMenuScene.hpp"
#include "game/System/Scene.hpp"
#include "app/ScriptManager.hpp"

using namespace game::logic;

static constexpr std::string_view gs_ScriptFileName("MainMenuScene.lua");

MainMenuScene::MainMenuScene()
    : ScriptContext(gs_ScriptFileName)
{
    m_LuaEnv[app::ScriptManager::SLM_Initialize]();
    game::sys::Scene::GetInstance();
}
