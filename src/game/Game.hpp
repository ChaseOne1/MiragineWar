#pragma once
#include "utility/ScriptContext.hpp"
#include "utility/Registry.hpp"

namespace game {
class Game : public utility::Singleton<Game>
{
    friend class utility::Singleton<Game>;

private:
    entt::entity m_Game = utility::Registry::GetInstance().GetRegistry().create();
    utility::ScriptContext m_ScriptContext;

private:
    Game();
    ~Game() = default;
};
}
