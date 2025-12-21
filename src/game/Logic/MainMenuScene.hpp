#pragma once
#include "utility/Singleton.hpp"

namespace game::logic {
class MainMenuScene : public utility::Singleton<MainMenuScene>
{
    friend class utility::Singleton<MainMenuScene>;

private:
    sol::environment m_LuaEnv;
    sol::load_result m_LuaScript;

private:
    MainMenuScene();
    ~MainMenuScene() = default;

    void OnScriptFileChanged();
};
}
