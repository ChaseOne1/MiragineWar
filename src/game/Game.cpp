#include "Game.hpp"
#include "game/System/Scene.hpp"

using namespace game;

Game::Game()
    : app::ScriptContext("main.lua")
{
    game::sys::Scene::GetInstance();
    Initialize();
}
