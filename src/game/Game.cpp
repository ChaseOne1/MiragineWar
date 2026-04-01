#include "Game.hpp"

using namespace game;

Game::Game()
    : m_ScriptContext("main.lua")
{
    m_ScriptContext.RegisterTypes();
    m_ScriptContext.RegisterEntity();
    m_ScriptContext.RegisterComponents();
    m_ScriptContext.RegisterScriptLib();

    if (const auto result = m_ScriptContext.m_this_module[utility::ScriptContext::SMI_Initialize]
            .get<sol::protected_function>()();
        !result.valid()) {
        LOGE("Error occured: status = {}, reason = {}",
            static_cast<std::size_t>(result.status()), result.get<sol::error>().what());
    }
}
