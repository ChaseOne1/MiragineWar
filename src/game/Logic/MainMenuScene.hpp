#pragma once
#include "app/ScriptContext.hpp"
#include "utility/Singleton.hpp"

namespace game::logic {
class MainMenuScene final : public utility::Singleton<MainMenuScene>, app::ScriptContext
{
    friend class utility::Singleton<MainMenuScene>;

private:
    MainMenuScene();
    ~MainMenuScene() override = default;
};
}
